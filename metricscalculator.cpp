#include "metricscalculator.h"
#include "VehicleData.h"
#include <QDebug>

MetricsCalculator::MetricsCalculator(QObject *parent) : QObject(parent) {}

QVariantMap MetricsCalculator::calculate(const QVariantMap &replayData)
{
    QVariantMap metrics;
    if (!replayData.contains("personal") || !replayData.contains("playerID")) {
        return metrics;
    }

    QVariantMap personalData = replayData.value("personal").toMap();
    quint64 playerID = replayData.value("playerID").toULongLong();

    QString techName = replayData.value("playerVehicle").toString();
    VehicleInfo vehicleInfo = VehicleDatabase::getInfoFromTechName(techName);

    metrics["cleanVehicleName"] = vehicleInfo.name;
    metrics["vehicleTier"] = vehicleInfo.tier;
    metrics["vehicleType"] = vehicleInfo.type;

    // Сумарні показники за бій
    double totalDamageDealt = 0, damageAssisted = 0, damageBlocked = 0, kills = 0;
    double totalShots = 0, totalHits = 0, totalPiercings = 0;
    double xp = 0, credits = 0, spotted = 0; // ❗️ НОВІ ЗМІННІ

    for (auto it = personalData.constBegin(); it != personalData.constEnd(); ++it) {
        if (it.key() == "avatar") continue;
        QVariantMap vehicleStats = it.value().toMap();
        if (vehicleStats.value("accountDBID").toULongLong() == playerID) {
            totalDamageDealt += vehicleStats.value("damageDealt", 0).toDouble();
            damageAssisted += vehicleStats.value("damageAssistedRadio", 0).toDouble();
            damageAssisted += vehicleStats.value("damageAssistedTrack", 0).toDouble();
            damageBlocked += vehicleStats.value("damageBlockedByArmor", 0).toDouble();
            kills += vehicleStats.value("kills", 0).toDouble();
            totalShots += vehicleStats.value("shots", 0).toDouble();
            totalHits += vehicleStats.value("directHits", 0).toDouble();
            totalPiercings += vehicleStats.value("piercings", 0).toDouble();
            // ❗️ ЗБИРАЄМО НОВІ ДАНІ
            xp += vehicleStats.value("xp", 0).toDouble();
            credits += vehicleStats.value("credits", 0).toDouble();
            spotted += vehicleStats.value("spotted", 0).toDouble();
        }
    }

    // Збираємо основні метрики
    metrics["totalDamageDealt"] = totalDamageDealt;
    metrics["damageAssisted"] = damageAssisted;
    metrics["damageBlockedByArmor"] = damageBlocked;
    metrics["kills"] = kills;
    metrics["xp"] = xp;
    metrics["credits"] = credits;
    metrics["spotted"] = spotted;
    metrics["shots"] = totalShots;
    metrics["hits"] = totalHits;
    metrics["piercings"] = totalPiercings;

    double accuracy = (totalShots > 0) ? (totalHits / totalShots) * 100.0 : 0.0;
    double penetrationRatio = (totalHits > 0) ? (totalPiercings / totalHits) * 100.0 : 0.0;
    metrics["accuracy"] = QString::number(accuracy, 'f', 2) + "%";
    metrics["penetrationRatio"] = QString::number(penetrationRatio, 'f', 2) + "%";

    // ❗️ НОВІ РОЗРАХУНКИ
    double avgDmgPerPen = (totalPiercings > 0) ? (totalDamageDealt / totalPiercings) : 0.0;
    double combinedContribution = totalDamageDealt + damageAssisted + damageBlocked;
    metrics["avgDmgPerPen"] = qRound(avgDmgPerPen);
    metrics["combinedContribution"] = qRound(combinedContribution);

    return metrics;
}
