#ifndef REPLAYPARSER_H
#define REPLAYPARSER_H

#include <QObject>
#include <QVariantMap>

class ReplayParser : public QObject
{
    Q_OBJECT
public:
    explicit ReplayParser(QObject *parent = nullptr);
    QVariantMap parse(const QString &filePath);

private:
    QByteArray decompress(const QByteArray &data);
};

#endif // REPLAYPARSER_H
