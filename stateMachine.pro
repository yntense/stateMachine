QT += core gui network
QT += mqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Tools/appconfig.cpp \
    Tools/log/log.cpp \
    Tools/myappconfig.cpp \
    handlemessage.cpp \
    ledcontroller.cpp \
    main.cpp \
    messagedevice.cpp \
    mqttreceiver.cpp \
    tools.cpp \
    widget.cpp

HEADERS += \
    Tools/appconfig.h \
    Tools/log/log.h \
    Tools/myappconfig.h \
    handlemessage.h \
    ledcontroller.h \
    messagedevice.h \
    mqttreceiver.h \
    tools.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resource/2.png

RESOURCES += \
    Resource.qrc
