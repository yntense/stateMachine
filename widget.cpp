#include <QValidator>

#include "widget.h"
#include "ui_widget.h"
#include "mqttreceiver.h"
#include "tools.h"
#include <QJsonObject>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

   ui->setupUi(this);
   QIntValidator value(0, 1000000, this);
   ui->interval->setValidator(&value);
   led = new LedController(ui, this);

   MQTTReceiver *m_MQTTReceiver = new MQTTReceiver();
   m_MQTTReceiver->moveToThread(&m_MQTTReceiverThread);
   connect(&m_MQTTReceiverThread, &QThread::finished, m_MQTTReceiver, &QObject::deleteLater);
   m_MQTTReceiverThread.start();
   m_MQTTReceiver->init("www.beatingcai.com", "test", "public");
   //该信号发出之后，m_MQTTReceiver的成员函数是在 m_MQTTReceiverThread 运行
   emit m_MQTTReceiver->start();

    // HandleMessage 运行在其他线程
    HandleMessage *messageCenter = new HandleMessage();
    messageCenter->moveToThread(&m_handleMessageThread);
    connect(&m_handleMessageThread, &QThread::finished, messageCenter, &QObject::deleteLater);
    m_handleMessageThread.start();

    connect(m_MQTTReceiver, &MQTTReceiver::dispatchMessage, messageCenter, &HandleMessage::onReceiveMessage);
    messageCenter->registerMessageListen("led", (MessageDevice *)led);
    messageCenter->registerMessageInput("mqtt", (MessageDevice *)m_MQTTReceiver);

    connect(led, &LedController::messageReponse, messageCenter, &HandleMessage::onReceiveMessageResponse);
    connect(m_MQTTReceiver, &MQTTReceiver::myState, led, &LedController::onListenNetState);

}

Widget::~Widget()
{
    m_handleMessageThread.quit();
    m_handleMessageThread.wait();
    m_MQTTReceiverThread.quit();
    m_MQTTReceiverThread.wait();
    delete ui;
}



void Widget::on_Light_clicked()
{
    QJsonObject msg{
        {"msgId",100},
        {"clientID", "mini-code"},
        {"operation", 0},
        {"destSubDevice", "led"},
        {"arg",0},
        {"payload", 500}
    };

    emit led->controlLedState(msg);
}

void Widget::on_Blink_clicked()
{

    bool ok = false;
    QString intervalString = ui->interval->text();
    int interval = intervalString.toInt(&ok, 10);
    if(ok){
        QJsonObject msg{
            {"msgId",100},
            {"clientID", "mini-code"},
            {"operation", 0},
            {"destSubDevice", "led"},
            {"arg",2},
            {"payload", interval}
        };
        emit led->controlLedState(msg);
    }

}

void Widget::on_Close_clicked()
{
    QJsonObject msg{
        {"msgId",100},
        {"clientID", "mini-code"},
        {"operation", 0},
        {"destSubDevice", "led"},
        {"arg",1},
        {"payload", 500}
    };

    emit led->controlLedState(msg);
}

