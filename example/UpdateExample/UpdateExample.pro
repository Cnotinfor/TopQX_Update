#-------------------------------------------------
#
# Project created by QtCreator 2011-07-28T17:52:16
#
#-------------------------------------------------

QT       += core gui network

TARGET = UpdateExample
TEMPLATE = app

CONFIG += link_prl

INCLUDEPATH += ../../include # path for the UpdateManager Lib

LIBS += -L../../lib/shared # path for the UpdateManager lib

CONFIG( debug, debug|release ) {
	TARGET = $${TARGET}_d
	BUILD_NAME = debug
	LIBS += -lUpdateManager_d
}
CONFIG( release, debug|release ) {
	BUILD_NAME = release
	LIBS += -lUpdateManager
}

SOURCES += main.cpp\
		UpdateExample.cpp

HEADERS  += UpdateExample.h

FORMS    += UpdateExample.ui

