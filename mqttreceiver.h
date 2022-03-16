#ifndef MQTTRECEIVER_H
#define MQTTRECEIVER_H

#include <QObject>
#include <QMqttClient>
#include <QQueue>
#include <QJsonObject>

#include "messagedevice.h"

#define MQTT_PORT 1883

class MQTTReceiver : public MessageDevice
{
    Q_OBJECT
public:
    explicit MQTTReceiver(QObject *parent = nullptr);

    void init(QString hostname, QString username, QString password);

signals:
    void dispatchMessage(const QJsonObject &msg);
    void myState(QMqttClient::ClientState);     //更新MQTT的联网状态
    void start();

public slots:
    void onResponse();
    void onStart();

private slots:
    void onConnectBroker();
    void onBrokerDisconnected();
    void onBrokerConnected();
    void onStateChanged(QMqttClient::ClientState state);
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    //接收响应信息
    void onHandleMessage(const QJsonObject &response);
    void onHandleMessageResponse();

private:
    QMqttClient *m_client;
    QString m_hostname;
    QString m_username;
    QString m_password;
    QString m_clientId = "test";

    int m_reConnectInterval = 3000;    //MQTT默认重连时间
    QMqttClient::ClientState m_mqttState = QMqttClient::Disconnected;

    int m_reTransmitCount = 3;          //默认重传次数
    int m_reTransmitInterval = 3000;    //默认重传时间

    QString m_localControlTopic = ""; //用于控制本设备
    QStringList m_publicControlTopic; //用于接收其他设备对其发出控制命令

    void parseMessage(const QJsonObject &msg);
    bool m_handleMessage = false;
    QQueue<QJsonObject> m_messageQueue;

    void sendMessgebyTopic(const QMqttTopicName &topic, const QJsonObject &responseMessage, quint8 qos = 0, bool retain = false);
};

#endif // MQTTRECEIVER_H
