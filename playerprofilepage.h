#ifndef PLAYERPROFILEPAGE_H
#define PLAYERPROFILEPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QTextEdit>
#include <QChartView>
#include <QTabWidget>
#include <QProgressBar>
#include <QMap>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QDateTime>

class DatabaseManager;
class MetricsCalculator;

struct PlayerProfile {
    QString playerName;
    int battleCount = 0;
    int wins = 0;
    int survivedBattles = 0;
    
    double totalDamage = 0;
    double totalAssisted = 0;
    double totalBlocked = 0;
    double totalKills = 0;
    double totalSpotted = 0;
    double totalExperience = 0;
    double totalCredits = 0;
    
    double maxDamage = 0;
    double maxKills = 0;
    double maxAssisted = 0;
    double maxBlocked = 0;
    double maxExperience = 0;
    
    QMap<QString, int> battlesByVehicleType;
    QMap<QString, double> damageByVehicleType;
    QMap<QString, double> experienceByVehicleType;
    QMap<QString, int> winsByVehicleType;
    
    QMap<QString, int> battlesByMap;
    QMap<QString, int> winRateByMap;
    
    QList<double> recentDamage;
    QList<double> recentWinRate;
    QList<QDateTime> battleDates;
    
    QMap<QString, double> avgDamageByMap;
    QMap<QString, double> avgExperienceByMap;
    QMap<QString, int> battlesByTier;
    QMap<QString, double> avgDamageByTier;
    
    // Calculated properties
    double winRate() const { return battleCount > 0 ? (wins * 100.0 / battleCount) : 0; }
    double avgDamage() const { return battleCount > 0 ? (totalDamage / battleCount) : 0; }
    double avgKills() const { return battleCount > 0 ? (totalKills / battleCount) : 0; }
    double avgSpotted() const { return battleCount > 0 ? (totalSpotted / battleCount) : 0; }
    double avgExperience() const { return battleCount > 0 ? (totalExperience / battleCount) : 0; }
    double avgCredits() const { return battleCount > 0 ? (totalCredits / battleCount) : 0; }
    double avgAssisted() const { return battleCount > 0 ? (totalAssisted / battleCount) : 0; }
    double avgBlocked() const { return battleCount > 0 ? (totalBlocked / battleCount) : 0; }
    double survivalRate() const { return battleCount > 0 ? (survivedBattles * 100.0 / battleCount) : 0; }
    
    double wn8() const;
    double efficiency() const;
    QString skillLevel() const;
};

class PlayerProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent = nullptr);
    void refreshProfile();

private slots:
    void onPlayerSelected(const QString &playerName);
    void recalculateAndDisplay();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupTabs();
    void setupOverviewTab();
    void setupStatsTab();
    void setupAnalysisTab();
    void displayProfile(const PlayerProfile &profile);
    void updateOverviewTab(const PlayerProfile &profile);
    void updateStatsTab(const PlayerProfile &profile);
    void updateAnalysisTab(const PlayerProfile &profile);

    DatabaseManager *m_dbManager;
    MetricsCalculator *m_metricsCalculator;
    
    QMap<QString, PlayerProfile> m_profiles;
    
    // UI elements
    QComboBox *m_playerSelector;
    QTabWidget *m_tabWidget;
    
    // Overview Tab
    QLabel *m_battlesLabel;
    QLabel *m_winRateLabel;
    QLabel *m_avgDamageLabel;
    QLabel *m_wn8Label;
    QLabel *m_efficiencyLabel;
    QLabel *m_survivalLabel;
    QChartView *m_chartView;
    QTableWidget *m_classStatsTable;
    
    // Stats Tab
    QTableWidget *m_detailedStatsTable;
    
    // Analysis Tab
    QTextEdit *m_analysisText;
    QTextEdit *m_recommendationsText;
};

#endif // PLAYERPROFILEPAGE_H