#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "ledcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_Light_clicked();

    void on_Blink_clicked();

    void on_Close_clicked();

private:
    Ui::Widget *ui;
    LedController *led;
};
#endif // WIDGET_H
