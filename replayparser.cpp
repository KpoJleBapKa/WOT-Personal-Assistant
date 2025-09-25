#include "ReplayParser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDataStream>
#include <QDebug>
#include <QtGlobal>

// === ЦЕ ВИПРАВЛЕННЯ: ПЕРЕМІЩУЄМО #include СЮДИ ===
#include "blowfish_tables.h"
// ================================================

// ======================================================================
// === ВБУДОВАНА РЕАЛІЗАЦІЯ BLOWFISH (замість Crypto++) ==================
// ======================================================================
namespace LightweightBlowfish {
// Структура для зберігання ключів
struct BlowfishContext {
    quint32 p[18];
    quint32 s[4][256];
};

// Оригінальні P-блоки та S-блоки тепер підключаються вище

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
        // Копіюємо зашифрований блок
        memcpy(&xl, encryptedData.constData() + i, 4);
        memcpy(&xr, encryptedData.constData() + i + 4, 4);

        // Конвертуємо Little Endian в Host Order
        xl = qFromLittleEndian(xl);
        xr = qFromLittleEndian(xr);

        LightweightBlowfish::blowfish_decrypt(&ctx, &xl, &xr);

        // Конвертуємо назад в Little Endian для запису
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


// ======================================================================
// === РЕШТА КОДУ ЗАЛИШАЄТЬСЯ БЕЗ ЗМІН ===================================
// ======================================================================

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
        if (blockData.size() != blockSize) {
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

    QVariantList playerPositions;

    while (!streamReader.atEnd()) {
        if (stream.size() - streamReader.device()->pos() < 9) break;

        quint8 type;
        float timestamp;
        quint32 payloadSize;
        streamReader >> type >> timestamp >> payloadSize;

        long currentPosBeforePayload = streamReader.device()->pos();
        if (stream.size() - currentPosBeforePayload < payloadSize) {
            qWarning() << "Помилка парсингу: неочікуваний кінець потоку.";
            break;
        }

        if (type == 0x0A && payloadSize >= 21) {
            quint32 playerId;
            float x, y, z;
            streamReader.skipRawData(1);
            streamReader >> playerId;
            streamReader.skipRawData(4);
            streamReader >> x >> y >> z;

            if (playerId == recorderPlayerId) {
                QVariantMap positionData;
                positionData["timestamp"] = timestamp;
                positionData["x"] = x;
                positionData["y"] = y;
                positionData["z"] = z;
                playerPositions.append(positionData);
            }
        }

        streamReader.device()->seek(currentPosBeforePayload + payloadSize);
    }

    out_data["player_positions"] = playerPositions;
    qDebug() << "Парсинг завершено. Знайдено" << playerPositions.size() << "записів про позицію гравця.";
}
