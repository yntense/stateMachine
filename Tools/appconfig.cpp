#include <QDebug>
#include <QMutex>
#include <QFileInfo>

#include "appconfig.h"

#define CONFIG_FILE_NAME "./setting.ini"  //在应用的目录下生成配置文件

APPConfig::APPConfig(QObject *parent): QObject(parent)
{

}

void APPConfig::init()
{
    //用于查看配置文件是否存在
    if(!QFileInfo(CONFIG_FILE_NAME).isFile())
    {
        m_settings = new QSettings(CONFIG_FILE_NAME,QSettings::IniFormat);
        setdefault();
    }else
    {
        m_settings = new QSettings(CONFIG_FILE_NAME,QSettings::IniFormat);
    }
    m_groupList = m_settings->childGroups();
}

/**
 * @brief APPConfig::writeConfig
 * @param group group 配置文件的组名
 * @param key   想要写的键
 * @param value 想要写的值
 */
void APPConfig::writeConfig(const QString &group, const QString &key, const QVariant &value)
{
    m_settings->beginGroup(group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
}

/**
 * @brief APPConfig::readArg
 * @param group 配置文件的组名
 * @param arg   想要读取的参数
 * @param defaultValue 读不到时返回默认值
 * @return QVariant
 */
QVariant APPConfig::readConfig(const QString &group, const QString &arg, const QVariant &defaultValue)
{
    QVariant value;
    if(!m_groupList.contains(group))
    {
        return defaultValue;
    }
    m_settings->beginGroup(group);
    value = m_settings->value(arg, defaultValue);
    m_settings->endGroup();
    return value;
}

/**
 * @brief APPConfig::setdefaulet 默认配置文件中的内容
 */
void APPConfig::setdefault()
{

    onSetdefault(m_mapMultiConfig);
    qDebug()<< m_mapMultiConfig.keys();
    QStringList groupList = m_mapMultiConfig.keys();
    for(int groupIndex = 0; groupIndex < groupList.size(); groupIndex++ )
    {
        QMap<QString, QVariant> configs = m_mapMultiConfig.value(groupList[groupIndex]);
        QStringList keys = configs.keys();
        for(int configIndex = 0; configIndex < configs.size(); configIndex++)
        {
            writeConfig(groupList[groupIndex], keys[configIndex], configs.value(keys[configIndex]));
        }
    }
}


