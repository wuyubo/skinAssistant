#-------------------------------------------------
#
# Project created by QtCreator 2014-11-16T16:53:04
#
#-------------------------------------------------

QT       += core gui\
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zXml
TEMPLATE = app


SOURCES += main.cpp\
        xmlexample.cpp \
    zxml.cpp \
    setting.cpp \
    xlabel.cpp \
    dialogui.cpp \
    interface.cpp

HEADERS  += xmlexample.h \
    zxml.h \
    global.h \
    setting.h \
    xlabel.h \
    dialogui.h \
    interface.h

FORMS    += xmlexample.ui \
    dialogui.ui

DISTFILES += \
    skin.png
