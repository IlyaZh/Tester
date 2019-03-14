#-------------------------------------------------
#
# Project created by QtCreator 2017-03-23T20:15:38
#
#-------------------------------------------------

QT       += core gui sql printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =ESTester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    itemdialog.cpp \
    settingsdialog.cpp \
    csvexport.cpp \
    printer.cpp \
    downloader.cpp

HEADERS  += mainwindow.h \
    globals.h \
    itemdialog.h \
    settingsdialog.h \
    csvexport.h \
    printer.h \
    downloader.h

FORMS    += mainwindow.ui \
    newitemdialog.ui \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
