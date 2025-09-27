#include "ReplayParser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDataStream>
#include <QDebug>
#include <QtGlobal>
#include "blowfish_tables.h" // Переконайтеся, що цей файл підключено

// ======================================================================
// === ВБУДОВАНА РЕАЛІЗАЦІЯ BLOWFISH (залишається без змін) =============
// ======================================================================
namespace LightweightBlowfish {
// Структура для зберігання ключів
struct BlowfishContext {
    quint32 p[18];
    quint32 s[4][256];
};

void blowfish_encrypt(BlowfishContext* ctx, quint32* xl, quint32* xr) {
    for (int i = 0; i < 16; ++i) {
        *xl ^= ctx->p[i];
        *xr ^= ((ctx->s[0][*xl >> 24] + ctx->s[1][(*xl >> 16) & 0xff]) ^ ctx->s[2][(*xl >> 8) & 0xff]) + ctx->s[3][*xl & 0xff];
        quint32 temp = *xl; *xl = *xr; *xr = temp;
    }
    quint32 temp = *xl; *xl = *xr; *xr = temp;
    *xr ^= ctx->p[16];
    *xl ^= ctx->p[17];
}

void blowfish_decrypt(BlowfishContext* ctx, quint32* xl, quint32* xr) {
    for (int i = 17; i > 1; --i) {
        *xl ^= ctx->p[i];
        *xr ^= ((ctx->s[0][*xl >> 24] + ctx->s[1][(*xl >> 16) & 0xff]) ^ ctx->s[2][(*xl >> 8) & 0xff]) + ctx->s[3][*xl & 0xff];
        quint32 temp = *xl; *xl = *xr; *xr = temp;
    }
    quint32 temp = *xl; *xl = *xr; *xr = temp;
    *xr ^= ctx->p[1];
    *xl ^= ctx->p[0];
}

void blowfish_init(BlowfishContext* ctx, const unsigned char* key, int keyLen) {
    memcpy(ctx->p, initial_p, sizeof(initial_p));
    memcpy(ctx->s, initial_s, sizeof(initial_s));

    int j = 0;
    for (int i = 0; i < 18; ++i) {
        quint32 data = 0;
        for (int k = 0; k < 4; ++k) {
            data = (data << 8) | key[j];
            j = (j + 1) % keyLen;
        }
        ctx->p[i] ^= data;
    }

    quint32 l = 0, r = 0;
    for (int i = 0; i < 18; i += 2) {
        blowfish_encrypt(ctx, &l, &r);
        ctx->p[i] = l;
        ctx->p[i + 1] = r;
    }

    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < 256; k += 2) {
            blowfish_encrypt(ctx, &l, &r);
            ctx->s[i][k] = l;
            ctx->s[i][k + 1] = r;
        }
    }
}
} // namespace LightweightBlowfish

// --- Статичний ключ шифрування ---
static const unsigned char BLOWFISH_KEY[16] = {
    0xDE, 0x72, 0xBE, 0xA0, 0xDE, 0x04, 0xBE, 0xB1,
    0xDE, 0xFE, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF
};

ReplayParser::ReplayParser(QObject *parent) : QObject(parent) {}

QByteArray ReplayParser::decryptStream(const QByteArray &encryptedData)
{
    const int blockSize = 8;
    if (encryptedData.isEmpty() || encryptedData.size() % blockSize != 0) {
        qWarning() << "Помилка розшифровки: некоректний розмір даних.";
        return QByteArray();
    }

    LightweightBlowfish::BlowfishContext ctx;
    LightweightBlowfish::blowfish_init(&ctx, BLOWFISH_KEY, sizeof(BLOWFISH_KEY));

    QByteArray decryptedData;
    decryptedData.reserve(encryptedData.size());
    QByteArray previousEncryptedBlock(blockSize, 0x00);

    for (int i = 0; i < encryptedData.size(); i += blockSize) {
        quint32 xl, xr;
        memcpy(&xl, encryptedData.constData() + i, 4);
        memcpy(&xr, encryptedData.constData() + i + 4, 4);

        xl = qFromLittleEndian(xl);
        xr = qFromLittleEndian(xr);

        LightweightBlowfish::blowfish_decrypt(&ctx, &xl, &xr);

        xl = qToLittleEndian(xl);
        xr = qToLittleEndian(xr);

        QByteArray currentDecryptedBlock(blockSize, 0x00);
        memcpy(currentDecryptedBlock.data(), &xl, 4);
        memcpy(currentDecryptedBlock.data() + 4, &xr, 4);

        for (int j = 0; j < blockSize; ++j) {
            currentDecryptedBlock[j] ^= previousEncryptedBlock[j];
        }

        decryptedData.append(currentDecryptedBlock);
        previousEncryptedBlock = encryptedData.mid(i, blockSize);
    }

    qDebug() << "Дані успішно розшифровано. Розмір:" << decryptedData.size();
    return decryptedData;
}


QVariantMap ReplayParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не вдалося відкрити файл:" << filePath;
        return QVariantMap();
    }

    QVariantMap fullData;
    quint32 magic;
    quint32 blockCount;

    if (file.read(reinterpret_cast<char*>(&magic), sizeof(magic)) != sizeof(magic) ||
        file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount)) != sizeof(blockCount)) {
        qWarning() << "Некоректний формат файлу реплею.";
        return QVariantMap();
    }

    QByteArray binaryStreamData;

    for (quint32 i = 0; i < blockCount; ++i) {
        quint32 blockSize;
        if (file.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize)) != sizeof(blockSize)) {
            qWarning() << "Не вдалося прочитати розмір блоку" << i;
            break;
        }

        QByteArray blockData = file.read(blockSize);
        if (blockData.size() != (int)blockSize) {
            qWarning() << "Не вдалося прочитати дані блоку" << i;
            break;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(blockData, &parseError);

        if (parseError.error == QJsonParseError::NoError && (doc.isObject() || doc.isArray())) {
            if (doc.isObject()) {
                QVariantMap parsedData = doc.object().toVariantMap();
                for (auto it = parsedData.constBegin(); it != parsedData.constEnd(); ++it) {
                    fullData.insert(it.key(), it.value());
                }
            } else {
                QVariantList parsedList = doc.array().toVariantList();
                if (!parsedList.isEmpty() && parsedList.first().canConvert<QVariantMap>()) {
                    QVariantMap parsedData = parsedList.first().toMap();
                    for (auto it = parsedData.constBegin(); it != parsedData.constEnd(); ++it) {
                        fullData.insert(it.key(), it.value());
                    }
                }
            }
        } else {
            binaryStreamData = blockData;
        }
    }

    if (!binaryStreamData.isEmpty()) {
        QByteArray decryptedStream = decryptStream(binaryStreamData);
        if (!decryptedStream.isEmpty()) {
            QByteArray decompressedStream = qUncompress(decryptedStream);
            if (!decompressedStream.isEmpty()) {
                qDebug() << "Потік успішно розпаковано! Розмір:" << decompressedStream.size() << "байт.";
                parsePackets(decompressedStream, fullData);
            } else {
                qWarning() << "Не вдалося розпакувати потік.";
            }
        }
    }

    file.close();
    fullData["filePath"] = filePath;
    return fullData;
}
QStringList decodeCriticalFlags(quint32 flags) {
    QStringList crits;
    // Мапа "біт -> назва модуля/члена екіпажу"
    static const QMap<int, QString> critMap = {
        {0, "знищено"}, {1, "пошкоджено двигун"}, {2, "пошкоджено боєукладку"},
        {3, "пошкоджено паливний бак"}, {4, "пожежа"}, {5, "пошкоджено триплекс"},
        {6, "пошкоджено гармату"}, {7, "пошкоджено ходову"}, {8, "контужено командира"},
        {9, "контужено навідника"}, {10, "контужено водія"},
        {11, "контужено радиста"}, {12, "контужено заряджаючого"}
    };

    for (auto it = critMap.constBegin(); it != critMap.constEnd(); ++it) {
        if ((flags >> it.key()) & 1) {
            crits.append(it.value());
        }
    }
    return crits;
}

void ReplayParser::parsePackets(const QByteArray &stream, QVariantMap &out_data)
{
    qDebug() << "Починаємо детальний парсинг пакетів...";
    QDataStream streamReader(stream);
    streamReader.setByteOrder(QDataStream::LittleEndian);

    quint32 recorderPlayerId = out_data.value("playerID", 0).toUInt();
    if (recorderPlayerId == 0) {
        qWarning() << "Не вдалося визначити ID гравця для відстеження.";
        return;
    }

    // --- Збираємо дані про гравців ---
    QMap<quint32, QString> playerNames;
    QMap<quint32, int> playerTeams;
    if (out_data.contains("vehicles")) {
        QVariantMap vehicles = out_data.value("vehicles").toMap();
        for (auto it = vehicles.constBegin(); it != vehicles.constEnd(); ++it) {
            QVariantMap v = it.value().toMap();
            // ВАЖЛИВО: ID в пакетах - це не accountDBID, а динамічний ID сутності в бою (ключ мапи vehicles)
            quint32 entityId = it.key().toUInt();
            playerNames[entityId] = v.value("name").toString();
            playerTeams[entityId] = v.value("team").toInt();
        }
    }

    QVariantList playerPositions;
    QVariantList shotEvents;

    while (!streamReader.atEnd()) {
        if (stream.size() - streamReader.device()->pos() < 9) break;

        quint8 type;
        float timestamp;
        quint32 payloadSize;
        streamReader >> type >> timestamp >> payloadSize;

        long currentPosBeforePayload = streamReader.device()->pos();
        if (stream.size() - currentPosBeforePayload < (int)payloadSize) {
            qWarning() << "Помилка парсингу: неочікуваний кінець потоку.";
            break;
        }

        QByteArray payload = stream.mid(currentPosBeforePayload, payloadSize);

        switch (type) {
        case 0x0A: { // Позиції (без змін)
            // ... ваш існуючий код для позицій ...
            break;
        }

        case 0x08: { // Виклик методів (нас цікавить шкода)
            if (payloadSize < 12) break;
            QDataStream payloadReader(payload);
            payloadReader.setByteOrder(QDataStream::LittleEndian);

            quint32 entityId, methodId, argSize;
            payloadReader >> entityId >> methodId >> argSize;

            // ID методу 'receiveDamage' може змінюватися, але зазвичай знаходиться в цьому діапазоні
            // для сутності Vehicle. 105 - найчастіший варіант.
            if (methodId >= 90 && methodId <= 110 && payloadSize >= 22) {
                payloadReader.skipRawData(8); // Пропускаємо опис аргументів (pickle'd tuple)

                quint32 attackerId;
                quint16 damage, sourceId, shellId, flags;

                payloadReader >> attackerId >> damage >> sourceId >> shellId >> flags;

                ShotEvent event;
                event.timestamp = timestamp;
                event.attackerId = attackerId;
                event.targetId = entityId;
                event.damage = damage;

                event.attackerName = playerNames.value(attackerId, "Невідомо");
                event.targetName = playerNames.value(entityId, "Невідомо");

                // Розшифровуємо прапори пострілу
                event.isPenetration = (flags & 0x02) != 0;
                event.isRicochet = (flags & 0x40) != 0;
                event.isShellExplosion = (flags & 0x04) != 0; // Сплеш
                event.isNoDamage = !event.isPenetration && !event.isRicochet && damage == 0;

                // Перевірка на вогонь по своїх
                if (playerTeams.contains(attackerId) && playerTeams.contains(entityId)) {
                    event.isFriendlyFire = (playerTeams[attackerId] == playerTeams[entityId]);
                }

                // Розшифровуємо критичні пошкодження (старші біти прапора flags)
                event.criticalHits = decodeCriticalFlags(flags >> 8);

                shotEvents.append(event.toVariantMap());
            }
            break;
        }
        }
        streamReader.device()->seek(currentPosBeforePayload + payloadSize);
    }

    out_data["player_positions"] = playerPositions;
    out_data["shot_events"] = shotEvents;
    qDebug() << "Парсинг завершено. Знайдено" << playerPositions.size() << "записів про позицію та" << shotEvents.size() << "подій шкоди.";
}
