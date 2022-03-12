#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QSettings>
#include <QMap>

class APPConfig : public QObject
{
    Q_OBJECT
public:
    explicit APPConfig(QObject *parent);
    void writeConfig(const QString &group, const QString &key, const QVariant &value);
    QVariant readConfig(const QString &group, const QString &arg, const QVariant &defaultValue = QVariant());
    void init();
private:
    static APPConfig *m_appConfig;
    QSettings *m_settings;
    QStringList m_groupList;
    void setdefault(); //设置默认配置
protected:
    QMap<QString, QMap<QString, QVariant>> m_mapMultiConfig;
    virtual void onSetdefault(QMap<QString, QMap<QString, QVariant>> &m_mapMultiConfig) = 0;

};

#endif // APPCONFIG_H
