#ifndef MESSAGEDEVICE_H
#define MESSAGEDEVICE_H

#include <QObject>

class MessageDevice : public QObject
{
    Q_OBJECT
public:
    explicit MessageDevice(QObject *parent = nullptr);
    ~MessageDevice();


signals:
    void sendMessage(const QJsonObject &msg) const;

private slots:
    virtual void onHandleMessage(const QJsonObject &msg) ;

};

#endif // MESSAGEDEVICE_H
