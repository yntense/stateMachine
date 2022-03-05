#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QJsonObject>

#include <messagedevice.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class LedController : public MessageDevice
{
    Q_OBJECT
public:
    typedef enum {
        LIGHT,
        CLOSE,
        BLINK
    }eLedState,eLedCmd;

    typedef struct {
        eLedCmd cmd;
        QString msg;
    }eLedEvent;

    explicit LedController(QObject *parent = nullptr);
    explicit LedController(Ui::Widget *ui, QObject *parent = nullptr);



private:
    eLedState m_ledState = CLOSE;
    QQueue<QJsonObject> m_messageQueue;
    bool m_handleMessage = false;
    QTimer *timer;
    Ui::Widget *m_ui;
    QPixmap *ledLightPic;
    QPixmap *ledClosePic;

signals:
    void controlLedState(const QJsonObject &msg);

private slots:
    void onHandleLedEvent();
    void onHandleTimeOut();
    void onHandleMessage(const QJsonObject &msg);

private:
    void startBlink(int interval);
    void stopBlink();
    void closeLed();
    void lightLed();


};

#endif // LEDCONTROLLER_H
