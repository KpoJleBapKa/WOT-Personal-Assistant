#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QString>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);

    bool loadData();
    bool saveData();
    void addReplayData(const QString &filePath, const QVariantMap &data);
    QVariantMap getReplayData(const QString &filePath);
    bool clearAllData();
    QVariantList getReplays();

private:
    QString m_dataFilePath;
    QVariantList m_replays;
};

#endif // DATABASEMANAGER_H
