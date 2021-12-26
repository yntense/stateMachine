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
LedController::LedController(QObject *parent) : QObject(parent)
{

}

LedController::LedController(Ui::Widget *ui, QObject *parent) : QObject(parent)
{
    //注册自定义类型，才能用于信号与槽的传递
    qRegisterMetaType<eLedEvent>("LedController::eLedEvent");
    m_ui = ui;
    connect(this, &LedController::controlLedState, this,  &LedController::onControlLedState);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LedController::onHandleTimeOut);
    ledLightPic = new QPixmap(QString::fromLocal8Bit(":/resource/ledLight.png"));
    ledClosePic = new QPixmap(QString::fromLocal8Bit(":/resource/ledClose.png"));
    closeLed();
}

void LedController::onControlLedState(eLedEvent enent)
{
    if(m_handleMessage)
    {
        m_cmdQueue.enqueue(enent);
    }else{
        m_cmdQueue.enqueue(enent);
        QTimer::singleShot(0, this, &LedController::onHandleLedEvent);
    }
}

void LedController::onHandleLedEvent()
{
    if (!m_cmdQueue.empty()) {
        //说明正在处理信息
        m_handleMessage = true;
        eLedEvent ledEvent = m_cmdQueue.dequeue();
        QString  msg = ledEvent.msg;

        //根据Led状态散转
        switch(m_ledState)
        {
            case LIGHT:   //当前 Led 已经被点亮
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        qDebug() << "当前 Led 已经为点亮状态";
                    break;
                    case BLINK:
                        qDebug() << "当前 Led 为点亮状态, 转换为闪烁状态";
                        m_ledState = BLINK;
                        startBlink(msg.toInt());
                    break;
                    case CLOSE:
                        qDebug() << "当前 Led 为点亮状态, 转换为关闭状态";
                        m_ledState = CLOSE;
                        closeLed();
                    break;

                    default:
                    break;
                }
            break;
            case BLINK:   //当前 Led 已经是闪烁状态
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        qDebug() << "当前 Led 为闪烁状态，转换为点亮状态";
                        m_ledState = LIGHT;
                        stopBlink();
                        lightLed();
                    break;
                    case BLINK:
                        qDebug() << "当前 Led 为闪烁状态，修改闪烁间隔为 "+ msg + " ms";
                        startBlink(msg.toInt());
                        m_ledState = BLINK;
                    break;
                    case CLOSE:
                        qDebug() << "当前 Led 为闪烁状态，转换为关闭状态";
                        m_ledState = CLOSE;
                        stopBlink();
                        closeLed();
                    break;

                    default:
                    break;
                }
            break;
            case CLOSE:   //当前 Led 已经是关闭状态
                switch (ledEvent.cmd)
                {
                    case LIGHT:
                        qDebug() << "当前 Led 为关闭状态，转换为点亮状态";
                        m_ledState = LIGHT;
                        lightLed();
                    break;
                    case BLINK:
                        qDebug() << "当前 Led 为关闭状态，转换为闪烁状态,间隔为 " + msg + " ms";
                        m_ledState = BLINK;
                        startBlink(msg.toInt());
                    break;
                    case CLOSE:
                        qDebug() << "当前 Led 已经为关闭状态";
                    break;

                    default:
                    break;
                }
            break;
        }
         m_handleMessage = false;
    }
}

void LedController::startBlink(int interval)
{
    timer->start(interval);
}

void LedController::stopBlink()
{
    timer->stop();
}


void LedController::closeLed()
{
    m_ui->ledImage->setPixmap(*ledClosePic);
    m_ui->ledImage->setScaledContents(true);
}

void LedController::lightLed()
{
    m_ui->ledImage->setPixmap(*ledLightPic);
    m_ui->ledImage->setScaledContents(true);
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


