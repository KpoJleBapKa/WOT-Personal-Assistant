#ifndef METRICSCALCULATOR_H
#define METRICSCALCULATOR_H

#include <QObject>
#include <QVariantMap>

class MetricsCalculator : public QObject
{
    Q_OBJECT
public:
    explicit MetricsCalculator(QObject *parent = nullptr);
    QVariantMap calculate(const QVariantMap &replayData);
};

#endif // METRICSCALCULATOR_H
