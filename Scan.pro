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
    fichier.cpp \
    sql.cpp

HEADERS += \
    ./parcours.h \
    fichier.h \
    sql.h

LIBS+=/usr/lib/x86_64-linux-gnu/libboost_system.so \
    /usr/lib/x86_64-linux-gnu/libboost_filesystem.so \
	-ltomcrypt
	


