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

#define mqttreceiver
#ifdef mqttreceiver
   MQTTReceiver *m_MQTTReceiver = new MQTTReceiver();
   m_MQTTReceiver->init("www.beatingcai.com", "test", "public");
   emit m_MQTTReceiver->start();
#endif

    messageCenter = new HandleMessage();
    connect(m_MQTTReceiver, &MQTTReceiver::dispatchMessage, messageCenter, &HandleMessage::onReceiveMessage);
    messageCenter->registerMessageListen("led", (MessageDevice *)led);
    messageCenter->registerMessageInput("mqtt", (MessageDevice *)m_MQTTReceiver);

    connect(led, &LedController::messageReponse, messageCenter, &HandleMessage::onReceiveMessageResponse);
}

Widget::~Widget()
{
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

