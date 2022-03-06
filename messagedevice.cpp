#include "messagedevice.h"

MessageDevice::MessageDevice(QObject *parent) : QObject(parent)
{
    connect(this, &MessageDevice::sendMessage, this, &MessageDevice::onHandleMessage);

}

MessageDevice::~MessageDevice()
{

}
