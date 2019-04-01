# Check if the config file exists

! include( ../Common/Config.pri) {
    error( "Couldn't find the common.pri file!" )
}

TARGET = server
TEMPLATE = app

DEPENDPATH += $$PWD/../Common/
INCLUDEPATH += $$PWD/../Common/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        bombitem.cpp \
        server.cpp \
        socketlistmodel.cpp

HEADERS += \
        mainwindow.h \
        bombitem.h \
        server.h \
        socketlistmodel.h

FORMS += \
        mainwindow.ui

! include( ../Common/Common.pri ) {
    error( "Couldn't find the common.pri file!" )
}
