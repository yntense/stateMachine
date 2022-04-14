
#include <QTimer>
#include <QDebug>

#include "handlemessage.h"


HandleMessage::HandleMessage(QObject *parent) : QObject(parent)
{

}

void HandleMessage::registerMessageListen(const QString &name, const MessageDevice *subDevice)
{
    m_recordDevice.insert(name, subDevice);
}

void HandleMessage::registerMessageInput(const QString &name, const MessageDevice *inputDevice)
{
    m_recordInputDevice.insert(name, inputDevice);
}


void HandleMessage::onReceiveMessage(const QJsonObject &msg)
{
    qDebug() << "threadId" <<QThread::currentThreadId();
    if(m_handleMessage)
    {
        m_messageQueue.enqueue(msg);
    }else{
        m_messageQueue.enqueue(msg);
        QTimer::singleShot(0, this, &HandleMessage::onHandleMessage);
    }
}

void HandleMessage::onReceiveMessageResponse(const QJsonObject msg)
{

    if(m_handleMessageResponse)
    {
        m_messageResponseQueue.enqueue(msg);
    }else{
        m_messageResponseQueue.enqueue(msg);
        QTimer::singleShot(0, this, &HandleMessage::onHandleMessageResponse);
    }
}

void HandleMessage::onHandleMessage()
{
    if(!m_messageQueue.empty())
    {
      m_handleMessage = true;
      QJsonObject message = m_messageQueue.dequeue();
      QString destSubDevice = message.value("destSubDevice").toString();
      if(m_recordDevice.contains(destSubDevice))
      {
         emit m_recordDevice.value(destSubDevice)->sendMessage(message);
      }
      if(!m_messageQueue.empty())
      {
         QTimer::singleShot(0, this, &HandleMessage::onHandleMessage);
      }else{
           m_handleMessage = false;
      }
    }
}

void HandleMessage::onHandleMessageResponse()
{
    if(!m_messageResponseQueue.empty())
    {
      m_handleMessageResponse = true;
      QJsonObject message = m_messageResponseQueue.dequeue();
      QString inputDevice = message.value("MessageType").toString();
      if(m_recordInputDevice.contains(inputDevice))
      {
         message.remove("MessageType");
         qDebug() << inputDevice;
         emit m_recordInputDevice.value(inputDevice)->sendMessage(message);
      }else if(inputDevice == "ALL")
      {
          QList< const MessageDevice * > inputDevices =  m_recordInputDevice.values();
          for(int i = 0; i < inputDevices.size(); i++)
          {
              emit inputDevices[i]->sendMessage(message);
          }
      }
      if(!m_messageResponseQueue.empty())
      {
         QTimer::singleShot(0, this, &HandleMessage::onHandleMessageResponse);
      }else{
           m_handleMessageResponse = false;
      }
    }
}

