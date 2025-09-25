#include "recommendersystem.h"
#include <QVariant>

// --- Допоміжні функції для чистоти коду ---

void RecommenderSystem::addGunneryRecommendations(const QVariantMap &metrics, QStringList &recommendations)
{
    double accuracy = metrics.value("accuracy").toString().remove('%').toDouble();
    double penetrationRatio = metrics.value("penetrationRatio").toString().remove('%').toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();

    if (accuracy > 80.0 && penetrationRatio < 75.0) {
        recommendations.append("<b>Аналіз стрільби:</b> Ви добре влучаєте, але часто не пробиваєте. Це може означати стрільбу по занадто броньованих цілях. Спробуйте ретельніше вицілювати вразливі зони (НЛД, люки) або завантажте більше преміум-снарядів для критичних ситуацій.");
    }
    else if (accuracy < 75.0 && totalDamage > 1000) {
        recommendations.append("<b>Аналіз стрільби:</b> Ваша точність була невисокою. Не забувайте повністю зводитись, особливо на середніх та дальніх дистанціях. Кожен промах — це втрачена шкода та потенційна загроза для вас.");
    }

    if (penetrationRatio > 90.0 && totalDamage > 2000) {
        recommendations.append("<b>Позитивна відзнака:</b> Бездоганна реалізація пострілів. Ви чудово знаєте, куди стріляти, щоб завдати шкоди.");
    }
}

void RecommenderSystem::addSurvivabilityRecommendations(const QVariantMap &metrics, const QString &grade, QStringList &recommendations)
{
    QString vehicleType = metrics.value("vehicleType").toString();
    double blockedDamage = metrics.value("damageBlockedByArmor", 0.0).toDouble();
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();

    if (vehicleType.contains("heavy", Qt::CaseInsensitive) && blockedDamage < totalDamage && blockedDamage < 1000 && (grade == "Посередній" || grade == "Нижче середнього")) {
        recommendations.append("<b>Аналіз виживання (Важкий танк):</b> Броня — ваш головний інструмент, але в цьому бою вона майже не використовувалась. Намагайтеся бути на першій лінії, правильно позиціонувати корпус і приймати удари замість менш броньованих союзників.");
    }

    if (grade == "Жахливий" && totalDamage < 500 && metrics.value("spotted", 0).toInt() > 0) {
        recommendations.append("<b>Аналіз ранньої агресії:</b> Схоже, ви загинули на самому початку бою, проявивши надмірну агресію. Спробуйте на початку грати обережніше, збираючи інформацію про роз'їзд ворога, перш ніж вступати у відкриту конфронтацію.");
    }

    if (blockedDamage > totalDamage * 1.5 && blockedDamage > 3000) {
        recommendations.append("<b>Позитивна відзнака:</b> Майстерне використання броні. Ви поглинули колосальну кількість шкоди, що дозволило вашій команді діяти вільніше.");
    }
}

void RecommenderSystem::addTeamPlayRecommendations(const QVariantMap &metrics, const QString &grade, QStringList &recommendations)
{
    QString vehicleType = metrics.value("vehicleType").toString();
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();
    // ❗️ ВИПРАВЛЕНО: Додано оголошення змінної totalDamage
    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();

    if (vehicleType.contains("light", Qt::CaseInsensitive) && assistedDamage < 1000 && (grade == "Поганий" || grade == "Жахливий")) {
        recommendations.append("<b>Аналіз командної гри (Легкий танк):</b> Ваша основна задача — розвідка. Низькі показники допомоги свідчать про пасивну гру або неправильний вибір позицій. Вивчайте карти, щоб знати безпечні та ефективні маршрути для підсвічування.");
    }

    if (vehicleType.contains("medium", Qt::CaseInsensitive) && assistedDamage < 500 && (grade == "Посередній" || grade == "Нижче середнього")) {
        recommendations.append("<b>Аналіз командної гри (Середній танк):</b> Не забувайте, що СТ — це універсальні бійці. Окрім нанесення шкоди, важливо допомагати з розвідкою та підтримкою союзників на різних флангах.");
    }

    if (assistedDamage > totalDamage && assistedDamage > 3000) {
        recommendations.append("<b>Позитивна відзнака:</b> Чудова командна гра! Ваша розвідка та підтримка принесли величезну користь.");
    }
}

void RecommenderSystem::addStrategicRecommendations(const QVariantMap &metrics, const QVariantMap &analysis, const QVariantMap &replayData, QStringList &recommendations)
{
    QString grade = analysis.value("performanceGrade").toString();
    int kills = metrics.value("kills", 0).toInt();
    // Поки що результат бою не парситься, але якщо будете додавати, ця логіка стане в нагоді
    // bool isVictory = ... ;

    if ((grade == "Еталонний" || grade == "Дуже хороший") && kills <= 1 /* && !isVictory */) {
        recommendations.append("<b>Стратегічний аналіз:</b> Ви завдали величезну шкоду, але це не конвертувалося у фраги. Можливо, варто було фокусувати вогонь, щоб швидше виводити з гри ключових супротивників, навіть якщо вони не є найзручнішою ціллю для нанесення шкоди.");
    }

    if (kills > 2 && metrics.value("totalDamageDealt", 0.0).toDouble() < 2000 && (grade == "Хороший" || grade == "Непоганий")) {
        recommendations.append("<b>Позитивна відзнака:</b> Майстер добивання. Ви ефективно прибирали з гри супротивників з низьким запасом міцності. Виведення 'стволів' з бою — це ключовий навик, що веде до перемоги.");
    }
}


// --- Головний метод ---
RecommenderSystem::RecommenderSystem(QObject *parent) : QObject(parent) {}

QStringList RecommenderSystem::generate(const QVariantMap &replayData, const QVariantMap &metrics, const QVariantMap &analysis)
{
    QStringList recommendations;

    QString grade = analysis.value("performanceGrade").toString();
    if (grade.contains("помилка даних")) {
        recommendations.append("Рекомендації не можуть бути сформовані через помилку в даних реплею.");
        return recommendations;
    }

    // Викликаємо кожен блок аналізу окремо
    addGunneryRecommendations(metrics, recommendations);
    addSurvivabilityRecommendations(metrics, grade, recommendations);
    addTeamPlayRecommendations(metrics, grade, recommendations);
    addStrategicRecommendations(metrics, analysis, replayData, recommendations);

    // Якщо жодної конкретної поради не було згенеровано, даємо загальний вердикт
    if (recommendations.isEmpty()) {
        if (grade == "Еталонний" || grade == "Дуже хороший") {
            recommendations.append("<b>Чудовий бій:</b> Ви продемонстрували високий клас гри. Проаналізуйте реплей, щоб закріпити успішні дії та повторити їх у майбутньому.");
        } else if (grade == "Хороший" || grade == "Непоганий") {
            recommendations.append("<b>Гідний результат:</b> Ви зробили вагомий внесок у бій. Продовжуйте в тому ж дусі!");
        } else {
            recommendations.append("<b>Стандартний бій:</b> Непоганий результат. Щоб покращити ефективність, спробуйте сфокусуватися на одному з аспектів гри: вибір позиції, реалізація пострілів або читання мінікарти.");
        }
    }

    return recommendations;
}
