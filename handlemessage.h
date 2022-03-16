#ifndef HANDLEMESSAGE_H
#define HANDLEMESSAGE_H

#include <QObject>
#include <QJsonObject>
#include <QQueue>
#include <QHash>
#include <QThread>

#include "messagedevice.h"


class HandleMessage : public QObject
{
    Q_OBJECT

public:
    explicit HandleMessage(QObject *parent = nullptr);
    void registerMessageListen(const QString &name, const MessageDevice *subDevice);
    void registerMessageInput(const QString &name, const MessageDevice *subDevice);


public slots:
    void onReceiveMessage(const QJsonObject &msg);
    void onHandleMessage();
    void onReceiveMessageResponse(const QJsonObject msg);
    void onHandleMessageResponse();


signals:

private:
    QQueue<QJsonObject> m_messageQueue;
    bool m_handleMessage = false;
    QHash<QString, const MessageDevice *> m_recordDevice; //记录被控制的设备

    QQueue<QJsonObject> m_messageResponseQueue;
    bool m_handleMessageResponse = false;
    QHash<QString, const MessageDevice *> m_recordInputDevice; //记录信息输入的对象

};

#endif // HANDLEMESSAGE_H
