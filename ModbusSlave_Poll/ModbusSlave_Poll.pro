#-------------------------------------------------
#
# Project created by QtCreator 2022-01-30T01:57:53
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModbusSlave_Poll

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    3rdparty/libmodbus/src/modbus.c \
    3rdparty/libmodbus/src/modbus-data.c \
    3rdparty/libmodbus/src/modbus-rtu.c \
    3rdparty/libmodbus/src/modbus-tcp.c \
    3rdparty/libmodbus/src/modbus-ascii.c \
    3rdparty/qextserialport/qextserialport.cpp \
        mainwindow.cpp \
    modbusslavethread.cpp \
    modbuspollthread.cpp

HEADERS += \
    3rdparty/libmodbus/src/modbus.h \
    3rdparty/qextserialport/qextserialport.h \
    3rdparty/qextserialport/qextserialenumerator.h \
        mainwindow.h \
    modbusslavethread.h \
    modbuspollthread.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += 3rdparty/libmodbus \
               3rdparty/libmodbus/src \
               3rdparty/qextserialport
unix {
    SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
           3rdparty/qextserialport/qextserialenumerator_unix.cpp
    DEFINES += _TTY_POSIX_
}

win32 {
    SOURCES += 3rdparty/qextserialport/win_qextserialport.cpp \
           3rdparty/qextserialport/qextserialenumerator_win.cpp
    DEFINES += _TTY_WIN_  WINVER=0x0501
    LIBS += -lsetupapi -lws2_32
}
