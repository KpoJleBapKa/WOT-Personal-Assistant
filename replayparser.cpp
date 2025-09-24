#include "ReplayParser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>

ReplayParser::ReplayParser(QObject *parent) : QObject(parent) {}

QVariantMap ReplayParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося відкрити файл: " + filePath);
        return QVariantMap();
    }

    QVariantMap fullData;
    quint32 magic;
    quint32 blockCount;

    // Зчитуємо заголовок (магічне число і кількість блоків)
    if (file.read(reinterpret_cast<char*>(&magic), sizeof(magic)) != sizeof(magic) ||
        file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount)) != sizeof(blockCount)) {
        QMessageBox::critical(nullptr, "Помилка", "Некоректний формат файлу реплею.");
        return QVariantMap();
    }

    qDebug() << "Знайдено блоків:" << blockCount;

    // Цикл для обробки кожного блоку даних
    for (quint32 i = 0; i < blockCount; ++i) {
        quint32 blockSize;
        if (file.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize)) != sizeof(blockSize)) {
            qWarning() << "Помилка: Не вдалося прочитати розмір блоку" << i;
            break;
        }

        QByteArray blockData = file.read(blockSize);
        if (blockData.isEmpty()) {
            qWarning() << "Помилка: Блок" << i << "пустий.";
            continue;
        }

        QJsonParseError parseError;
        QByteArray decompressedData = qUncompress(blockData);
        QJsonDocument doc;
        bool isDecompressed = false;

        if (!decompressedData.isEmpty()) {
            doc = QJsonDocument::fromJson(decompressedData, &parseError);
            isDecompressed = true;
        } else {
            doc = QJsonDocument::fromJson(blockData, &parseError);
        }

        if (parseError.error == QJsonParseError::NoError) {
            QVariantMap parsedData = doc.object().toVariantMap();
            for (auto it = parsedData.constBegin(); it != parsedData.constEnd(); ++it) {
                fullData.insert(it.key(), it.value());
            }
            qDebug() << "Успішно спарсено JSON-блок" << i + 1 << "(стиснений:" << isDecompressed << ")";
        } else {
            qDebug() << "Блок" << i + 1 << "не є валідним JSON. Пропускаємо.";
        }
    }

    file.close();

    if (fullData.isEmpty()) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося витягти дані з реплею. Файл може бути пошкодженим або несумісним.");
    }

    fullData["filePath"] = filePath;
    return fullData;
}
