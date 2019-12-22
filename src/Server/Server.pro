# Check if the config file exists

! include( ../Common/Config.pri) {
    error( "Couldn't find the Config.pri file!" )
}

TARGET = server
TEMPLATE = app

DEPENDPATH += $$PWD/../Common/
INCLUDEPATH += $$PWD/../Common/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        server.cpp \
        socketlistmodel.cpp \
    bombwire.cpp \
    bombbody.cpp \
    bombtimer.cpp \
    bomb.cpp

HEADERS += \
        mainwindow.h \
        server.h \
        socketlistmodel.h \
    bombwire.h \
    bombbody.h \
    bombtimer.h \
    bomb.h

FORMS += \
        mainwindow.ui

! include( ../Common/Common.pri ) {
    error( "Couldn't find the common.pri file!" )
}
