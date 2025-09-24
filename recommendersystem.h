#ifndef RECOMMENDERSYSTEM_H
#define RECOMMENDERSYSTEM_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

class RecommenderSystem : public QObject
{
    Q_OBJECT
public:
    explicit RecommenderSystem(QObject *parent = nullptr);
    QStringList generate(const QVariantMap &metrics, const QVariantMap &analysis);
};

#endif // RECOMMENDERSYSTEM_H
