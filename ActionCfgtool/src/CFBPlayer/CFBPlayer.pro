#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T16:59:47
#
#-------------------------------------------------
include ($$(PWD)../plate.pri)

TEMPLATE = lib
QT      += core gui multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += precompile_header
DEFINES += CFBPLAYER_LIBRARY

INCLUDEPATH += $$(PWD)../common \
               $$(PWD)../CFBEnv \
               $${SRC}/cfbbase/utils

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBPlayer
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -lCFBEnv
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBPlayerd
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -lCFBEnvd
    }
}

SOURCES += cfbvideoplayer.cpp \
           videoplayercontrlwidget.cpp \
           cfbslider.cpp \
           cfbvideooutputitem.cpp \
           videocapture.cpp

HEADERS += cfbplayer_global.h \
           cfbvideoplayer.h \
           cfbplayerconst.h \
           videoplayercontrlwidget.h \
           cfbslider.h \
           cfbvideooutputitem.h \
           videocapture.h

FORMS += cfbvideoplayer.ui \
         videoplayercontrlwidget.ui

RESOURCES += res.qrc

TRANSLATIONS += $${SRC}/res/languages/CFBPlayer_zh_CN.ts \
                $${SRC}/res/languages/CFBPlayer_en.ts

MOC_DIR         = $${TEMP}/$$TARGET/moc
UI_DIR          = $${TEMP}/$$TARGET/ui
UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
RCC_DIR         = $${TEMP}/$$TARGET/rcc
