# Importing Config File

! include( ../Common/Config.pri) {
    error( "Couldn't find the Config.pri file!" )
}

TARGET = client
TEMPLATE = app

DEFINES += CLIENT_SIDE \  # Client Socket Thread Side Compilation
            QABSTRACTSOCKET_DEBUG

DEPENDPATH += $$PWD/../Common/
INCLUDEPATH += $$PWD/../Common/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    client.cpp

HEADERS += \
        mainwindow.h \
    client.h

FORMS += \
        mainwindow.ui

# Import Common Sources

! include( ../Common/Common.pri ) {
    error( "Couldn't find the common.pri file!" )
}
