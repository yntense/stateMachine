
#include <QDateTime>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QString>
#include <QJsonDocument>
#include <QTimer>
#include <QThread>

#include "mqttreceiver.h"
#include "tools.h"

#define GEN_LOCAL_TOPIC(id) (QString("/%1/control").arg(id))
#define GEN_PUBLIC_TOPIC(id) (QString("/%1/device/control").arg(id))
#define GEN_RESPONSE_TOPIC(id) (QString("/%1/control/response").arg(id))
#define GEN_DEVICE_STATE_TOPIC(id) (QString("/%1/state").arg(id))


MQTTReceiver::MQTTReceiver(QObject *parent) : MessageDevice(parent)
{
    qDebug() << "MQTTReceiver 在线程ID为: " << QThread::currentThreadId()  << " 中运行";
    connect(this, &MQTTReceiver::start, this, &MQTTReceiver::onStart);
}

void MQTTReceiver::init(QString hostname, QString username, QString password)
{
    QString clientId = "";
    m_hostname = hostname;
    m_username = username;
    m_password = password;
    clientId = Tools::getCpuId();
    if(clientId != "fail")
    {
       m_clientId = clientId;
    }else{
        qCritical() << "Getting cpu id failed!";
    }
}

void MQTTReceiver::onResponse()
{

}

void MQTTReceiver::onStart()
{
    qDebug() << "QMqttClient 在线程ID为: " << QThread::currentThreadId()  << " 中运行";
    m_client = new QMqttClient();       //QMqttClient 对象

    m_client->setHostname(m_hostname);
    m_client->setPort(MQTT_PORT);
    m_client->setUsername(m_username);
    m_client->setPassword(m_password);
    m_client->setWillTopic(GEN_DEVICE_STATE_TOPIC(m_clientId));
    m_client->setWillRetain(true);
    m_client->setWillMessage("offline");


    connect(m_client, &QMqttClient::stateChanged, this, &MQTTReceiver::onStateChanged);
    connect(m_client, &QMqttClient::disconnected, this, &MQTTReceiver::onBrokerDisconnected);
    connect(m_client, &QMqttClient::connected, this, &MQTTReceiver::onBrokerConnected);
    connect(m_client, &QMqttClient::messageReceived, this, &MQTTReceiver::onMessageReceived);
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {

        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
         qDebug()<< content;

    });
    QTimer::singleShot(0, this, &MQTTReceiver::onConnectBroker);

}

void MQTTReceiver::onConnectBroker()
{
    switch (m_mqttState)
    {
        case QMqttClient::Disconnected:
            m_client->connectToHost();
        break;
    default:
        break;

    }

}

void MQTTReceiver::onBrokerDisconnected()
{
    qDebug()<< "state disconnected";

}

void MQTTReceiver::onBrokerConnected()
{
    qDebug() << "state connected";
    emit myState(m_mqttState);
    m_localControlTopic = GEN_LOCAL_TOPIC(m_clientId);
    QMqttTopicFilter topic(m_localControlTopic);
    qDebug()<< "topic :" << m_localControlTopic;
    auto subscription = m_client->subscribe(topic);
    if (!subscription) {
        qDebug()<< "subscribe failed";
        return;
    }
}

void MQTTReceiver::onStateChanged(QMqttClient::ClientState state)
{
    //更新网络状态
    emit myState(m_mqttState);
    m_mqttState = state;
    switch (state)
    {
        case QMqttClient::Disconnected:
            QTimer::singleShot(m_reConnectInterval, this, &MQTTReceiver::onConnectBroker);
        break;
    default:
        break;

    }
}

void MQTTReceiver::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
   bool hasPublicControlTopic = false;
   //有其他设备的订阅
   if(m_publicControlTopic.filter(topic.name()).size() > 0)
   {
        hasPublicControlTopic = true;
   }

   if(topic.name() == m_localControlTopic || hasPublicControlTopic)
   {
        QJsonParseError error;
        QJsonDocument controlMessage = QJsonDocument::fromJson(message, &error);
        if(controlMessage.isNull())
        {
            qDebug() << QString("Null Message from %1, error: %2, from msg: %3").arg(topic.name()).arg(error.errorString()).arg(QString(message).simplified());
            return ;
        }
        QJsonObject controlJson = controlMessage.object();
        if(controlJson.value("destSubDevice") == "mqtt")
        {
            parseMessage(controlJson);
            return ;
        }
        qDebug() << controlJson;
        controlJson.insert("MessageType", "mqtt");
        emit dispatchMessage(controlJson);
   }
}

void  MQTTReceiver::parseMessage(const QJsonObject &msg)
{
    switch (msg.value("operation").toInt())
    {
        case 0x01:
            qDebug() << msg;
            if (msg.value("arg").toInt() == 0)
            {
                QString publicTopic = GEN_PUBLIC_TOPIC(msg.value("payload").toString());
                if(m_publicControlTopic.filter(publicTopic).size() == 0)
                {
                    QMqttTopicFilter topic(publicTopic);
                    qDebug()<< "topic :" << GEN_PUBLIC_TOPIC(msg.value("payload").toString());
                    auto subscription = m_client->subscribe(topic);
                    m_publicControlTopic.append(publicTopic);
                }

            }
        break;
    default:
        break;
    }
}

void MQTTReceiver::onHandleMessage(const QJsonObject &msg)
{
    if(m_handleMessage)
    {
        m_messageQueue.enqueue(msg);
    }else{
        m_messageQueue.enqueue(msg);
        QTimer::singleShot(0, this, &MQTTReceiver::onHandleMessageResponse);
    }
}


void MQTTReceiver::onHandleMessageResponse()
{
    if (!m_messageQueue.empty()) {
        //说明正在处理信息
        m_handleMessage = true;
        QJsonObject responseMessage = m_messageQueue.head();
        QStringList keys = responseMessage.keys();
        //包含状态，说明为更新 device 状态
        if(keys.contains("State") && !keys.contains("clientID"))
        {
            //发送到设备状态主题
            sendMessgebyTopic(QMqttTopicName(GEN_DEVICE_STATE_TOPIC(m_clientId)), responseMessage, 0, true);

        }else{
            sendMessgebyTopic(QMqttTopicName(GEN_RESPONSE_TOPIC(responseMessage.value("clientID").toString())),responseMessage);
        }
        m_handleMessage = false;
    }
}

void MQTTReceiver::sendMessgebyTopic(const QMqttTopicName &topic, const QJsonObject &responseMessage, quint8 qos , bool retain )
{
    static int transmitCount = 0;
    if(m_client->publish(topic, QString(QJsonDocument(responseMessage).toJson()).toUtf8(), qos, retain) != -1)
    {
        m_messageQueue.dequeue();
        if(!m_messageQueue.empty())
        {
            QTimer::singleShot(0, this, &MQTTReceiver::onHandleMessageResponse);
        }
    }else
    {
        transmitCount++;
        // 重发间隔 * 重发次数 = 等于重发总时长
       if(transmitCount == m_reTransmitCount)
       {
           m_messageQueue.dequeue();
           if(!m_messageQueue.empty())
           {
               QTimer::singleShot(0, this, &MQTTReceiver::onHandleMessageResponse);
           }
       }else
       {
           QTimer::singleShot(m_reTransmitInterval, this, &MQTTReceiver::onHandleMessageResponse);
       }
    }
}



