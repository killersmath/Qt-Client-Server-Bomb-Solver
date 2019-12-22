QT          += core gui network widgets

CONFIG      += c++11

#Target file directory
DESTDIR     = bin
#Intermediate file
OBJECTS_DIR = generated_files #Intermediate object files directory
MOC_DIR     = generated_files #Intermediate moc files directory
UI_DIR      = generated_files
RCC_DIR     = generated_files
MAKEFILE    = generated_files

win32 {
    TARGET_CUSTOM_EXT = .exe
    DEPLOY_COMMAND = windeployqt
}

macx {
    TARGET_CUSTOM_EXT = .app
    DEPLOY_COMMAND = macdeployqt
}

linux {
    DEPLOY_COMMAND = linuxdeployqt
}

DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/bin/$${TARGET}$${TARGET_CUSTOM_EXT}))

DEPLOY_FLAG = --no-translations --compiler-runtime

QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET} $${DEPLOY_FLAG}
