#ifndef BEHAVIORANALYZER_H
#define BEHAVIORANALYZER_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include <QMap>
#include <QVector>

// Розширені структури для складного аналізу поведінки
struct PositionData {
    double x = 0.0, y = 0.0, z = 0.0;  // Координати гравця
    double distanceToEnemies = 0.0;     // Відстань до найближчих ворогів
    double distanceToAllies = 0.0;      // Відстань до союзників
    double mapPosition = 0.0;           // Позиція на карті (0-100)
    QString zone = "unknown";           // Зона карти (фланг, центр, база)
    double coverUsage = 0.0;            // Використання покриття (0-100)
    double mobility = 0.0;              // Мобільність (0-100)
};

struct AdvancedTacticalPattern {
    double aggressiveness = 0.0;        // Агресивність (0-100)
    double defensiveness = 0.0;         // Оборонність (0-100)
    double supportiveness = 0.0;        // Підтримка команди (0-100)
    double positioning = 0.0;           // Позиціонування (0-100)
    double mapControl = 0.0;            // Контроль карти (0-100)
    double initiative = 0.0;            // Ініціатива (0-100)
    
    // Розширені тактичні показники
    double adaptability = 0.0;          // Адаптивність до ситуації (0-100)
    double riskTaking = 0.0;            // Схильність до ризику (0-100)
    double patience = 0.0;              // Терпіння та вичікування (0-100)
    double decisionMaking = 0.0;        // Якість прийняття рішень (0-100)
    double resourceManagement = 0.0;    // Управління ресурсами (0-100)
    double situationalAwareness = 0.0;  // Ситуаційна обізнаність (0-100)
};

struct RoleEffectiveness {
    double damageDealer = 0.0;          // Ефективність як дамаг-ділер
    double scout = 0.0;                 // Ефективність як розвідник
    double support = 0.0;               // Ефективність як підтримка
    double tank = 0.0;                  // Ефективність як танк
    double sniper = 0.0;                // Ефективність як снайпер
    double brawler = 0.0;               // Ефективність в ближньому бою
};

struct TimeBasedAnalysis {
    double earlyGame = 0.0;             // Ефективність в ранній грі (0-100)
    double midGame = 0.0;               // Ефективність в середині гри (0-100)
    double lateGame = 0.0;              // Ефективність в кінці гри (0-100)
    double clutchFactor = 0.0;          // Фактор вирішальних моментів (0-100)
};



struct WeaknessAnalysis {
    QStringList primaryWeaknesses;      // Основні слабкі місця
    QStringList secondaryWeaknesses;    // Другорядні слабкі місця
    QStringList improvementAreas;       // Області для покращення
    double overallRating = 0.0;         // Загальна оцінка (0-100)
};

// Нові структури для складного аналізу
struct MapAnalysis {
    QString mapName;
    QString mapType;                    // "urban", "open", "mixed", "hills"
    double effectiveness = 0.0;         // Ефективність на цій карті (0-100)
    QString preferredZone;              // Улюблена зона карти
    double averagePosition = 0.0;       // Середня позиція на карті
    QVector<PositionData> positionHistory; // Історія позиціонування
};

struct PlayerStatistics {
    QString playerName;
    int totalBattles = 0;
    double winRate = 0.0;
    double averageDamage = 0.0;
    double averageAssists = 0.0;
    double averageKills = 0.0;
    double averageSurvival = 0.0;
    QMap<QString, double> mapEffectiveness; // Ефективність по картах
    QMap<QString, double> vehicleEffectiveness; // Ефективність по техніці
    QVector<QVariantMap> recentBattles; // Останні бої для порівняння
};

struct MachineLearningData {
    QVector<QVector<double>> features;  // Вектори ознак для ML
    QVector<double> targets;            // Цільові значення
    QMap<QString, double> weights;      // Ваги ознак
    double predictionAccuracy = 0.0;    // Точність прогнозування
    QStringList importantFeatures;      // Найважливіші ознаки
};

struct AdvancedCombatEfficiency {
    double accuracy = 0.0;              // Точність стрільби
    double penetrationRate = 0.0;       // Частота пробиттів
    double damagePerShot = 0.0;         // Шкода за постріл
    double survivalRate = 0.0;          // Частота виживання
    double killEfficiency = 0.0;        // Ефективність знищення
    double assistEfficiency = 0.0;      // Ефективність допомоги
    
    // Розширені бойові показники
    double targetSelection = 0.0;       // Вибір цілей (0-100)
    double timing = 0.0;                // Таймінг атак (0-100)
    double ammoEfficiency = 0.0;        // Ефективність використання боєприпасів
    double criticalHitRate = 0.0;       // Частота критичних ударів
    double moduleDamage = 0.0;          // Пошкодження модулів
    double crewDamage = 0.0;            // Пошкодження екіпажу
    double fireRate = 0.0;              // Швидкість стрільби
    double reloadEfficiency = 0.0;      // Ефективність перезарядки
};

struct TeamSynergy {
    double coordination = 0.0;          // Координація з командою (0-100)
    double communication = 0.0;         // Комунікація (0-100)
    double leadership = 0.0;            // Лідерство (0-100)
    double adaptability = 0.0;          // Адаптивність (0-100)
    
    // Розширені показники командної роботи
    double roleFlexibility = 0.0;       // Гнучкість ролей (0-100)
    double supportReadiness = 0.0;      // Готовність підтримати (0-100)
    double initiative = 0.0;            // Ініціатива в команді (0-100)
    double conflictResolution = 0.0;    // Вирішення конфліктів (0-100)
    double resourceSharing = 0.0;       // Обмін ресурсами (0-100)
    double tacticalAwareness = 0.0;     // Тактична обізнаність (0-100)
};

class BehaviorAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit BehaviorAnalyzer(QObject *parent = nullptr);
    QVariantMap analyze(const QVariantMap &replayData, const QVariantMap &metrics);

private:
    // Основні методи аналізу
    AdvancedTacticalPattern analyzeAdvancedTacticalPatterns(const QVariantMap &replayData, const QVariantMap &metrics);
    RoleEffectiveness analyzeRoleEffectiveness(const QVariantMap &replayData, const QVariantMap &metrics);
    TimeBasedAnalysis analyzeTimeBasedPerformance(const QVariantMap &replayData, const QVariantMap &metrics);
    AdvancedCombatEfficiency analyzeAdvancedCombatEfficiency(const QVariantMap &replayData, const QVariantMap &metrics);
    TeamSynergy analyzeTeamSynergy(const QVariantMap &replayData, const QVariantMap &metrics);
    WeaknessAnalysis analyzeWeaknesses(const QVariantMap &replayData, const QVariantMap &metrics);
    
    // Нові складні методи аналізу
    MapAnalysis analyzeMapEffectiveness(const QVariantMap &replayData, const QVariantMap &metrics);
    PositionData analyzePositioning(const QVariantMap &replayData, const QVariantMap &metrics);
    PlayerStatistics loadPlayerStatistics(const QString &playerName);
    MachineLearningData performMachineLearningAnalysis(const QVariantMap &replayData, const QVariantMap &metrics);
    QVariantMap compareWithHistoricalData(const QVariantMap &replayData, const QVariantMap &metrics);
    QVariantMap predictPerformance(const QVariantMap &replayData, const QVariantMap &metrics);
    
    // Допоміжні методи
    double calculateAggressiveness(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateDefensiveness(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateSupportiveness(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculatePositioning(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateMapControl(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateInitiative(const QVariantMap &replayData, const QVariantMap &metrics);
    
    // Методи для аналізу ролей
    double calculateDamageDealerEffectiveness(const QVariantMap &metrics);
    double calculateScoutEffectiveness(const QVariantMap &metrics);
    double calculateSupportEffectiveness(const QVariantMap &metrics);
    double calculateTankEffectiveness(const QVariantMap &metrics);
    double calculateSniperEffectiveness(const QVariantMap &metrics);
    double calculateBrawlerEffectiveness(const QVariantMap &metrics);
    
    // Методи для часового аналізу
    QVector<QVariantMap> getTimeBasedEvents(const QVariantMap &replayData);
    double analyzePhaseEffectiveness(const QVector<QVariantMap> &events, int phase);
    
    // Методи для аналізу командної взаємодії
    double analyzeCommunication(const QVariantMap &replayData);
    double analyzeCoordination(const QVariantMap &replayData);
    double analyzeLeadership(const QVariantMap &replayData);
    double analyzeAdaptability(const QVariantMap &replayData);
    
    // Методи для виявлення слабких місць
    QStringList identifyPrimaryWeaknesses(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics, const RoleEffectiveness &roles);
    QStringList identifySecondaryWeaknesses(const QVariantMap &metrics, const AdvancedCombatEfficiency &combat, const TeamSynergy &team);
    QStringList generateImprovementSuggestions(const WeaknessAnalysis &weaknesses);
    
    // Додаткові допоміжні методи
    double calculateSurvivalTime(const QVariantMap &replayData);
    double calculateSurvivalRate(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateEarlyGameDamage(const QVariantMap &replayData);
    double calculateClutchFactor(const QVector<QVariantMap> &events, const QVariantMap &metrics);
    double calculateOverallRating(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics, const RoleEffectiveness &roles, const AdvancedCombatEfficiency &combat, const TeamSynergy &team);
    
    // Розширені допоміжні методи для складного аналізу
    double calculateAdaptability(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateRiskTaking(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculatePatience(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateDecisionMaking(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateResourceManagement(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateSituationalAwareness(const QVariantMap &replayData, const QVariantMap &metrics);
    
    // Методи для аналізу позиціонування
    PositionData extractPositionData(const QVariantMap &replayData);
    double calculateDistanceToEnemies(const QVariantMap &replayData);
    double calculateDistanceToAllies(const QVariantMap &replayData);
    QString determineMapZone(const PositionData &position, const QString &mapName);
    double calculateCoverUsage(const QVariantMap &replayData);
    double calculateMobility(const QVariantMap &replayData);
    
    // Методи машинного навчання
    QVector<double> extractFeatures(const QVariantMap &replayData, const QVariantMap &metrics);
    double predictWinProbability(const QVector<double> &features);
    QMap<QString, double> calculateFeatureWeights(const QVector<QVector<double>> &features, const QVector<double> &targets);
    QStringList identifyImportantFeatures(const QMap<QString, double> &weights);
    
    // Методи порівняльного аналізу
    double calculatePerformanceDeviation(const QVariantMap &currentBattle, const PlayerStatistics &historicalData);
    QVariantMap generateImprovementRecommendations(const QVariantMap &currentBattle, const PlayerStatistics &historicalData);
    double calculateMapTypeEffectiveness(const QString &mapType, const QVariantMap &metrics);
    double calculateVehicleTypeEffectiveness(const QString &vehicleType, const QVariantMap &metrics);
    
    // Додаткові методи для розширеного аналізу
    QString determineMapType(const QString &mapName);
    QString determinePreferredZone(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateAveragePosition(const QVariantMap &replayData);
    QVector<PositionData> extractPositionHistory(const QVariantMap &replayData);
    double calculateTargetSelection(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateTiming(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateAmmoEfficiency(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateCriticalHitRate(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateModuleDamage(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateCrewDamage(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateFireRate(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateReloadEfficiency(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateRoleFlexibility(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateSupportReadiness(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateTeamInitiative(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateConflictResolution(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateResourceSharing(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateTacticalAwareness(const QVariantMap &replayData, const QVariantMap &metrics);
    double calculateDamageVariation(const QVariantMap &metrics);
    QString generateStrategyRecommendation(double winProbability, const QVariantMap &metrics);
    
    // Утилітарні методи
    QString getPerformanceGrade(double totalDamage, double assistedDamage, const QString& vehicleType, int tier);
    QString getDetailedPerformanceDescription(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics);
    QString generateBehavioralInsights(const QVariantMap &replayData, const QVariantMap &metrics);
    QVariantMap createDetailedReport(const QVariantMap &replayData, const QVariantMap &metrics);
};

#endif // BEHAVIORANALYZER_H
