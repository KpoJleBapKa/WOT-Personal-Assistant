#include "recommendersystem.h"

RecommenderSystem::RecommenderSystem(QObject *parent) : QObject(parent) {}

QStringList RecommenderSystem::generate(const QVariantMap &metrics, const QVariantMap &analysis)
{
    QStringList recommendations;

    // Отримуємо оцінку та інші дані
    QString grade = analysis.value("performanceGrade").toString();
    QString vehicleType = metrics.value("vehicleType").toString();
    double blockedDamage = metrics.value("damageBlockedByArmor", 0.0).toDouble();
    double assistedDamage = metrics.value("damageAssisted", 0.0).toDouble();


    if (grade == "Еталонний" || grade == "Дуже хороший") {
        recommendations.append("<b>Видатний результат!</b> Ваша гра в цьому бою була зразковою для техніки цього класу.");
    }
    else if (grade == "Хороший" || grade == "Непоганий") {
        recommendations.append("<b>Хороший бій!</b> Ви зробили значний внесок у результат, продовжуйте в тому ж дусі.");
    }
    else if (grade == "Посередній") {
        recommendations.append("<b>Стабільний результат.</b> Щоб досягти більшого, спробуйте аналізувати мінікарту для вибору більш вигідних позицій.");
    }
    else if (grade == "Нижче середнього" || grade == "Поганий") {
        recommendations.append("<b>Є куди рости.</b> Спробуйте довше залишатися у бою та реалізовувати постріли. Не поспішайте виїжджати на відкриті позиції.");
    }
    else if (grade == "Жахливий") {
        recommendations.append("<b>Невдалий бій.</b> Проаналізуйте, що пішло не так: можливо, це була помилка на старті або невдалий роз'їзд. Кожен такий бій — це досвід.");
    }


    // Додаткові, більш конкретні поради
    if (vehicleType.contains("heavy", Qt::CaseInsensitive) && blockedDamage < 1000 && (grade == "Поганий" || grade == "Нижче середнього")) {
        recommendations.append("<b>Порада для важкого танка:</b> Схоже, вам не вдалося використати броню. Намагайтеся не підставляти вразливі зони та 'танкувати' від рельєфу або будівель.");
    }
    if (vehicleType.contains("light", Qt::CaseInsensitive) && assistedDamage < 1500 && (grade == "Поганий" || grade == "Нижче середнього")) {
        recommendations.append("<b>Порада для легкого танка:</b> Основна ваша сила — в огляді та маскуванні. Вивчайте ключові позиції для розвідки, щоб приносити максимум користі, не отримуючи шкоди.");
    }


    return recommendations;
}
