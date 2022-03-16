#include <QTimer>
#include <QDebug>

#include "ledcontroller.h"
#include "ui_widget.h"

/**
 * @author 公众号：嵌入式小屋
 *         github：https://github.com/yntense/stateMachine.git
 * @brief LedController::LedController
 * @param parent
 */
LedController::LedController(QObject *parent) : MessageDevice(parent)
{

}

LedController::LedController(Ui::Widget *ui, QObject *parent) : MessageDevice(parent)
{
    //注册自定义类型，才能用于信号与槽的传递
    qRegisterMetaType<eLedEvent>("LedController::eLedEvent");
    qRegisterMetaType<QMqttClient::ClientState>("QMqttClient::ClientState");
    m_ui = ui;
    connect(this, &LedController::controlLedState, this,  &LedController::onHandleMessage);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LedController::onHandleTimeOut);
    ledLightPic = new QPixmap(QString::fromLocal8Bit(":/resource/ledLight.png"));
    ledClosePic = new QPixmap(QString::fromLocal8Bit(":/resource/ledClose.png"));
    closeLed();

}


void LedController::onHandleMessage(const QJsonObject &msg)
{
    if(m_handleMessage)
    {
        m_messageQueue.enqueue(msg);
    }else{
        m_messageQueue.enqueue(msg);
        QTimer::singleShot(0, this, &LedController::onHandleLedEvent);
    }
}

void LedController::onHandleLedEvent()
{
    if (!m_messageQueue.empty()) {
        //说明正在处理信息
        m_handleMessage = true;
        QJsonObject ledMessage = m_messageQueue.dequeue();
        eLedEvent ledEvent;
        QString msg;
        qint32 interval = 0;
        switch (ledMessage.value("operation").toInt()) {
            case 0x00:
                ledEvent.cmd = (eLedCmd)ledMessage.value("arg").toInt();
                m_blinkInterval = ledMessage.value("payload").toInt();
            break;
        default:
            break;
        }
        interval = m_blinkInterval;
        QString response;
        int code = -1;

        //根据Led状态散转
        switch(m_ledState)
        {
            case LIGHT:   //当前 Led 已经被点亮
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        response = "当前 Led 已经为点亮状态";
                        qDebug() << "当前 Led 已经为点亮状态";
                        code = 0;
                    break;
                    case BLINK:
                        response = "当前 Led 为点亮状态, 转换为闪烁状态";
                        qDebug() << "当前 Led 为点亮状态, 转换为闪烁状态";
                        onStateChange(BLINK, interval, response);
                        m_ledState = BLINK;
                        startBlink(interval);
                        code = 0;
                    break;
                    case CLOSE:
                        response = "当前 Led 为点亮状态, 转换为关闭状态";
                        qDebug() << "当前 Led 为点亮状态, 转换为关闭状态";
                        onStateChange(CLOSE, interval, response);
                        m_ledState = CLOSE;
                        code = closeLed();
                    break;

                    default:
                    break;
                }
            break;
            case BLINK:   //当前 Led 已经是闪烁状态
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        response = "当前 Led 为闪烁状态，转换为点亮状态";
                        qDebug() << "当前 Led 为闪烁状态，转换为点亮状态";
                        onStateChange(LIGHT, interval, response);
                        m_ledState = LIGHT;
                        stopBlink();
                        code = lightLed();
                    break;
                    case BLINK:
                        response = "当前 Led 为闪烁状态，修改闪烁间隔为 "+ QString("%1").arg(interval) + " ms";
                        qDebug() << "当前 Led 为闪烁状态，修改闪烁间隔为 "+ QString("%1").arg(interval) + " ms";
                        onStateChange(BLINK, interval, response);
                        code = startBlink(interval);
                        m_ledState = BLINK;
                    break;
                    case CLOSE:
                        response = "当前 Led 为闪烁状态，转换为关闭状态";
                        qDebug() << "当前 Led 为闪烁状态，转换为关闭状态";
                        onStateChange(CLOSE, interval, response);
                        m_ledState = CLOSE;
                        stopBlink();
                        code = closeLed();
                    break;

                    default:
                    break;
                }
            break;
            case CLOSE:   //当前 Led 已经是关闭状态
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        response = "当前 Led 为关闭状态，转换为点亮状态";
                        qDebug() << "当前 Led 为关闭状态，转换为点亮状态";
                        onStateChange(LIGHT, interval, response);
                        m_ledState = LIGHT;
                        code = lightLed();
                    break;
                    case BLINK:
                        response = "当前 Led 为关闭状态，转换为闪烁状态,间隔为 " + QString("%1").arg(interval) + " ms";
                        qDebug() << "当前 Led 为关闭状态，转换为闪烁状态,间隔为 " + QString("%1").arg(interval) + " ms";
                        onStateChange(BLINK, interval, response);
                        m_ledState = BLINK;
                        code = startBlink(interval);
                    break;
                    case CLOSE:
                        response = "当前 Led 已经为关闭状态";
                        qDebug() << "当前 Led 已经为关闭状态";
                        onStateChange(CLOSE, interval, response);
                        code = 0;
                    break;

                    default:
                    break;
                }
            break;
        }
        QJsonObject responseJson = {
            {"msgId", ledMessage.value("msgId").toInt()},
            {"MessageType", ledMessage.value("MessageType").toString()},
            {"clientID", ledMessage.value("clientID").toString()},
            {"operation", ledMessage.value("operation").toInt()},
            {"destSubDevice", ledMessage.value("destSubDevice").toString()},
            {"status",code},
            {"msg", response}
        };
         emit messageReponse(responseJson);
         m_handleMessage = false;
    }
}

int8_t LedController::startBlink(int interval)
{
    timer->start(interval);
    return 0;
}

int8_t LedController::stopBlink()
{
    timer->stop();
    return 0;
}


int8_t LedController::closeLed()
{
    m_ui->ledImage->setPixmap(*ledClosePic);
    m_ui->ledImage->setScaledContents(true);
    return 0;
}

int8_t LedController::lightLed()
{
    m_ui->ledImage->setPixmap(*ledLightPic);
    m_ui->ledImage->setScaledContents(true);
    return 0;
}

/**
 *
 * @brief LedController::onHandleTimeOut
 */
void LedController::onHandleTimeOut()
{
    static bool show = true;
    if(show) {
        lightLed();
        show = false;
    }else {
        closeLed();
        show = true;
    }
}

/**
 *
 * @brief LedController::onStateChange
 */
void LedController::onStateChange(const eLedState &state, int interval, const QString &msg)
{
   if( state != m_ledState )
   {
       QJsonObject responseJson = {
           {"MessageType", "ALL"},
           {"State", m_ledState},
           {"interval", interval},
           {"msg", msg},
       };
      emit messageReponse(responseJson);
   }
}

/**
 * @brief onListenNetState 用于响应网络状态
 * @param state
 */
void LedController::onListenNetState(QMqttClient::ClientState state)
{
    qDebug() << state;
   if(QMqttClient::Connected == state)
   {

       //初始化设备状态
       QJsonObject responseJson = {
           {"MessageType", "ALL"},
           {"State", m_ledState},
           {"interval", m_blinkInterval},
           {"msg", "设备就绪"},
       };
      emit messageReponse(responseJson);
   }
}
