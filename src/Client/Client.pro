# Importing Config File

QT          += core gui network widgets

CONFIG      += c++11

#Target file directory
#DESTDIR     = bin
#Intermediate file
OBJECTS_DIR = generated_files #Intermediate object files directory
MOC_DIR     = generated_files #Intermediate moc files directory
UI_DIR      = generated_files
RCC_DIR     = generated_files
MAKEFILE    = generated_files


TARGET = Client
TEMPLATE = app

! include( ../Common/Config.pri) {
    error( "Couldn't find the Config.pri file!" )
}

DEFINES += CLIENT_SIDE # Client Socket Thread Side Compilation

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
