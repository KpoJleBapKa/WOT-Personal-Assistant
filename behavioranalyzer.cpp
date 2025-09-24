#include "behavioranalyzer.h"
#include <QDebug>

BehaviorAnalyzer::BehaviorAnalyzer(QObject *parent) : QObject(parent) {}

QVariantMap BehaviorAnalyzer::analyze(const QVariantMap &replayData, const QVariantMap &metrics)
{
    QVariantMap analysis;
    double damageDealt = metrics.value("totalDamageDealt", 0).toDouble();

    // 1. Аналіз стилю гри (агресивний/пасивний)
    // Це дуже спрощена модель. В ідеалі, потрібно порівнювати з середніми показниками на цьому танку.
    QString gameStyle;
    if (damageDealt > 3500) {
        gameStyle = "Дуже агресивний";
    } else if (damageDealt > 2000) {
        gameStyle = "Активний";
    } else if (damageDealt > 1000) {
        gameStyle = "Обережний";
    } else {
        gameStyle = "Пасивний";
    }
    analysis["gameStyle"] = gameStyle;


    // 2. Аналіз ефективності стрільби
    double penetrationRatio = metrics.value("penetrationRatio").toString().remove("%").toDouble();
    QString shootingEffectiveness;
    if (penetrationRatio > 85.0) {
        shootingEffectiveness = "Відмінна реалізація пострілів";
    } else if (penetrationRatio > 70.0) {
        shootingEffectiveness = "Хороша реалізація пострілів";
    } else {
        shootingEffectiveness = "Потребує покращення (слабкі зони)";
    }
    analysis["shootingEffectiveness"] = shootingEffectiveness;

    // 3. Аналіз командної взаємодії (на основі допомоги)
    QVariantMap personalData = replayData.value("personal").toMap();
    quint64 playerID = replayData.value("playerID").toULongLong();
    double assistedDamage = 0;
    for (auto it = personalData.constBegin(); it != personalData.constEnd(); ++it) {
        if (it.key() == "avatar") continue;
        QVariantMap vehicleStats = it.value().toMap();
        if (vehicleStats.value("accountDBID").toULongLong() == playerID) {
            assistedDamage += vehicleStats.value("damageAssistedRadio", 0).toDouble();
            assistedDamage += vehicleStats.value("damageAssistedTrack", 0).toDouble();
        }
    }

    QString teamPlay;
    if (assistedDamage > 1000) {
        teamPlay = "Активна допомога команді";
    } else if (assistedDamage > 250) {
        teamPlay = "Прийнятна допомога команді";
    } else {
        teamPlay = "Сконцентрований на нанесенні шкоди";
    }
    analysis["teamPlay"] = teamPlay;

    return analysis;
}
