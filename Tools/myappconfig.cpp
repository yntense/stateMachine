#include "myappconfig.h"

#include <QMap>
#include <QDebug>

//MQTT
#define APP_MQTT    "MQTT"
#define DOMAIN    "domain"
#define PORT      "port"
#define PASSWD    "passwd"
#define USERNAME  "username"
#define CLIENT_ID "clientId"

//LOG
#define APP_LOG      "LOG"
#define APP_LOG_PATH "path"

MyAPPConfig * MyAPPConfig::m_appConfig = nullptr;
MyAPPConfig::MyAPPConfig(APPConfig *parent): APPConfig(parent)
{
    init();
    //获取相应的 key
    m_config.insert(MQTT, APP_MQTT);
    m_config.insert(MQTT_CLIENT_ID, DOMAIN);
    m_config.insert(MQTT_DOMAIN, PORT);
    m_config.insert(MQTT_PASSWD, PASSWD);
    m_config.insert(MQTT_PORT, USERNAME);
    m_config.insert(MQTT_USERNAME, CLIENT_ID);

    m_config.insert(LOG, APP_LOG);
    m_config.insert(LOG_PATH, APP_LOG_PATH);
}

MyAPPConfig *MyAPPConfig::getAppConfig()
{
     if(m_appConfig == nullptr){
        m_appConfig = new MyAPPConfig();
     }
     return m_appConfig;
}
/**
 * @brief onsetdefaultCLK
 * @param m_mapMultiConfig 使用 mapMultiConfig 添加自己的配置
 */
void MyAPPConfig::onSetdefault(QMap<QString, QMap<QString, QVariant>> &mapMultiConfig)
{
    //写配置
    QMap<QString, QVariant> mqttConfig;
    QString groupName = APP_MQTT;
    mqttConfig.insert(DOMAIN, "www.beatingcai.com");
    mqttConfig.insert(PORT, 1883);
    mqttConfig.insert(USERNAME, "");
    mqttConfig.insert(PASSWD, "");
    mqttConfig.insert(CLIENT_ID, "");
    mapMultiConfig.insert(groupName,mqttConfig);

    QMap<QString, QVariant> logConfig;
    groupName = APP_LOG;
    logConfig.insert(APP_LOG_PATH, "./");
    mapMultiConfig.insert(groupName,logConfig);
}

QString MyAPPConfig::getAppConfigKey(eAPPConfig eConfig)
{
    return m_config.value(eConfig, "");
}

