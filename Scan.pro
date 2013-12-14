#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T14:56:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Scan
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ./parcours.cpp \
    fichier.cpp

HEADERS += \
    ./parcours.h \
    fichier.h

LIBS+=/usr/lib/libboost_system.so \
    /usr/lib/libboost_filesystem.so
