#include "ReplayParser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject> // Додано для коректної роботи з QJsonDocument

ReplayParser::ReplayParser(QObject *parent) : QObject{parent} {}

QVariantMap ReplayParser::parse(const QString &filePath)
{
    QByteArray jsonBytes = readJsonBlock(filePath);
    if (jsonBytes.isEmpty()) {
        QMessageBox::critical(nullptr, "Помилка парсингу", "Не вдалося прочитати JSON-блок або файл порожній.");
        return QVariantMap();
    }

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(nullptr, "Помилка парсингу", "Помилка парсингу JSON: " + parseError.errorString());
        return QVariantMap();
    }

    if (!jsonDoc.isObject()) {
        QMessageBox::critical(nullptr, "Помилка парсингу", "JSON-документ не є об'єктом.");
        return QVariantMap();
    }

    return jsonDoc.object().toVariantMap();
}

QByteArray ReplayParser::readJsonBlock(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося відкрити файл: " + filePath);
        return QByteArray();
    }

    file.seek(4);
    quint32 blockSize;
    if (file.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize)) != sizeof(blockSize)) {
        return QByteArray();
    }
    file.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

    QByteArray jsonBlock = file.read(blockSize);
    file.close();

    return qUncompress(jsonBlock);
}
