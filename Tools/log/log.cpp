#include <QMutex>
#include <QDateTime>
#include <QDir>

#include "log.h"
#include "../myappconfig.h"
#include "../appconfig.h"

QHash<QtMsgType , QFile *> Log::m_logFileNames;
Log::Log(QObject *parent) : QObject(parent)
{
    //设置 qDebug() 的输出格式
    qSetMessagePattern("%{message} {File:%{file} Line:%{line} Function:%{function} DateTime:%{time [yyyy-MM-dd hh:mm:ss ddd]}}");

    MyAPPConfig * appConfig = MyAPPConfig::getAppConfig();
    QString log_path = appConfig->readConfig(appConfig->getAppConfigKey(MyAPPConfig::LOG),appConfig->getAppConfigKey(MyAPPConfig::LOG_PATH), "").toString();
    QFile *debugFile = nullptr;
    if((log_path != "") && QDir(log_path).exists())
    {
        debugFile = new QFile(log_path + QString(QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt")));

    }else
    {
        debugFile = new QFile(QString(QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt")));
    }
    //需要判断文件大小在此处增加
    //
    m_logFileNames.insert(QtDebugMsg, debugFile);
    debugFile->open(QIODevice::ReadWrite | QIODevice::Append);
}

Log::~Log()
{
    //关闭所有 Log 文件
    QList<QFile *> logFileList = m_logFileNames.values();
    for(int i = 0; i < logFileList.size(); i++)
    {
        logFileList[i]->close();
        logFileList[i]->deleteLater();
    }
}

void Log::outputMessage(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    QString text;
    QFile *log = nullptr;
    switch (type)
    {
        case QtDebugMsg:
            log = m_logFileNames.value(QtDebugMsg);
            //按之前设置好的输出格式，进行输出
            text = qFormatLogMessage(type,context,msg);
        break;
        case QtWarningMsg:
            text = QString("Warning:");
            break;
        case QtCriticalMsg:
            text = QString("Critical:");
            break;
        case QtFatalMsg:
            text = QString("Fatal:");
            break;
        default:
            text = QString("Debug:");
    }

    QTextStream stream(log);
    stream << text << "\r\n";
    log->flush();
    // 解锁
    mutex.unlock();
}
