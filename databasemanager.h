#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QString>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);

    void addReplayData(const QString &filePath, const QVariantMap &data);
    QVariantMap getReplayData(const QString &filePath);
    QVariantList getReplays();
    bool clearAllData(); // 🔹 Оголошення методу clearAllData()
Q_SIGNALS:
    void dataChanged(); // Сигнал про те, що дані реплеїв змінилися

private:
    bool loadData();
    bool saveData();

    QVariantList m_replays;
    QString m_dataFilePath;
};

#endif // DATABASEMANAGER_H
