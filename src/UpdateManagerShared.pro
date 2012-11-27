# ###################################################################
#  Project common settings
# ###################################################################
include( "UpdateManagerSettings.pro" )

# ###################################################################
#  Project custom settings
# ###################################################################

CONFIG += dll

DEFINES += SHARED_LIB
DESTDIR = ../lib/shared

# ####################################################################
#  Source Files
# ####################################################################

include( "src.pri" )

