#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QJsonObject>
#include <QMqttClient>

#include <messagedevice.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class LedController : public MessageDevice
{
    Q_OBJECT
public:
    typedef enum {
        LIGHT = 0,
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
    qint32 m_blinkInterval = 0;

signals:
    void controlLedState(const QJsonObject &msg);
    void messageReponse(const QJsonObject response);

private slots:
    void onHandleLedEvent();
    void onHandleTimeOut();
    void onHandleMessage(const QJsonObject &msg);
    void onStateChange(const eLedState &state, int interval, const QString &msg);

public slots:
    void onListenNetState(QMqttClient::ClientState state);


private:
    int8_t startBlink(int interval);
    int8_t stopBlink();
    int8_t closeLed();
    int8_t lightLed();


};

#endif // LEDCONTROLLER_H
