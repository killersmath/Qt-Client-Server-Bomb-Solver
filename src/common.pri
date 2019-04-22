QT       += core gui network widgets

CONFIG += c++11

MOC_DIR     = build/moc
OBJECTS_DIR = build/obj
UI_DIR      = build/ui
RCC_DIR     = build/
MAKEFILE    = build/

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}
