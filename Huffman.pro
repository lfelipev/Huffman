#-------------------------------------------------
#
# Project created by QtCreator 2014-06-14T12:40:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Huffman
TEMPLATE = app


SOURCES += main.cpp\
    select.cpp \
    codificar.cpp \
    decodificar.cpp

HEADERS  += \
    codificar.h \
    decodificar.h \
    select.h

FORMS    += \
    select.ui \
    codificar.ui \
    decodificar.ui

RESOURCES +=

OTHER_FILES += \
    AppIcon.rc \
    appIcon.ico
