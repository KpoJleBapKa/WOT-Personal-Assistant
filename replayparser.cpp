#include "ReplayParser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QDebug>

ReplayParser::ReplayParser(QObject *parent) : QObject(parent) {}

QVariantMap ReplayParser::parse(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося відкрити файл: " + filePath);
        return QVariantMap();
    }

    QVariantMap fullData;

    // Зчитуємо магічне число (4 байти)
    quint32 magic;
    if (file.read(reinterpret_cast<char*>(&magic), sizeof(magic)) != sizeof(magic)) {
        QMessageBox::critical(nullptr, "Помилка", "Некоректний формат файлу (magic).");
        return QVariantMap();
    }

    // Зчитуємо кількість блоків
    quint32 blockCount;
    if (file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount)) != sizeof(blockCount)) {
        QMessageBox::critical(nullptr, "Помилка", "Некоректний формат файлу (block count).");
        return QVariantMap();
    }

    // Зчитуємо розмір першого блоку JSON
    quint32 firstBlockSize;
    if (file.read(reinterpret_cast<char*>(&firstBlockSize), sizeof(firstBlockSize)) != sizeof(firstBlockSize)) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося прочитати розмір першого блоку.");
        return QVariantMap();
    }

    // Зчитуємо сам перший блок
    QByteArray firstBlockData = file.read(firstBlockSize);
    QJsonParseError parseError;
    QJsonDocument doc1 = QJsonDocument::fromJson(firstBlockData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(nullptr, "Помилка парсингу", "Помилка парсингу першого JSON: " + parseError.errorString());
        return QVariantMap();
    }
    fullData = doc1.object().toVariantMap();

    // Якщо є другий блок
    if (blockCount > 1) {
        quint32 secondBlockSize;
        if (file.read(reinterpret_cast<char*>(&secondBlockSize), sizeof(secondBlockSize)) != sizeof(secondBlockSize)) {
            QMessageBox::critical(nullptr, "Помилка", "Не вдалося прочитати розмір другого блоку.");
            return QVariantMap();
        }

        QByteArray secondBlockData = file.read(secondBlockSize);

        // Спробуємо розпакувати, але якщо не вдалося, беремо сирий блок
        QByteArray decompressedSecond = qUncompress(secondBlockData);
        if (decompressedSecond.isEmpty() && !secondBlockData.isEmpty()) {
            qDebug() << "qUncompress не спрацював, використовую сирий блок другого JSON";
            decompressedSecond = secondBlockData;
        }

        QJsonDocument doc2 = QJsonDocument::fromJson(decompressedSecond, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox::critical(nullptr, "Помилка парсингу",
                                  "Помилка парсингу другого JSON: " + parseError.errorString());
            return QVariantMap();
        }

        QVariantMap secondData = doc2.object().toVariantMap();
        for (auto it = secondData.constBegin(); it != secondData.constEnd(); ++it) {
            fullData.insert(it.key(), it.value());
        }
    }

    file.close();
    fullData["filePath"] = filePath;
    return fullData;
}
