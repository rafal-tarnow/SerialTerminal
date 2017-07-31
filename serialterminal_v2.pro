#-------------------------------------------------
#
# Project created by QtCreator 2015-10-09T09:12:14
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialterminal
TEMPLATE = app

target.path=/home/zsmu/$${TARGET}/bin
INSTALLS += target

CONFIG += c++11

SOURCES += main.cpp\
        widget.cpp \
    settingsdialog.cpp \
    newframe.cpp \
    deleteframedialog.cpp \
    newlinepolicydialog.cpp \
    connection.cpp

HEADERS  += widget.h \
    settingsdialog.hpp \
    newframe.h \
    deleteframedialog.h \
    newlinepolicydialog.h \
    connection.hpp

FORMS    += widget.ui \
    settingsdialog.ui \
    newframe.ui \
    deleteframedialog.ui \
    newlinepolicydialog.ui
