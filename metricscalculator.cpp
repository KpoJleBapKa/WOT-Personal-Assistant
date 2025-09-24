#include "metricscalculator.h"

MetricsCalculator::MetricsCalculator(QObject *parent) : QObject(parent) {}

QVariantMap MetricsCalculator::calculate(const QVariantMap &replayData)
{
    QVariantMap metrics;
    if (!replayData.contains("personal") || !replayData.contains("playerID")) {
        return metrics; // Повертаємо пусту мапу, якщо даних немає
    }

    QVariantMap personalData = replayData.value("personal").toMap();
    quint64 playerID = replayData.value("playerID").toULongLong();

    // Сумарні показники за бій, якщо гравець використовував кілька танків
    double totalShots = 0;
    double totalHits = 0;
    double totalPiercings = 0;
    double totalDamageDealt = 0;

    for (auto it = personalData.constBegin(); it != personalData.constEnd(); ++it) {
        if (it.key() == "avatar") continue;

        QVariantMap vehicleStats = it.value().toMap();
        if (vehicleStats.value("accountDBID").toULongLong() == playerID) {
            totalShots += vehicleStats.value("shots", 0).toDouble();
            totalHits += vehicleStats.value("directHits", 0).toDouble();
            totalPiercings += vehicleStats.value("piercings", 0).toDouble();
            totalDamageDealt += vehicleStats.value("damageDealt", 0).toDouble();
        }
    }

    // Розрахунок метрик
    double accuracy = (totalShots > 0) ? (totalHits / totalShots) * 100.0 : 0.0;
    double penetrationRatio = (totalHits > 0) ? (totalPiercings / totalHits) * 100.0 : 0.0;

    // Внесок у перемогу (спрощена модель)
    // Формула: (DamageDealt / 1000) * (Accuracy / 100) * (PenetrationRatio / 100)
    double contributionScore = (totalDamageDealt / 1000.0) * (accuracy / 100.0) * (penetrationRatio / 100.0);

    metrics["accuracy"] = QString::number(accuracy, 'f', 2) + "%";
    metrics["penetrationRatio"] = QString::number(penetrationRatio, 'f', 2) + "%";
    metrics["totalDamageDealt"] = totalDamageDealt;
    metrics["contributionScore"] = QString::number(contributionScore, 'f', 2);

    // Час виживання поки що неможливо розрахувати без детального аналізу потоку подій
    metrics["survivalTime"] = "Недоступно";

    return metrics;
}
