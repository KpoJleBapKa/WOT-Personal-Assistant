#include "behavioranalyzer.h"
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QtMath>

// Функція оцінки продуктивності (залишається без змін)
QString BehaviorAnalyzer::getPerformanceGrade(double totalDamage, double assistedDamage, const QString& vehicleType, int tier)
{
    if (tier == 0 || vehicleType == "unknown") {
        return "Неможливо визначити (помилка даних)";
    }
    double mainMetric = 0;
    if (vehicleType.contains("light", Qt::CaseInsensitive)) { mainMetric = assistedDamage; } else { mainMetric = totalDamage; }
    if (!vehicleType.contains("light", Qt::CaseInsensitive)) {
        switch (tier) {
        case 10: if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4000) return "Дуже хороший"; if (mainMetric >= 3500) return "Хороший"; if (mainMetric >= 3000) return "Непоганий"; if (mainMetric >= 2500) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 1500) return "Поганий"; break;
        case 9: if (mainMetric >= 4000) return "Еталонний"; if (mainMetric >= 3500) return "Дуже хороший"; if (mainMetric >= 3000) return "Хороший"; if (mainMetric >= 2500) return "Непоганий"; if (mainMetric >= 2000) return "Посередній"; if (mainMetric >= 1500) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий"; break;
        case 8: if (vehicleType.contains("heavy", Qt::CaseInsensitive)) { if (mainMetric >= 3500) return "Еталонний"; if (mainMetric >= 3000) return "Дуже хороший"; if (mainMetric >= 2500) return "Хороший"; if (mainMetric >= 2000) return "Непоганий"; if (mainMetric >= 1500) return "Посередній"; if (mainMetric >= 1000) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий"; } else { if (mainMetric >= 3000) return "Еталонний"; if (mainMetric >= 2500) return "Хороший"; if (mainMetric >= 2000) return "Непоганий"; if (mainMetric >= 1500) return "Посередній"; if (mainMetric >= 1000) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий"; } break;
        }
    } else {
        switch (tier) {
        case 10: if (mainMetric >= 6000) return "Еталонний"; if (mainMetric >= 5000) return "Дуже хороший"; if (mainMetric >= 4500) return "Хороший"; if (mainMetric >= 4000) return "Непоганий"; if (mainMetric >= 3000) return "Посередній"; if (mainMetric >= 2500) return "Нижче середнього"; if (mainMetric >= 2000) return "Поганий"; break;
        case 9: if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4500) return "Дуже хороший"; if (mainMetric >= 4000) return "Хороший"; if (mainMetric >= 3500) return "Непоганий"; if (mainMetric >= 3000) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 1500) return "Поганий"; break;
        case 8: if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4500) return "Дуже хороший"; if (mainMetric >= 3500) return "Хороший"; if (mainMetric >= 3000) return "Непоганий"; if (mainMetric >= 2500) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 2000) return "Поганий"; break;
        }
    }
    return "Жахливий";
}

BehaviorAnalyzer::BehaviorAnalyzer(QObject *parent) : QObject(parent) {}

QVariantMap BehaviorAnalyzer::analyze(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap analysis;
    if (metrics.isEmpty()) return analysis;

    // Виконуємо всі типи розширеного аналізу
    AdvancedTacticalPattern tactics = analyzeAdvancedTacticalPatterns(replayData, metrics);
    RoleEffectiveness roles = analyzeRoleEffectiveness(replayData, metrics);
    TimeBasedAnalysis timeAnalysis = analyzeTimeBasedPerformance(replayData, metrics);
    AdvancedCombatEfficiency combat = analyzeAdvancedCombatEfficiency(replayData, metrics);
    TeamSynergy team = analyzeTeamSynergy(replayData, metrics);
    WeaknessAnalysis weaknesses = analyzeWeaknesses(replayData, metrics);
    
    // Нові складні аналізи
    MapAnalysis mapAnalysis = analyzeMapEffectiveness(replayData, metrics);
    PositionData positioning = analyzePositioning(replayData, metrics);
    MachineLearningData mlData = performMachineLearningAnalysis(replayData, metrics);
    QVariantMap historicalComparison = compareWithHistoricalData(replayData, metrics);
    QVariantMap performancePrediction = predictPerformance(replayData, metrics);

    // Базова оцінка продуктивності
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    QString vehicleType = metrics.value("vehicleType").toString();
    int tier = metrics.value("vehicleTier", 0).toInt();
    
    QString performanceGrade = getPerformanceGrade(totalDamage, assistedDamage, vehicleType, tier);
    analysis["performanceGrade"] = performanceGrade;

    // Детальний опис поведінки
    analysis["detailedBehavior"] = getDetailedPerformanceDescription(metrics, tactics);
    
    // Розширені тактичні паттерни
    QVariantMap tacticsMap;
    tacticsMap["aggressiveness"] = QString::number(tactics.aggressiveness, 'f', 1) + "%";
    tacticsMap["defensiveness"] = QString::number(tactics.defensiveness, 'f', 1) + "%";
    tacticsMap["supportiveness"] = QString::number(tactics.supportiveness, 'f', 1) + "%";
    tacticsMap["positioning"] = QString::number(tactics.positioning, 'f', 1) + "%";
    tacticsMap["mapControl"] = QString::number(tactics.mapControl, 'f', 1) + "%";
    tacticsMap["initiative"] = QString::number(tactics.initiative, 'f', 1) + "%";
    tacticsMap["adaptability"] = QString::number(tactics.adaptability, 'f', 1) + "%";
    tacticsMap["riskTaking"] = QString::number(tactics.riskTaking, 'f', 1) + "%";
    tacticsMap["patience"] = QString::number(tactics.patience, 'f', 1) + "%";
    tacticsMap["decisionMaking"] = QString::number(tactics.decisionMaking, 'f', 1) + "%";
    tacticsMap["resourceManagement"] = QString::number(tactics.resourceManagement, 'f', 1) + "%";
    tacticsMap["situationalAwareness"] = QString::number(tactics.situationalAwareness, 'f', 1) + "%";
    analysis["advancedTacticalPatterns"] = tacticsMap;

    // Ефективність ролей
    QVariantMap rolesMap;
    rolesMap["damageDealer"] = QString::number(roles.damageDealer, 'f', 1) + "%";
    rolesMap["scout"] = QString::number(roles.scout, 'f', 1) + "%";
    rolesMap["support"] = QString::number(roles.support, 'f', 1) + "%";
    rolesMap["tank"] = QString::number(roles.tank, 'f', 1) + "%";
    rolesMap["sniper"] = QString::number(roles.sniper, 'f', 1) + "%";
    rolesMap["brawler"] = QString::number(roles.brawler, 'f', 1) + "%";
    analysis["roleEffectiveness"] = rolesMap;

    // Часовий аналіз
    QVariantMap timeMap;
    timeMap["earlyGame"] = QString::number(timeAnalysis.earlyGame, 'f', 1) + "%";
    timeMap["midGame"] = QString::number(timeAnalysis.midGame, 'f', 1) + "%";
    timeMap["lateGame"] = QString::number(timeAnalysis.lateGame, 'f', 1) + "%";
    timeMap["clutchFactor"] = QString::number(timeAnalysis.clutchFactor, 'f', 1) + "%";
    analysis["timeBasedAnalysis"] = timeMap;

    // Бойова ефективність
    QVariantMap combatMap;
    combatMap["accuracy"] = QString::number(combat.accuracy, 'f', 1) + "%";
    combatMap["penetrationRate"] = QString::number(combat.penetrationRate, 'f', 1) + "%";
    combatMap["damagePerShot"] = QString::number(combat.damagePerShot, 'f', 0);
    combatMap["survivalRate"] = QString::number(combat.survivalRate, 'f', 1) + "%";
    combatMap["killEfficiency"] = QString::number(combat.killEfficiency, 'f', 1) + "%";
    combatMap["assistEfficiency"] = QString::number(combat.assistEfficiency, 'f', 1) + "%";
    analysis["combatEfficiency"] = combatMap;

    // Командна взаємодія
    QVariantMap teamMap;
    teamMap["communication"] = QString::number(team.communication, 'f', 1) + "%";
    teamMap["coordination"] = QString::number(team.coordination, 'f', 1) + "%";
    teamMap["leadership"] = QString::number(team.leadership, 'f', 1) + "%";
    teamMap["adaptability"] = QString::number(team.adaptability, 'f', 1) + "%";
    analysis["teamInteraction"] = teamMap;

    // Слабкі місця та рекомендації
    QVariantMap weaknessMap;
    weaknessMap["primaryWeaknesses"] = weaknesses.primaryWeaknesses;
    weaknessMap["secondaryWeaknesses"] = weaknesses.secondaryWeaknesses;
    weaknessMap["improvementAreas"] = weaknesses.improvementAreas;
    weaknessMap["overallRating"] = QString::number(weaknesses.overallRating, 'f', 1) + "%";
    analysis["weaknessAnalysis"] = weaknessMap;

    // Поведінкові інсайти
    //analysis["behavioralInsights"] = generateBehavioralInsights(replayData, metrics);

    // Розширена бойова ефективність
    QVariantMap advancedCombatMap;
    advancedCombatMap["accuracy"] = QString::number(combat.accuracy, 'f', 1) + "%";
    advancedCombatMap["penetrationRate"] = QString::number(combat.penetrationRate, 'f', 1) + "%";
    advancedCombatMap["damagePerShot"] = QString::number(combat.damagePerShot, 'f', 0);
    advancedCombatMap["survivalRate"] = QString::number(combat.survivalRate, 'f', 1) + "%";
    advancedCombatMap["killEfficiency"] = QString::number(combat.killEfficiency, 'f', 1) + "%";
    advancedCombatMap["assistEfficiency"] = QString::number(combat.assistEfficiency, 'f', 1) + "%";
    advancedCombatMap["targetSelection"] = QString::number(combat.targetSelection, 'f', 1) + "%";
    advancedCombatMap["timing"] = QString::number(combat.timing, 'f', 1) + "%";
    advancedCombatMap["ammoEfficiency"] = QString::number(combat.ammoEfficiency, 'f', 1) + "%";
    advancedCombatMap["criticalHitRate"] = QString::number(combat.criticalHitRate, 'f', 1) + "%";
    advancedCombatMap["moduleDamage"] = QString::number(combat.moduleDamage, 'f', 1) + "%";
    advancedCombatMap["crewDamage"] = QString::number(combat.crewDamage, 'f', 1) + "%";
    advancedCombatMap["fireRate"] = QString::number(combat.fireRate, 'f', 1) + "%";
    advancedCombatMap["reloadEfficiency"] = QString::number(combat.reloadEfficiency, 'f', 1) + "%";
    analysis["advancedCombatEfficiency"] = advancedCombatMap;

    // Командна синергія
    QVariantMap teamSynergyMap;
    teamSynergyMap["coordination"] = QString::number(team.coordination, 'f', 1) + "%";
    teamSynergyMap["communication"] = QString::number(team.communication, 'f', 1) + "%";
    teamSynergyMap["leadership"] = QString::number(team.leadership, 'f', 1) + "%";
    teamSynergyMap["adaptability"] = QString::number(team.adaptability, 'f', 1) + "%";
    teamSynergyMap["roleFlexibility"] = QString::number(team.roleFlexibility, 'f', 1) + "%";
    teamSynergyMap["supportReadiness"] = QString::number(team.supportReadiness, 'f', 1) + "%";
    teamSynergyMap["initiative"] = QString::number(team.initiative, 'f', 1) + "%";
    teamSynergyMap["conflictResolution"] = QString::number(team.conflictResolution, 'f', 1) + "%";
    teamSynergyMap["resourceSharing"] = QString::number(team.resourceSharing, 'f', 1) + "%";
    teamSynergyMap["tacticalAwareness"] = QString::number(team.tacticalAwareness, 'f', 1) + "%";
    analysis["teamSynergy"] = teamSynergyMap;

    // Аналіз карти
    QVariantMap mapAnalysisMap;
    mapAnalysisMap["mapName"] = mapAnalysis.mapName;
    mapAnalysisMap["mapType"] = mapAnalysis.mapType;
    mapAnalysisMap["effectiveness"] = QString::number(mapAnalysis.effectiveness, 'f', 1) + "%";
    mapAnalysisMap["preferredZone"] = mapAnalysis.preferredZone;
    mapAnalysisMap["averagePosition"] = QString::number(mapAnalysis.averagePosition, 'f', 1);
    analysis["mapAnalysis"] = mapAnalysisMap;

    // Позиціонування
    QVariantMap positioningMap;
    positioningMap["x"] = QString::number(positioning.x, 'f', 2);
    positioningMap["y"] = QString::number(positioning.y, 'f', 2);
    positioningMap["z"] = QString::number(positioning.z, 'f', 2);
    positioningMap["distanceToEnemies"] = QString::number(positioning.distanceToEnemies, 'f', 2);
    positioningMap["distanceToAllies"] = QString::number(positioning.distanceToAllies, 'f', 2);
    positioningMap["mapPosition"] = QString::number(positioning.mapPosition, 'f', 1) + "%";
    positioningMap["zone"] = positioning.zone;
    positioningMap["coverUsage"] = QString::number(positioning.coverUsage, 'f', 1) + "%";
    positioningMap["mobility"] = QString::number(positioning.mobility, 'f', 1) + "%";
    analysis["positioning"] = positioningMap;

    // Машинне навчання
    QVariantMap mlMap;
    mlMap["predictionAccuracy"] = QString::number(mlData.predictionAccuracy, 'f', 1) + "%";
    mlMap["importantFeatures"] = mlData.importantFeatures;
    analysis["machineLearning"] = mlMap;

    // Порівняння з історичними даними
    analysis["historicalComparison"] = historicalComparison;

    // Прогнозування продуктивності
    analysis["performancePrediction"] = performancePrediction;

    // Детальний звіт
    analysis["detailedReport"] = createDetailedReport(replayData, metrics);

    return analysis;
}

// ==================== ОСНОВНІ МЕТОДИ АНАЛІЗУ ====================

AdvancedTacticalPattern BehaviorAnalyzer::analyzeAdvancedTacticalPatterns(const QVariantMap &replayData, const QVariantMap &metrics)
{
    AdvancedTacticalPattern pattern;
    
    // Базові тактичні показники
    pattern.aggressiveness = calculateAggressiveness(replayData, metrics);
    pattern.defensiveness = calculateDefensiveness(replayData, metrics);
    pattern.supportiveness = calculateSupportiveness(replayData, metrics);
    pattern.positioning = calculatePositioning(replayData, metrics);
    pattern.mapControl = calculateMapControl(replayData, metrics);
    pattern.initiative = calculateInitiative(replayData, metrics);
    
    // Розширені тактичні показники
    pattern.adaptability = calculateAdaptability(replayData, metrics);
    pattern.riskTaking = calculateRiskTaking(replayData, metrics);
    pattern.patience = calculatePatience(replayData, metrics);
    pattern.decisionMaking = calculateDecisionMaking(replayData, metrics);
    pattern.resourceManagement = calculateResourceManagement(replayData, metrics);
    pattern.situationalAwareness = calculateSituationalAwareness(replayData, metrics);
    
    return pattern;
}

RoleEffectiveness BehaviorAnalyzer::analyzeRoleEffectiveness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    RoleEffectiveness roles;
    
    roles.damageDealer = calculateDamageDealerEffectiveness(metrics);
    roles.scout = calculateScoutEffectiveness(metrics);
    roles.support = calculateSupportEffectiveness(metrics);
    roles.tank = calculateTankEffectiveness(metrics);
    roles.sniper = calculateSniperEffectiveness(metrics);
    roles.brawler = calculateBrawlerEffectiveness(metrics);
    
    return roles;
}

TimeBasedAnalysis BehaviorAnalyzer::analyzeTimeBasedPerformance(const QVariantMap &replayData, const QVariantMap &metrics)
{
    TimeBasedAnalysis timeAnalysis;
    
    QVector<QVariantMap> events = getTimeBasedEvents(replayData);
    
    timeAnalysis.earlyGame = analyzePhaseEffectiveness(events, 0); // 0-33% бою
    timeAnalysis.midGame = analyzePhaseEffectiveness(events, 1);   // 33-66% бою
    timeAnalysis.lateGame = analyzePhaseEffectiveness(events, 2);  // 66-100% бою
    timeAnalysis.clutchFactor = calculateClutchFactor(events, metrics);
    
    return timeAnalysis;
}

AdvancedCombatEfficiency BehaviorAnalyzer::analyzeAdvancedCombatEfficiency(const QVariantMap &replayData, const QVariantMap &metrics)
{
    AdvancedCombatEfficiency combat;
    
    double shots = metrics.value("shots", 0.0).toDouble();
    double hits = metrics.value("hits", 0.0).toDouble();
    double piercings = metrics.value("piercings", 0.0).toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double kills = metrics.value("kills", 0.0).toDouble();
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    
    // Базові показники
    combat.accuracy = (shots > 0) ? (hits / shots) * 100.0 : 0.0;
    combat.penetrationRate = (hits > 0) ? (piercings / hits) * 100.0 : 0.0;
    combat.damagePerShot = (shots > 0) ? totalDamage / shots : 0.0;
    combat.survivalRate = calculateSurvivalRate(replayData, metrics);
    combat.killEfficiency = (spotted > 0) ? (kills / spotted) * 100.0 : 0.0;
    combat.assistEfficiency = (spotted > 0) ? (assists / spotted) : 0.0;
    
    // Розширені бойові показники
    combat.targetSelection = calculateTargetSelection(replayData, metrics);
    combat.timing = calculateTiming(replayData, metrics);
    combat.ammoEfficiency = calculateAmmoEfficiency(replayData, metrics);
    combat.criticalHitRate = calculateCriticalHitRate(replayData, metrics);
    combat.moduleDamage = calculateModuleDamage(replayData, metrics);
    combat.crewDamage = calculateCrewDamage(replayData, metrics);
    combat.fireRate = calculateFireRate(replayData, metrics);
    combat.reloadEfficiency = calculateReloadEfficiency(replayData, metrics);
    
    return combat;
}

TeamSynergy BehaviorAnalyzer::analyzeTeamSynergy(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(metrics)
    TeamSynergy team;
    
    // Базові показники командної роботи
    team.communication = analyzeCommunication(replayData);
    team.coordination = analyzeCoordination(replayData);
    team.leadership = analyzeLeadership(replayData);
    team.adaptability = analyzeAdaptability(replayData);
    
    // Розширені показники командної синергії
    team.roleFlexibility = calculateRoleFlexibility(replayData, metrics);
    team.supportReadiness = calculateSupportReadiness(replayData, metrics);
    team.initiative = calculateTeamInitiative(replayData, metrics);
    team.conflictResolution = calculateConflictResolution(replayData, metrics);
    team.resourceSharing = calculateResourceSharing(replayData, metrics);
    team.tacticalAwareness = calculateTacticalAwareness(replayData, metrics);
    
    return team;
}

WeaknessAnalysis BehaviorAnalyzer::analyzeWeaknesses(const QVariantMap &replayData, const QVariantMap &metrics)
{
    WeaknessAnalysis weaknesses;
    
    AdvancedTacticalPattern tactics = analyzeAdvancedTacticalPatterns(replayData, metrics);
    RoleEffectiveness roles = analyzeRoleEffectiveness(replayData, metrics);
    AdvancedCombatEfficiency combat = analyzeAdvancedCombatEfficiency(replayData, metrics);
    TeamSynergy team = analyzeTeamSynergy(replayData, metrics);
    
    weaknesses.primaryWeaknesses = identifyPrimaryWeaknesses(metrics, tactics, roles);
    weaknesses.secondaryWeaknesses = identifySecondaryWeaknesses(metrics, combat, team);
    weaknesses.improvementAreas = generateImprovementSuggestions(weaknesses);
    weaknesses.overallRating = calculateOverallRating(metrics, tactics, roles, combat, team);
    
    return weaknesses;
}

// ==================== ДОПОМІЖНІ МЕТОДИ ДЛЯ ТАКТИЧНИХ ПАТТЕРНІВ ====================

double BehaviorAnalyzer::calculateAggressiveness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо агресивність на основі наданої та отриманої шкоди
    // В пріоритеті саме отримана шкода
    double damageDealt = metrics.value("totalDamageDealt", 0.0).toDouble();
    double damageReceived = metrics.value("potentialDamageReceived", 0.0).toDouble();
    
    // Розрахунок агресивності: 70% отримана шкода + 30% надана шкода
    // Нормалізуємо до 100%: при 2000+ шкоди = 100%
    double totalDamage = damageReceived * 0.7 + damageDealt * 0.3;
    double aggression = (totalDamage / 20.0); // 2000 шкоди = 100%
    return qMin(100.0, aggression);
}

double BehaviorAnalyzer::calculateDefensiveness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    double score = 0.0;
    
    // Фактори оборонності
    double blockedDamage = metrics.value("damageBlockedByArmor", 0.0).toDouble();
    double receivedDamage = metrics.value("potentialDamageReceived", 0.0).toDouble();
    double survivalTime = calculateSurvivalTime(replayData);
    
    // Співвідношення заблокованої шкоди до отриманої
    if (receivedDamage > 0) {
        double blockRatio = blockedDamage / receivedDamage;
        score += qMin(100.0, blockRatio * 50.0) * 0.4;
    }
    
    // Час виживання (нормалізований)
    score += qMin(100.0, survivalTime * 2.0) * 0.3;
    
    // Кількість виявлених ворогів (оборонна розвідка)
    double spotted = metrics.value("spotted", 0.0).toDouble();
    score += qMin(100.0, spotted * 5.0) * 0.3;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateSupportiveness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    double score = 0.0;
    
    // Фактори підтримки
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    
    // Співвідношення допомоги до власної шкоди
    if (totalDamage > 0) {
        double assistRatio = assistedDamage / totalDamage;
        score += qMin(100.0, assistRatio * 100.0) * 0.4;
    }
    
    // Розвідка як підтримка
    score += qMin(100.0, spotted * 8.0) * 0.3;
    
    // Аналіз командних досягнень
    int achievements = metrics.value("achievementsCount", 0).toInt();
    score += qMin(100.0, achievements * 10.0) * 0.3;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculatePositioning(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    double score = 0.0;
    
    // Фактори позиціонування
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    double penetrationRate = metrics.value("penetrationRatio").toString().replace("%", "").toDouble();
    double damagePerShot = metrics.value("avgDmgPerPen", 0.0).toDouble();
    
    // Точність стрільби як показник хорошого позиціонування
    score += qMin(100.0, accuracy) * 0.3;
    
    // Частота пробиттів як показник правильного вибору цілей
    score += qMin(100.0, penetrationRate) * 0.3;
    
    // Середня шкода за пробиття як показник ефективного позиціонування
    score += qMin(100.0, damagePerShot / 10.0) * 0.4;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateMapControl(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    double score = 0.0;
    
    // Фактори контролю карти
    double spotted = metrics.value("spotted", 0.0).toDouble();
    double kills = metrics.value("kills", 0.0).toDouble();
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    
    // Розвідка як контроль карти
    score += qMin(100.0, spotted * 6.0) * 0.4;
    
    // Знищення ворогів як контроль території
    score += qMin(100.0, kills * 15.0) * 0.3;
    
    // Допомога команді як контроль ситуації
    score += qMin(100.0, assists / 50.0) * 0.3;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateInitiative(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо ініціативу на основі наданої та отриманої шкоди
    // Ключовим показником буде надана шкода
    double damageDealt = metrics.value("totalDamageDealt", 0.0).toDouble();
    double damageReceived = metrics.value("potentialDamageReceived", 0.0).toDouble();
    
    // Розрахунок ініціативи: 70% надана шкода + 30% отримана шкода
    // Нормалізуємо до 100%: при 2000+ шкоди = 100%
    double totalDamage = damageDealt * 0.7 + damageReceived * 0.3;
    double initiative = (totalDamage / 20.0); // 2000 шкоди = 100%
    return qMin(100.0, initiative);
}

// ==================== МЕТОДИ ДЛЯ АНАЛІЗУ РОЛЕЙ ====================

double BehaviorAnalyzer::calculateDamageDealerEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double kills = metrics.value("kills", 0.0).toDouble();
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    double damagePerShot = metrics.value("avgDmgPerPen", 0.0).toDouble();
    
    // Шкода як основний показник
    score += qMin(100.0, totalDamage / 50.0) * 0.4;
    
    // Знищення як показник ефективності
    score += qMin(100.0, kills * 20.0) * 0.3;
    
    // Точність як показник майстерності
    score += qMin(100.0, accuracy) * 0.2;
    
    // Ефективність пострілів
    score += qMin(100.0, damagePerShot / 15.0) * 0.1;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateScoutEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double spotted = metrics.value("spotted", 0.0).toDouble();
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    double survivalTime = calculateSurvivalTime(QVariantMap()); // Спрощено для прикладу
    
    // Розвідка як основний показник
    score += qMin(100.0, spotted * 8.0) * 0.5;
    
    // Допомога команді
    score += qMin(100.0, assistedDamage / 100.0) * 0.3;
    
    // Виживання як показник майстерності розвідки
    score += qMin(100.0, survivalTime * 3.0) * 0.2;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateSupportEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    int achievements = metrics.value("achievementsCount", 0).toInt();
    
    // Допомога команді
    score += qMin(100.0, assistedDamage / 80.0) * 0.4;
    
    // Розвідка як підтримка
    score += qMin(100.0, spotted * 6.0) * 0.3;
    
    // Командні досягнення
    score += qMin(100.0, achievements * 12.0) * 0.3;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateTankEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double blockedDamage = metrics.value("damageBlockedByArmor", 0.0).toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double receivedDamage = metrics.value("potentialDamageReceived", 0.0).toDouble();
    
    // Заблокована шкода як основний показник
    score += qMin(100.0, blockedDamage / 100.0) * 0.5;
    
    // Співвідношення заблокованої до отриманої шкоди
    if (receivedDamage > 0) {
        double blockRatio = blockedDamage / receivedDamage;
        score += qMin(100.0, blockRatio * 50.0) * 0.3;
    }
    
    // Власна шкода як показник активності
    score += qMin(100.0, totalDamage / 60.0) * 0.2;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateSniperEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    double penetrationRate = metrics.value("penetrationRatio").toString().replace("%", "").toDouble();
    double damagePerShot = metrics.value("avgDmgPerPen", 0.0).toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    
    // Точність як основний показник снайпера
    score += qMin(100.0, accuracy) * 0.3;
    
    // Частота пробиттів
    score += qMin(100.0, penetrationRate) * 0.3;
    
    // Ефективність пострілів
    score += qMin(100.0, damagePerShot / 20.0) * 0.2;
    
    // Загальна шкода
    score += qMin(100.0, totalDamage / 50.0) * 0.2;
    
    return qMin(100.0, score);
}

double BehaviorAnalyzer::calculateBrawlerEffectiveness(const QVariantMap &metrics)
{
    double score = 0.0;
    
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double kills = metrics.value("kills", 0.0).toDouble();
    double shots = metrics.value("shots", 0.0).toDouble();
    double hits = metrics.value("hits", 0.0).toDouble();
    
    // Шкода в ближньому бою
    score += qMin(100.0, totalDamage / 40.0) * 0.4;
    
    // Знищення в ближньому бою
    score += qMin(100.0, kills * 25.0) * 0.3;
    
    // Швидкість стрільби (багато пострілів)
    if (shots > 0) {
        double fireRate = shots / qMax(1.0, hits);
        score += qMin(100.0, fireRate * 10.0) * 0.3;
    }
    
    return qMin(100.0, score);
}

// ==================== ДОПОМІЖНІ МЕТОДИ ====================

double BehaviorAnalyzer::calculateSurvivalTime(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Спрощена реалізація - в реальності потрібно аналізувати події смерті
    return 300.0; // 5 хвилин за замовчуванням
}

double BehaviorAnalyzer::calculateSurvivalRate(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(metrics)
    // Спрощена реалізація - в реальності потрібно аналізувати час виживання
    double survivalTime = calculateSurvivalTime(replayData);
    return qMin(100.0, survivalTime / 3.0); // Нормалізація до 100%
}

double BehaviorAnalyzer::calculateEarlyGameDamage(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз ранньої шкоди на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до детальних даних по часу
    return 0.0;
}

double BehaviorAnalyzer::calculateClutchFactor(const QVector<QVariantMap> &events, const QVariantMap &metrics)
{
    Q_UNUSED(events)
    // Спрощена реалізація - в реальності потрібно аналізувати вирішальні моменти
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double kills = metrics.value("kills", 0.0).toDouble();
    
    return qMin(100.0, (totalDamage / 100.0) + (kills * 10.0));
}

QVector<QVariantMap> BehaviorAnalyzer::getTimeBasedEvents(const QVariantMap &replayData)
{
    QVector<QVariantMap> events;
    
    if (replayData.contains("shot_events")) {
        QVariantList shotEvents = replayData.value("shot_events").toList();
        for (const QVariant &shotVar : shotEvents) {
            events.append(shotVar.toMap());
        }
    }
    
    return events;
}

double BehaviorAnalyzer::analyzePhaseEffectiveness(const QVector<QVariantMap> &events, int phase)
{
    Q_UNUSED(events)
    Q_UNUSED(phase)
    // Аналіз ефективності по фазах на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до детальних даних по фазах
    return 0.0;
}

double BehaviorAnalyzer::analyzeCommunication(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз комунікації на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до чату
    return 0.0;
}

double BehaviorAnalyzer::analyzeCoordination(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз координації на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до командних дій
    return 0.0;
}

double BehaviorAnalyzer::analyzeLeadership(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз лідерства на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до лідерських дій
    return 0.0;
}

double BehaviorAnalyzer::analyzeAdaptability(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз адаптивності на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про зміни стилю
    return 0.0;
}

QStringList BehaviorAnalyzer::identifyPrimaryWeaknesses(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics, const RoleEffectiveness &roles)
{
    Q_UNUSED(metrics)
    Q_UNUSED(roles)
    QStringList weaknesses;
    
    if (tactics.aggressiveness < 30) weaknesses << "Низька агресивність";
    if (tactics.defensiveness < 30) weaknesses << "Слабка оборона";
    if (tactics.supportiveness < 30) weaknesses << "Погана підтримка команди";
    if (tactics.positioning < 30) weaknesses << "Неправильне позиціонування";
    if (tactics.mapControl < 30) weaknesses << "Слабкий контроль карти";
    if (tactics.initiative < 30) weaknesses << "Відсутність ініціативи";
    
    return weaknesses;
}

QStringList BehaviorAnalyzer::identifySecondaryWeaknesses(const QVariantMap &metrics, const AdvancedCombatEfficiency &combat, const TeamSynergy &team)
{
    Q_UNUSED(metrics)
    Q_UNUSED(team)
    QStringList weaknesses;
    
    if (combat.accuracy < 50) weaknesses << "Низька точність стрільби";
    if (combat.penetrationRate < 50) weaknesses << "Погана ефективність пробиття";
    if (combat.survivalRate < 50) weaknesses << "Низька виживаність";
    
    return weaknesses;
}

QStringList BehaviorAnalyzer::generateImprovementSuggestions(const WeaknessAnalysis &weaknesses)
{
    QStringList suggestions;
    
    for (const QString &weakness : weaknesses.primaryWeaknesses) {
        if (weakness.contains("агресивність")) {
            suggestions << "Практикуйте більш активну гру в наступних боях";
        } else if (weakness.contains("оборона")) {
            suggestions << "Вивчайте тактики використання покриття";
        } else if (weakness.contains("підтримка")) {
            suggestions << "Фокусуйтеся на допомозі команді";
        }
    }
    
    return suggestions;
}

double BehaviorAnalyzer::calculateOverallRating(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics, const RoleEffectiveness &roles, const AdvancedCombatEfficiency &combat, const TeamSynergy &team)
{
    double score = 0.0;
    
    // Вагові коефіцієнти для різних аспектів
    score += (tactics.aggressiveness + tactics.defensiveness + tactics.supportiveness) / 3.0 * 0.3;
    score += (roles.damageDealer + roles.scout + roles.support) / 3.0 * 0.3;
    score += (combat.accuracy + combat.penetrationRate + combat.survivalRate) / 3.0 * 0.2;
    score += (team.communication + team.coordination + team.leadership) / 3.0 * 0.2;
    
    return qMin(100.0, score);
}

QString BehaviorAnalyzer::getDetailedPerformanceDescription(const QVariantMap &metrics, const AdvancedTacticalPattern &tactics)
{
    Q_UNUSED(metrics)
    QString description = "";
    
    // Аналіз тактичних паттернів
    if (tactics.aggressiveness > 70) {
        description += "Високоагресивний стиль гри з активною атакою\n";
    } else if (tactics.aggressiveness < 30) {
        description += "Обережний стиль гри з акцентом на виживання\n";
    } else {
        description += "Збалансований стиль гри\n";
    }
    
    if (tactics.defensiveness > 70) {
        description += "Відмінне використання покриття та броні\n";
    }
    
    if (tactics.supportiveness > 70) {
        description += "Ефективна підтримка команди\n";
    }
    
    return description;
}


QVariantMap BehaviorAnalyzer::createDetailedReport(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap report;
    
    report["summary"] = "Детальний звіт про поведінку гравця";
    report["timestamp"] = QDateTime::currentDateTime().toString();
    report["playerName"] = replayData.value("playerName", "Невідомо");
    report["vehicleName"] = metrics.value("cleanVehicleName", "Невідомо");
    
    return report;
}

// ==================== НОВІ СКЛАДНІ МЕТОДИ АНАЛІЗУ ====================

MapAnalysis BehaviorAnalyzer::analyzeMapEffectiveness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    MapAnalysis analysis;
    
    analysis.mapName = replayData.value("mapDisplayName", "Unknown").toString();
    analysis.mapType = determineMapType(analysis.mapName);
    analysis.effectiveness = calculateMapTypeEffectiveness(analysis.mapType, metrics);
    analysis.preferredZone = determinePreferredZone(replayData, metrics);
    analysis.averagePosition = calculateAveragePosition(replayData);
    analysis.positionHistory = extractPositionHistory(replayData);
    
    return analysis;
}

PositionData BehaviorAnalyzer::analyzePositioning(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(metrics)
    return extractPositionData(replayData);
}

PlayerStatistics BehaviorAnalyzer::loadPlayerStatistics(const QString &playerName)
{
    Q_UNUSED(playerName)
    PlayerStatistics stats;
    stats.playerName = playerName;
    
    // Завантаження статистики гравця з бази даних
    // Поки що повертаємо нульові значення, оскільки немає доступу до бази даних
    stats.totalBattles = 0;
    stats.winRate = 0.0;
    stats.averageDamage = 0.0;
    stats.averageAssists = 0.0;
    stats.averageKills = 0.0;
    stats.averageSurvival = 0.0;
    
    return stats;
}

MachineLearningData BehaviorAnalyzer::performMachineLearningAnalysis(const QVariantMap &replayData, const QVariantMap &metrics)
{
    MachineLearningData mlData;
    
    // Витягуємо ознаки для машинного навчання
    mlData.features.append(extractFeatures(replayData, metrics));
    
    // Розраховуємо ваги ознак
    mlData.weights = calculateFeatureWeights(mlData.features, QVector<double>());
    
    // Визначаємо важливі ознаки
    mlData.importantFeatures = identifyImportantFeatures(mlData.weights);
    
    // Прогнозуємо ймовірність перемоги
    mlData.predictionAccuracy = predictWinProbability(mlData.features.first()) * 100.0;
    
    return mlData;
}

QVariantMap BehaviorAnalyzer::compareWithHistoricalData(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap comparison;
    
    QString playerName = replayData.value("playerName", "Unknown").toString();
    PlayerStatistics historicalData = loadPlayerStatistics(playerName);
    
    // Порівнюємо поточний бій з історичними даними
    double currentDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double damageDeviation = calculatePerformanceDeviation(replayData, historicalData);
    
    comparison["damageDeviation"] = QString::number(damageDeviation, 'f', 1) + "%";
    comparison["vsAverageDamage"] = QString::number(currentDamage - historicalData.averageDamage, 'f', 0);
    comparison["performanceGrade"] = (damageDeviation > 0) ? "Вище середнього" : "Нижче середнього";
    comparison["recommendations"] = generateImprovementRecommendations(replayData, historicalData);
    
    return comparison;
}

QVariantMap BehaviorAnalyzer::predictPerformance(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap prediction;
    
    // Прогнозуємо продуктивність на основі поточних даних
    QVector<double> features = extractFeatures(replayData, metrics);
    double winProbability = predictWinProbability(features);
    
    prediction["winProbability"] = QString::number(winProbability * 100.0, 'f', 1) + "%";
    prediction["expectedDamage"] = QString::number(metrics.value("totalDamageDealt", 0.0).toDouble() * 1.1, 'f', 0);
    prediction["riskLevel"] = (winProbability < 0.3) ? "Високий" : (winProbability < 0.6) ? "Середній" : "Низький";
    prediction["recommendedStrategy"] = generateStrategyRecommendation(winProbability, metrics);
    
    return prediction;
}

// ==================== РОЗШИРЕНІ ДОПОМІЖНІ МЕТОДИ ====================

double BehaviorAnalyzer::calculateAdaptability(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз адаптивності на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про зміни стилю
    return 0.0;
}

double BehaviorAnalyzer::calculateRiskTaking(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо схильність до ризику на основі позиціонування та дій
    double damageRatio = metrics.value("totalDamageDealt", 0.0).toDouble() / 
                        qMax(1.0, metrics.value("potentialDamageReceived", 1.0).toDouble());
    double aggression = calculateAggressiveness(replayData, metrics);
    
    return qMin(100.0, (damageRatio * 20.0 + aggression) / 2.0);
}

double BehaviorAnalyzer::calculatePatience(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз терпіння на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до детальних даних про час між пострілами
    return 0.0;
}

double BehaviorAnalyzer::calculateDecisionMaking(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо якість прийняття рішень на основі ефективності дій
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    double penetrationRate = metrics.value("penetrationRatio").toString().replace("%", "").toDouble();
    double killEfficiency = (metrics.value("spotted", 0.0).toDouble() > 0) ? 
                           (metrics.value("kills", 0.0).toDouble() / metrics.value("spotted", 1.0).toDouble()) * 100.0 : 0.0;
    
    return (accuracy + penetrationRate + killEfficiency) / 3.0;
}

double BehaviorAnalyzer::calculateResourceManagement(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз управління ресурсами на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до детальних даних про ресурси
    return 0.0;
}

double BehaviorAnalyzer::calculateSituationalAwareness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо ситуаційну обізнаність на основі розвідки та позиціонування
    double spotted = metrics.value("spotted", 0.0).toDouble();
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double positioning = calculatePositioning(replayData, metrics);
    
    return qMin(100.0, (spotted * 5.0 + assists / 100.0 + positioning) / 3.0);
}

// ==================== ДОПОМІЖНІ МЕТОДИ ДЛЯ СКЛАДНОГО АНАЛІЗУ ====================

PositionData BehaviorAnalyzer::extractPositionData(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    PositionData position;
    
    // Витягування координат з реплею на основі доступних даних
    // Поки що використовуємо нульові значення, оскільки немає доступу до координат
    position.x = 0.0;
    position.y = 0.0;
    position.z = 0.0;
    position.distanceToEnemies = calculateDistanceToEnemies(replayData);
    position.distanceToAllies = calculateDistanceToAllies(replayData);
    position.mapPosition = 0.0;
    position.zone = "unknown";
    position.coverUsage = calculateCoverUsage(replayData);
    position.mobility = calculateMobility(replayData);
    
    return position;
}

double BehaviorAnalyzer::calculateDistanceToEnemies(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз відстані до ворогів на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до координат ворогів
    return 0.0;
}

double BehaviorAnalyzer::calculateDistanceToAllies(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз відстані до союзників на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до координат союзників
    return 0.0;
}

QString BehaviorAnalyzer::determineMapZone(const PositionData &position, const QString &mapName)
{
    Q_UNUSED(mapName)
    Q_UNUSED(position)
    // Визначення зони карти на основі доступних даних
    // Поки що повертаємо "unknown", оскільки немає доступу до координат
    return "unknown";
}

double BehaviorAnalyzer::calculateCoverUsage(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз використання покриття на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про покриття
    return 0.0;
}

double BehaviorAnalyzer::calculateMobility(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Аналіз мобільності гравця на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про рух
    return 0.0;
}

QVector<double> BehaviorAnalyzer::extractFeatures(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVector<double> features;
    
    // Витягуємо ключові ознаки для машинного навчання
    features.append(metrics.value("totalDamageDealt", 0.0).toDouble());
    features.append(metrics.value("damageAssisted", 0.0).toDouble());
    features.append(metrics.value("kills", 0.0).toDouble());
    features.append(metrics.value("spotted", 0.0).toDouble());
    features.append(metrics.value("accuracy").toString().replace("%", "").toDouble());
    features.append(metrics.value("penetrationRatio").toString().replace("%", "").toDouble());
    features.append(calculateSurvivalTime(replayData));
    features.append(calculateAggressiveness(replayData, metrics));
    features.append(calculateDefensiveness(replayData, metrics));
    features.append(calculateSupportiveness(replayData, metrics));
    
    return features;
}

double BehaviorAnalyzer::predictWinProbability(const QVector<double> &features)
{
    // Спрощена модель прогнозування на основі лінійної регресії
    if (features.size() < 10) return 0.5;
    
    double score = 0.0;
    score += features[0] * 0.0001;  // Шкода
    score += features[1] * 0.0001;  // Допомога
    score += features[2] * 0.1;     // Знищення
    score += features[3] * 0.05;    // Розвідка
    score += features[4] * 0.01;    // Точність
    score += features[5] * 0.01;    // Пробиття
    score += features[6] * 0.001;   // Час виживання
    score += features[7] * 0.01;    // Агресивність
    score += features[8] * 0.01;    // Оборонність
    score += features[9] * 0.01;    // Підтримка
    
    return qMax(0.0, qMin(1.0, score));
}

QMap<QString, double> BehaviorAnalyzer::calculateFeatureWeights(const QVector<QVector<double>> &features, const QVector<double> &targets)
{
    Q_UNUSED(features)
    Q_UNUSED(targets)
    // Спрощена реалізація - в реальності потрібен алгоритм машинного навчання
    QMap<QString, double> weights;
    weights["damage"] = 0.3;      // Шкода
    weights["assists"] = 0.2;     // Допомога
    weights["kills"] = 0.15;      // Знищення
    weights["spotted"] = 0.1;     // Розвідка
    weights["accuracy"] = 0.1;    // Точність
    weights["penetration"] = 0.1; // Пробиття
    weights["survival"] = 0.05;   // Час виживання
    return weights;
}

QStringList BehaviorAnalyzer::identifyImportantFeatures(const QMap<QString, double> &weights)
{
    QStringList important;
    
    // Сортуємо ознаки за важливістю
    QList<QPair<QString, double>> sortedWeights;
    for (auto it = weights.constBegin(); it != weights.constEnd(); ++it) {
        sortedWeights.append(qMakePair(it.key(), it.value()));
    }
    
    std::sort(sortedWeights.begin(), sortedWeights.end(), 
              [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
                  return a.second > b.second;
              });
    
    // Беремо топ-3 найважливіших ознаки
    for (int i = 0; i < qMin(3, sortedWeights.size()); ++i) {
        important.append(sortedWeights[i].first);
    }
    
    return important;
}

double BehaviorAnalyzer::calculatePerformanceDeviation(const QVariantMap &currentBattle, const PlayerStatistics &historicalData)
{
    Q_UNUSED(currentBattle)
    Q_UNUSED(historicalData)
    // Розрахунок відхилення поточної продуктивності від історичної
    // Поки що повертаємо 0, оскільки немає доступу до історичних даних
    return 0.0;
}

QVariantMap BehaviorAnalyzer::generateImprovementRecommendations(const QVariantMap &currentBattle, const PlayerStatistics &historicalData)
{
    Q_UNUSED(currentBattle)
    Q_UNUSED(historicalData)
    
    QVariantMap recommendations;
    recommendations["primary"] = "Покращте точність стрільби";
    recommendations["secondary"] = "Збільшіть активність в розвідці";
    recommendations["tertiary"] = "Практикуйте позиціонування";
    
    return recommendations;
}

double BehaviorAnalyzer::calculateMapTypeEffectiveness(const QString &mapType, const QVariantMap &metrics)
{
    Q_UNUSED(mapType)
    Q_UNUSED(metrics)
    // Розрахунок ефективності на різних типах карт на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до історичних даних по картах
    return 0.0;
}

double BehaviorAnalyzer::calculateVehicleTypeEffectiveness(const QString &vehicleType, const QVariantMap &metrics)
{
    Q_UNUSED(vehicleType)
    Q_UNUSED(metrics)
    // Розрахунок ефективності з різними типами техніки на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до історичних даних по техніці
    return 0.0;
}

// ==================== ДОДАТКОВІ МЕТОДИ ДЛЯ РОЗШИРЕНОГО АНАЛІЗУ ====================

QString BehaviorAnalyzer::determineMapType(const QString &mapName)
{
    // Визначаємо тип карти на основі назви
    if (mapName.contains("city", Qt::CaseInsensitive) || mapName.contains("urban", Qt::CaseInsensitive)) {
        return "urban";
    } else if (mapName.contains("field", Qt::CaseInsensitive) || mapName.contains("open", Qt::CaseInsensitive)) {
        return "open";
    } else if (mapName.contains("hill", Qt::CaseInsensitive) || mapName.contains("mountain", Qt::CaseInsensitive)) {
        return "hills";
    }
    return "mixed";
}

QString BehaviorAnalyzer::determinePreferredZone(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Визначення улюбленої зони гравця на основі доступних даних
    // Поки що повертаємо "unknown", оскільки немає доступу до історичних даних
    return "unknown";
}

double BehaviorAnalyzer::calculateAveragePosition(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Розрахунок середньої позиції гравця на карті на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до координат
    return 0.0;
}

QVector<PositionData> BehaviorAnalyzer::extractPositionHistory(const QVariantMap &replayData)
{
    Q_UNUSED(replayData)
    // Витягуємо історію позиціонування гравця
    QVector<PositionData> history;
    // В реальній реалізації тут буде аналіз координат з реплею
    return history;
}

double BehaviorAnalyzer::calculateTargetSelection(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо якість вибору цілей
    double penetrationRate = metrics.value("penetrationRatio").toString().replace("%", "").toDouble();
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    return (penetrationRate + accuracy) / 2.0;
}

double BehaviorAnalyzer::calculateTiming(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо таймінг атак
    double kills = metrics.value("kills", 0.0).toDouble();
    double shots = metrics.value("shots", 0.0).toDouble();
    return (shots > 0) ? (kills / shots) * 100.0 : 0.0;
}

double BehaviorAnalyzer::calculateAmmoEfficiency(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо ефективність використання боєприпасів
    double shots = metrics.value("shots", 0.0).toDouble();
    double damage = metrics.value("totalDamageDealt", 0.0).toDouble();
    return (shots > 0) ? (damage / shots) : 0.0;
}

double BehaviorAnalyzer::calculateCriticalHitRate(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз частоти критичних ударів на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про критичні удари
    return 0.0;
}

double BehaviorAnalyzer::calculateModuleDamage(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз пошкодження модулів на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про модулі
    return 0.0;
}

double BehaviorAnalyzer::calculateCrewDamage(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз пошкодження екіпажу на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про екіпаж
    return 0.0;
}

double BehaviorAnalyzer::calculateFireRate(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо швидкість стрільби
    double shots = metrics.value("shots", 0.0).toDouble();
    double battleTime = calculateSurvivalTime(replayData);
    return (battleTime > 0) ? (shots / (battleTime / 60.0)) : 0.0;
}

double BehaviorAnalyzer::calculateReloadEfficiency(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз ефективності перезарядки на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про перезарядку
    return 0.0;
}

double BehaviorAnalyzer::calculateRoleFlexibility(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо гнучкість ролей
    double damage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    
    // Розраховуємо баланс між різними ролями
    double total = damage + assists + spotted * 100.0;
    if (total == 0) return 0.0;
    
    double damageRatio = damage / total;
    double assistRatio = assists / total;
    double scoutRatio = (spotted * 100.0) / total;
    
    // Чим більше баланс, тим вища гнучкість
    double balance = 1.0 - qAbs(damageRatio - 0.33) - qAbs(assistRatio - 0.33) - qAbs(scoutRatio - 0.33);
    return qMax(0.0, balance * 100.0);
}

double BehaviorAnalyzer::calculateSupportReadiness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо готовність підтримати команду
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    double damage = metrics.value("totalDamageDealt", 0.0).toDouble();
    
    return qMin(100.0, (assists + spotted * 50.0) / qMax(1.0, damage / 100.0));
}

double BehaviorAnalyzer::calculateTeamInitiative(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо ініціативу в команді
    bool isFirstBlood = metrics.value("isFirstBlood", false).toBool();
    double earlyDamage = calculateEarlyGameDamage(replayData);
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    
    double initiative = 0.0;
    if (isFirstBlood) initiative += 30.0;
    if (totalDamage > 0) {
        double earlyRatio = earlyDamage / totalDamage;
        initiative += earlyRatio * 70.0;
    }
    
    return qMin(100.0, initiative);
}

double BehaviorAnalyzer::calculateConflictResolution(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    Q_UNUSED(metrics)
    // Аналіз вирішення конфліктів на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про конфлікти
    return 0.0;
}

double BehaviorAnalyzer::calculateResourceSharing(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо обмін ресурсами (допомога команді)
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double spotted = metrics.value("spotted", 0.0).toDouble();
    
    return qMin(100.0, (assists + spotted * 20.0) / 10.0);
}

double BehaviorAnalyzer::calculateTacticalAwareness(const QVariantMap &replayData, const QVariantMap &metrics)
{
    Q_UNUSED(replayData)
    // Аналізуємо тактичну обізнаність
    double spotted = metrics.value("spotted", 0.0).toDouble();
    double assists = metrics.value("damageAssisted", 0.0).toDouble();
    double accuracy = metrics.value("accuracy").toString().replace("%", "").toDouble();
    
    return (spotted * 5.0 + assists / 50.0 + accuracy) / 3.0;
}

double BehaviorAnalyzer::calculateDamageVariation(const QVariantMap &metrics)
{
    Q_UNUSED(metrics)
    // Розрахунок варіації шкоди (показник адаптивності) на основі доступних даних
    // Поки що повертаємо 0, оскільки немає доступу до даних про варіацію
    return 0.0;
}

QString BehaviorAnalyzer::generateStrategyRecommendation(double winProbability, const QVariantMap &metrics)
{
    Q_UNUSED(metrics)
    if (winProbability < 0.3) {
        return "Обережна тактика, фокус на виживанні";
    } else if (winProbability < 0.6) {
        return "Збалансований підхід, активна підтримка команди";
    } else {
        return "Агресивна тактика, лідерство в команді";
    }
}
