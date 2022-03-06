#include "tools.h"

#include <QProcess>
#include <QDebug>

Tools::Tools()
{

}

QString Tools::getCpuId()
{
    QString result = "";
    QString command = "wmic cpu get processorid";
    result = getMessageFromMachine(command);
    QStringList resultList =  result.simplified().split(' ');
    if(resultList.size() == 2)
    {
       return resultList[1];
    }

    return "fail";
}

QString Tools::getMessageFromMachine(QString command)
{
    QProcess process;
    process.start(command);
    process.waitForFinished();
    QString result = QString::fromLocal8Bit(process.readAllStandardOutput());
    return result;
}
