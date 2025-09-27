#ifndef REPLAYPARSER_H
#define REPLAYPARSER_H

#include <QObject>
#include <QVariantMap>

// ❗️ НОВА СТРУКТУРА: Описує подію нанесення шкоди
struct ShotEvent {
    float timestamp = 0.0f;
    quint32 attackerId = 0;
    quint32 targetId = 0;
    quint16 damage = 0;
    quint16 criticalFlags = 0; // Бітмаска для критів
    quint16 shellId = 0;       // ID снаряда
    bool isRicochet = false;
    bool isPenetration = false;

    // Допоміжна функція для легкої конвертації у QVariantMap
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["timestamp"] = timestamp;
        map["attackerId"] = attackerId;
        map["targetId"] = targetId;
        map["damage"] = damage;
        map["criticalFlags"] = criticalFlags;
        map["shellId"] = shellId;
        map["isRicochet"] = isRicochet;
        map["isPenetration"] = isPenetration;
        return map;
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
