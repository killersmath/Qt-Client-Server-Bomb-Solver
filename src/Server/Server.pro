# Check if the config file exists
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TARGET = server
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        bombitem.cpp \
        socketthread.cpp \
        server.cpp \
        socketlistmodel.cpp

HEADERS += \
        mainwindow.h \
        bombitem.h \
        socketthread.h \
        server.h \
        socketlistmodel.h

FORMS += \
        mainwindow.ui
