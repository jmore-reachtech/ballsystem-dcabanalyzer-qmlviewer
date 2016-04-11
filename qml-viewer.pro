TEMPLATE = app

QT += qml quick network
CONFIG += c++11

VERSION = 1.0.1
TARGET = qml-viewer

# add #define for the version
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += main.cpp \
    maincontroller.cpp \
    mainview.cpp \
    stringserver.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    maincontroller.h \
    mainview.h \
    stringserver.h \
    systemdefs.h

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11

OTHER_FILES += \
    settings.conf
