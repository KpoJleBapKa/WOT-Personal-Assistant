#include "DatabaseManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    m_dataFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "replays.json";
    QDir dir;
    if (!dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))) {
        qDebug() << "Failed to create application data directory.";
    }
    loadData();
}

bool DatabaseManager::loadData() {
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File not found. Creating new one.";
        saveData();
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "Failed to parse JSON data. Re-creating file.";
        file.close();
        clearAllData();
        return false;
    }
    m_replays = doc.array().toVariantList();
    qDebug() << "Data loaded successfully.";
    file.close();
    return true;
}

bool DatabaseManager::saveData() {
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for writing.";
        return false;
    }

    QJsonArray jsonArray = QJsonArray::fromVariantList(m_replays);
    QJsonDocument doc(jsonArray);
    file.write(doc.toJson(QJsonDocument::Indented));
    qDebug() << "Data saved successfully.";
    file.close();
    emit dataChanged(); // ❗️ Повідомляємо про зміни
    return true;
}

void DatabaseManager::addReplayData(const QString &filePath, const QVariantMap &data) {
    bool found = false;
    for (int i = 0; i < m_replays.size(); ++i) {
        QVariantMap replayMap = m_replays[i].toMap();
        if (replayMap.value("filePath").toString() == filePath) {
            replayMap["analysisData"] = QVariant(data);
            m_replays[i] = replayMap;
            found = true;
            break;
        }
    }
    if (!found) {
        QVariantMap newReplay;
        newReplay["filePath"] = filePath;
        newReplay["analysisData"] = QVariant(data);
        m_replays.append(newReplay);
    }
    saveData();
}

QVariantMap DatabaseManager::getReplayData(const QString &filePath) {
    for (const QVariant &var : m_replays) {
        QVariantMap replayMap = var.toMap();
        if (replayMap.value("filePath").toString() == filePath) {
            return replayMap.value("analysisData").toMap();
        }
    }
    return QVariantMap();
}

bool DatabaseManager::clearAllData() {
    m_replays.clear();
    if (saveData()) {
        QMessageBox::information(nullptr, "Готово", "Всі дані успішно скинуто.");
        return true;
    } else {
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося скинути дані.");
        return false;
    }
}

QVariantList DatabaseManager::getReplays() {
    return m_replays;
}
