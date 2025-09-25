#include "behavioranalyzer.h"
#include <QDebug>

QString getPerformanceGrade(double totalDamage, double assistedDamage, const QString& vehicleType, int tier)
{
    // ❗️ ЗАХИСТ ВІД ПОМИЛОК ПАРСЕРА
    if (tier == 0 || vehicleType == "unknown") {
        return "Неможливо визначити (помилка даних)";
    }

    double mainMetric = 0;
    if (vehicleType.contains("light", Qt::CaseInsensitive)) {
        mainMetric = assistedDamage;
    } else {
        mainMetric = totalDamage;
    }

    // --- Ваші порогові значення ---
    if (!vehicleType.contains("light", Qt::CaseInsensitive)) {
        switch (tier) {
        case 10:
            if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4000) return "Дуже хороший"; if (mainMetric >= 3500) return "Хороший"; if (mainMetric >= 3000) return "Непоганий"; if (mainMetric >= 2500) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 1500) return "Поганий"; break;
        case 9:
            if (mainMetric >= 4000) return "Еталонний"; if (mainMetric >= 3500) return "Дуже хороший"; if (mainMetric >= 3000) return "Хороший"; if (mainMetric >= 2500) return "Непоганий"; if (mainMetric >= 2000) return "Посередній"; if (mainMetric >= 1500) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий"; break;
        case 8:
            if (vehicleType.contains("heavy", Qt::CaseInsensitive)) {
                if (mainMetric >= 3500) return "Еталонний"; if (mainMetric >= 3000) return "Дуже хороший"; if (mainMetric >= 2500) return "Хороший"; if (mainMetric >= 2000) return "Непоганий"; if (mainMetric >= 1500) return "Посередній"; if (mainMetric >= 1000) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий";
            } else {
                if (mainMetric >= 3000) return "Еталонний"; if (mainMetric >= 2500) return "Хороший"; if (mainMetric >= 2000) return "Непоганий"; if (mainMetric >= 1500) return "Посередній"; if (mainMetric >= 1000) return "Нижче середнього"; if (mainMetric >= 1000) return "Поганий";
            } break;
        }
    } else {
        switch (tier) {
        case 10:
            if (mainMetric >= 6000) return "Еталонний"; if (mainMetric >= 5000) return "Дуже хороший"; if (mainMetric >= 4500) return "Хороший"; if (mainMetric >= 4000) return "Непоганий"; if (mainMetric >= 3000) return "Посередній"; if (mainMetric >= 2500) return "Нижче середнього"; if (mainMetric >= 2000) return "Поганий"; break;
        case 9:
            if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4500) return "Дуже хороший"; if (mainMetric >= 4000) return "Хороший"; if (mainMetric >= 3500) return "Непоганий"; if (mainMetric >= 3000) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 1500) return "Поганий"; break;
        case 8:
            if (mainMetric >= 5000) return "Еталонний"; if (mainMetric >= 4500) return "Дуже хороший"; if (mainMetric >= 3500) return "Хороший"; if (mainMetric >= 3000) return "Непоганий"; if (mainMetric >= 2500) return "Посередній"; if (mainMetric >= 2000) return "Нижче середнього"; if (mainMetric >= 2000) return "Поганий"; break;
        }
    }

    return "Жахливий";
}

BehaviorAnalyzer::BehaviorAnalyzer(QObject *parent) : QObject(parent) {}

QVariantMap BehaviorAnalyzer::analyze(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap analysis;
    if (metrics.isEmpty()) return analysis;

    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    double blockedDamage = metrics.value("damageBlockedByArmor", 0.0).toDouble();
    QString vehicleType = metrics.value("vehicleType").toString();
    int tier = metrics.value("vehicleTier", 0).toInt();

    QString performanceGrade = getPerformanceGrade(totalDamage, assistedDamage, vehicleType, tier);
    analysis["performanceGrade"] = performanceGrade;

    QString rolePerformance = "Основний внесок: ";
    if (vehicleType.contains("light", Qt::CaseInsensitive)) {
        rolePerformance += "допомога команді.";
    } else {
        rolePerformance += "нанесення шкоди.";
    }

    if (vehicleType.contains("heavy", Qt::CaseInsensitive) && blockedDamage > totalDamage * 0.7 && blockedDamage > 2000) {
        rolePerformance += " Також відмінно використано броню.";
    }
    if ((vehicleType.contains("medium", Qt::CaseInsensitive) || vehicleType.contains("light", Qt::CaseInsensitive)) && assistedDamage > 1500) {
        if (!rolePerformance.contains("допомога"))
            rolePerformance += " Помітна допомога союзникам.";
    }
    analysis["rolePerformance"] = rolePerformance;

    return analysis;
}
