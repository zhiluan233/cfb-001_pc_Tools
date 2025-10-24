#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T18:02:14
#
#-------------------------------------------------
#include ($$(PWD)../plate.pri)

TEMPLATE = lib
QT      += core gui widgets xml
CONFIG += precompile_header
DEFINES += CFBEVENTEDITOR_LIBRARY
PRECOMPILED_HEADER += $$PWD/stdafx.h

INCLUDEPATH += $$(PWD)../common \
               $$(PWD)../LibFlowControl \
               $$(PWD)../CFBEnv \
               $${SRC}/cfbbase/utils \
               $${SRC}/cfbbase/qutils

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBEventEditor
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -lqutils \
                -lCFBEnv
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBEventEditord
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -lqutilsd \
                -lCFBEnvd
    }
}

SOURCES += stdafx.cpp \
           cfbeventeditorwidget.cpp

HEADERS += cfbeventeditor.h\
           cfbeventeditor_global.h \
           stdafx.h \
           cfbeventeditorwidget.h \

FORMS += cfbeventeditorwidget.ui

RESOURCES += cfbeventeditor.qrc

TRANSLATIONS += $${SRC}/res/languages/CFBEventEditor_zh_CN.ts \
                $${SRC}/res/languages/CFBEventEditor_en.ts

#MOC_DIR         = $${TEMP}/$$TARGET/moc
#UI_DIR          = $${TEMP}/$$TARGET/ui
#UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
#UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
#OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
#RCC_DIR         = $${TEMP}/$$TARGET/rcc

