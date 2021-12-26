#include <QValidator>

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    QIntValidator value(0, 1000000, this);
    ui->interval->setValidator(&value);
    led = new LedController(ui, this);
}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_Light_clicked()
{
    LedController::eLedEvent event = {
        .cmd = LedController::LIGHT
    };

    emit led->controlLedState(event);
}

void Widget::on_Blink_clicked()
{
    bool ok = false;
    QString intervalString = ui->interval->text();
    intervalString.toInt(&ok, 10);
    if(ok){
        LedController::eLedEvent event = {
            .cmd = LedController::BLINK,
            .msg = intervalString
        };

        emit led->controlLedState(event);
    }


}

void Widget::on_Close_clicked()
{
    LedController::eLedEvent event = {
        .cmd = LedController::CLOSE
    };

    emit led->controlLedState(event);
}

