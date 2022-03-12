#ifndef MYAPPCONFIG_H
#define MYAPPCONFIG_H

#include <QObject>

#include "appconfig.h"

class MyAPPConfig : public APPConfig
{
    Q_OBJECT
public:
    typedef enum {
        MQTT,
        MQTT_CLIENT_ID,
        MQTT_DOMAIN,
        MQTT_PASSWD,
        MQTT_PORT,
        MQTT_USERNAME,
        LOG,
        LOG_PATH
    }eAPPConfig;

    static MyAPPConfig * getAppConfig();
    QString getAppConfigKey(eAPPConfig eConfig);
signals:

private:
    explicit MyAPPConfig(APPConfig *parent = nullptr);
    QHash<int, QString> m_config;
    static MyAPPConfig * m_appConfig;
protected:
    void onSetdefault(QMap<QString, QMap<QString, QVariant>> &m_mapMultiConfig) override;


};

#endif // MYAPPCONFIG_H
