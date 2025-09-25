#ifndef PLAYERPROFILEPAGE_H
#define PLAYERPROFILEPAGE_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QMap>
#include "databasemanager.h"
#include "metricscalculator.h"

// Структура для зберігання агрегованої статистики гравця
struct PlayerProfile {
    QString playerName;
    int battleCount = 0;
    double totalDamage = 0;
    double totalAssisted = 0;
    double totalBlocked = 0;
    double totalKills = 0;
    QMap<QString, int> battlesByVehicleType;
    QMap<QString, double> damageByVehicleType;

    double avgDamage() const { return battleCount > 0 ? totalDamage / battleCount : 0; }
};

class PlayerProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent = nullptr);

public slots:
    void refreshProfile(); // Публічний слот для оновлення ззовні

private slots:
    void recalculateAndDisplay(); // Приватний слот для реакції на зміни даних
    void onPlayerSelected(const QString& playerName);

private:
    void setupUI();
    void generateProfileReport(const PlayerProfile& profile);

    DatabaseManager *m_dbManager;
    MetricsCalculator *m_metricsCalculator;
    QMap<QString, PlayerProfile> m_profiles;

    QComboBox *m_playerSelector;
    QTextEdit *m_profileDisplay;
};

#endif // PLAYERPROFILEPAGE_H
