#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class LedController : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        LIGHT,
        BLINK,
        CLOSE
    }eLedState,eLedCmd;

    typedef struct {
        eLedCmd cmd;
        QString msg;
    }eLedEvent;

    explicit LedController(QObject *parent = nullptr);
    explicit LedController(Ui::Widget *ui, QObject *parent = nullptr);


private:
    eLedState m_ledState = CLOSE;
    QQueue<eLedEvent> m_cmdQueue;
    bool m_handleMessage = false;
    QTimer *timer;
    Ui::Widget *m_ui;
    QPixmap *ledLightPic;
    QPixmap *ledClosePic;

signals:
    void controlLedState(eLedEvent enent);

private slots:
    void onControlLedState(eLedEvent enent);
    void onHandleLedEvent();
    void onHandleTimeOut();

private:
    void startBlink(int interval);
    void stopBlink();
    void closeLed();
    void lightLed();


};

#endif // LEDCONTROLLER_H
