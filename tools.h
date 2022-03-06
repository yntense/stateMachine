#ifndef TOOLS_H
#define TOOLS_H

#include <QString>


class Tools
{
public:
    Tools();
    static QString getCpuId();
private:
    static QString getMessageFromMachine(QString command);
};

#endif // TOOLS_H
