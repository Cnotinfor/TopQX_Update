# ###################################################################
#  Project common settings
# ###################################################################
TARGET = UpdateManager
TEMPLATE = lib
QT += network

DEFINES += UPDATEMANAGER_LIB

CONFIG += link_prl

macx {
	INCLUDEPATH += \
		/Work/2010/CnotiSettings/include
	LIBS += \
		-L/Work/2010/CnotiSettings/lib/static
}
win32 {
	message($(CNOTI_SETTINGS_HOME))

	INCLUDEPATH += \
		$(CNOTI_SETTINGS_HOME)/include

	LIBS += \
		-L$(CNOTI_SETTINGS_HOME)/lib/static
}


CONFIG( debug, debug|release ) {
	TARGET = $${TARGET}_d
	BUILD_NAME = debug
	LIBS += \
		-lCnotiSettings_d
}
CONFIG( release, debug|release ) {
	BUILD_NAME = release
	LIBS += \
		-lCnotiSettings
}
