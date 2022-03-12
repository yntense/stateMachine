#include <QApplication>

#include "widget.h"
#include "Tools/log/log.h"
#include "Tools/myappconfig.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Log log;
#ifdef  QT_NO_DEBUG
    qInstallMessageHandler(log.outputMessage);//安装消息处理程序
#endif
    MyAPPConfig::getAppConfig();
    Widget w;
    w.show();
    return a.exec();
}
