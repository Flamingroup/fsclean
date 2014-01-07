#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T14:56:01
#
#-------------------------------------------------

QT       += core \
            sql \
	    widgets

TARGET = Scan
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ./parcours.cpp \
    fichier.cpp \
    sql.cpp \
    mainwindow.cpp \
    about.cpp

HEADERS += \
    ./parcours.h \
    fichier.h \
    sql.h \
    mainwindow.h \
    about.h

LIBS+=  /usr/lib/x86_64-linux-gnu/libboost_system.so \
        /usr/lib/x86_64-linux-gnu/libboost_filesystem.so \
	-ltomcrypt
#/usr/lib/x86_64-linux-gnu/libboost_system.so \	

OTHER_FILES += \
    config.cfg

QMAKE_CXXFLAGS += -std=c++11

FORMS += \
    interface.ui \
    about.ui


