#ifndef BEHAVIORANALYZER_H
#define BEHAVIORANALYZER_H

#include <QObject>
#include <QVariantMap>

class BehaviorAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit BehaviorAnalyzer(QObject *parent = nullptr);
    QVariantMap analyze(const QVariantMap &replayData, const QVariantMap &metrics);
};

#endif // BEHAVIORANALYZER_H
