#include "recommendersystem.h"

RecommenderSystem::RecommenderSystem(QObject *parent) : QObject(parent) {}

QStringList RecommenderSystem::generate(const QVariantMap &metrics, const QVariantMap &analysis)
{
    QStringList recommendations;

    double penetrationRatio = metrics.value("penetrationRatio").toString().remove("%").toDouble();
    double accuracy = metrics.value("accuracy").toString().remove("%").toDouble();
    QString gameStyle = analysis.value("gameStyle").toString();

    if (penetrationRatio < 70.0) {
        recommendations.append("<b>Покращення пробиття:</b> Спробуйте краще вицілювати вразливі зони супротивників або використовуйте підкаліберні/кумулятивні снаряди для броньованих цілей.");
    }

    if (accuracy < 75.0) {
        recommendations.append("<b>Підвищення точності:</b> Не стріляйте на ходу, якщо це не дозволяє стабілізація гармати. Повністю зводьтеся перед пострілом для максимальної точності.");
    }

    if (gameStyle == "Пасивний") {
        recommendations.append("<b>Активність у бою:</b> Ваш внесок у бій був невисоким. Намагайтеся бути більш активним та реалізовувати потенціал вашої техніки, підтримуючи союзників на ключових напрямках.");
    }

    if (recommendations.isEmpty()) {
        recommendations.append("Ви провели хороший бій! Продовжуйте в тому ж дусі.");
    }

    return recommendations;
}
