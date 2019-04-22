QT       += core gui network widgets

CONFIG += c++11

DESTDIR      = bin #Target file directory
OBJECTS_DIR  = generated_files #Intermediate object files directory
MOC_DIR      = generated_files #Intermediate moc files directory
UI_DIR      = generated_files
RCC_DIR     = generated_files
MAKEFILE    = generated_files

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/socketthread.cpp

HEADERS += \
    $$PWD/socketthread.h

#CONFIG(debug, debug|release) {
#    DESTDIR = debug
#}# else {
#    DESTDIR = release
#}

#MOC_DIR     = build/moc
#OBJECTS_DIR = build/obj
#UI_DIR      = build/ui
#RCC_DIR     = build/
#MAKEFILE    = build/
