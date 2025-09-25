#include "metricscalculator.h"
#include "VehicleData.h" // <-- Включаємо нашу оновлену базу даних
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

    // --- Новий, надійний спосіб отримання даних про танк ---
    QString techName = replayData.value("playerVehicle").toString();
    VehicleInfo vehicleInfo = VehicleDatabase::getInfoFromTechName(techName); // <-- Єдиний виклик

    int vehicleTier = vehicleInfo.tier;
    QString vehicleType = vehicleInfo.type;
    metrics["cleanVehicleName"] = vehicleInfo.name; // Передаємо чисту назву для відображення

    // Сумарні показники за бій
    double totalDamageDealt = 0, damageAssisted = 0, damageBlocked = 0, kills = 0;
    double totalShots = 0, totalHits = 0, totalPiercings = 0;

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
        }
    }

    // Збираємо ключові метрики
    metrics["totalDamageDealt"] = totalDamageDealt;
    metrics["damageAssisted"] = damageAssisted;
    metrics["damageBlockedByArmor"] = damageBlocked;
    metrics["kills"] = kills;
    metrics["vehicleTier"] = vehicleTier;
    metrics["vehicleType"] = vehicleType;

    double accuracy = (totalShots > 0) ? (totalHits / totalShots) * 100.0 : 0.0;
    double penetrationRatio = (totalHits > 0) ? (totalPiercings / totalHits) * 100.0 : 0.0;
    metrics["accuracy"] = QString::number(accuracy, 'f', 2) + "%";
    metrics["penetrationRatio"] = QString::number(penetrationRatio, 'f', 2) + "%";

    return metrics;
}
