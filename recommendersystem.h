#ifndef RECOMMENDERSYSTEM_H
#define RECOMMENDERSYSTEM_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>

class RecommenderSystem : public QObject
{
    Q_OBJECT
public:
    explicit RecommenderSystem(QObject *parent = nullptr);

    /**
     * @brief Генерує список персоналізованих рекомендацій на основі метрик та аналізу бою.
     * @param replayData "Сирі" дані з реплею (для стратегічного аналізу).
     * @param metrics Розраховані числові показники бою.
     * @param analysis Текстові висновки про поведінку гравця.
     * @return Список рекомендацій у форматі QStringList.
     */
    QStringList generate(const QVariantMap &replayData, const QVariantMap &metrics, const QVariantMap &analysis);

private:
    /**
     * @brief Додає рекомендації, пов'язані з ефективністю стрільби.
     */
    void addGunneryRecommendations(const QVariantMap &metrics, QStringList &recommendations);

    /**
     * @brief Додає рекомендації, пов'язані з виживанням та використанням броні.
     */
    void addSurvivabilityRecommendations(const QVariantMap &metrics, const QString &grade, QStringList &recommendations);

    /**
     * @brief Додає рекомендації, пов'язані з командною грою та розвідкою.
     */
    void addTeamPlayRecommendations(const QVariantMap &metrics, const QString &grade, QStringList &recommendations);

    /**
     * @brief Додає рекомендації, пов'язані зі стратегічним впливом на бій.
     */
    void addStrategicRecommendations(const QVariantMap &metrics, const QVariantMap &analysis, const QVariantMap &replayData, QStringList &recommendations);
};

#endif // RECOMMENDERSYSTEM_H
