#ifndef REPLAYPARSER_H
#define REPLAYPARSER_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>

struct ShotEvent {
    float timestamp = 0.0;
    quint32 attackerId = 0;
    quint32 targetId = 0;
    qint16 damage = 0;

    QString attackerName;
    QString targetName;
    bool isFriendlyFire = false;

    bool isPenetration = false; // Пробиття
    bool isRicochet = false;    // Рикошет
    bool isShellExplosion = false; // Вибух снаряда (сплеш)
    bool isNoDamage = false;      // Влучання без шкоди (потрапив у екран/модуль)

    QStringList criticalHits; // Список критичних пошкоджень

    // Метод для зручного перетворення в QVariantMap для збереження
    QVariantMap toVariantMap() const {
        return {
            {"timestamp", timestamp},
            {"attackerId", attackerId},
            {"targetId", targetId},
            {"damage", damage},
            {"attackerName", attackerName},
            {"targetName", targetName},
            {"isFriendlyFire", isFriendlyFire},
            {"isPenetration", isPenetration},
            {"isRicochet", isRicochet},
            {"isShellExplosion", isShellExplosion},
            {"isNoDamage", isNoDamage},
            {"criticalHits", criticalHits}
        };
    }
};


class ReplayParser : public QObject
{
    Q_OBJECT
public:
    explicit ReplayParser(QObject *parent = nullptr);

    // Метод parse тепер повертатиме дані, що включають 'shot_events'
    QVariantMap parse(const QString &filePath);

private:
    QByteArray decryptStream(const QByteArray &encryptedData);
    void parsePackets(const QByteArray &stream, QVariantMap &out_data);
};

#endif // REPLAYPARSER_H
