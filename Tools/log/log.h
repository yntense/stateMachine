#ifndef LOG_H
#define LOG_H

#include <QDebug>
#include <QFile>

class Log:public QObject
{
     Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);
    ~Log();

    static void outputMessage(QtMsgType type,const QMessageLogContext &context,const QString &msg);

private:
    static QHash<QtMsgType , QFile *> m_logFileNames;

};


#endif // LOG_H
