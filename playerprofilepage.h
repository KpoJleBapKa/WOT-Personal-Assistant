#ifndef PLAYERPROFILEPAGE_H
#define PLAYERPROFILEPAGE_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QTextEdit>

#include "databasemanager.h"
#include "metricscalculator.h"

// Qt Charts (для Qt6)
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>

// РОЗШИРЕНА структура для зберігання агрегованої статистики
struct PlayerProfile {
    QString playerName;
    int battleCount = 0;
    int wins = 0;
    double totalDamage = 0;
    double totalAssisted = 0;
    double totalBlocked = 0;
    double totalKills = 0;

    // Рекорди
    double maxDamage = 0;
    double maxKills = 0;
    double maxAssisted = 0;
    double maxBlocked = 0;

    QMap<QString, int> battlesByVehicleType;
    QMap<QString, double> damageByVehicleType;

    // Розрахункові властивості
    double avgDamage() const { return battleCount > 0 ? totalDamage / battleCount : 0; }
    double winRate() const { return battleCount > 0 ? (static_cast<double>(wins) / battleCount) * 100.0 : 0; }
};

class PlayerProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent = nullptr);

public slots:
    void refreshProfile();

private slots:
    void recalculateAndDisplay();
    void onPlayerSelected(const QString& playerName);

private:
    void setupUI();
    void displayProfile(const PlayerProfile& profile);

    DatabaseManager *m_dbManager;
    MetricsCalculator *m_metricsCalculator;
    QMap<QString, PlayerProfile> m_profiles;

    // UI елементи
    QComboBox *m_playerSelector;
    QLabel *m_battlesLabel;
    QLabel *m_winRateLabel;
    QLabel *m_avgDamageLabel;

    QTableWidget *m_classStatsTable;
    QChartView *m_chartView; // Тепер без QtCharts::
    QTextEdit *m_analysisText; // Для аналітичних висновків
};

#endif // PLAYERPROFILEPAGE_H
