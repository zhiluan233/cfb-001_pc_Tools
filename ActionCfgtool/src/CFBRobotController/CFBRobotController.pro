#-------------------------------------------------
#
# Project created by QtCreator 2015-12-24T11:02:10
#
#-------------------------------------------------
#include ($$(PWD)../plate.pri)

TEMPLATE  = lib
QT       += core gui xml
CONFIG += precompile_header
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += CFBROBOTCONTROLLER_EXPORTS

INCLUDEPATH += $${SRC}/CFBRobotController/Robot \
               $${SRC}/cfbbase/qutils \
               $$(PWD)../Common \
               $${SRC}/cfbbase/common\
               $${TRD}/libusb/include
PRECOMPILED_HEADER += $$PWD/stdafx.h

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBRobotController
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -L$${TRD}/msvcp/$${PLATE_NAME}/$${DIR_NAME} \
                -luser32 \
                -lsetupapi \
                -L$${TRD}/libusb/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibusb-1.0
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBRobotControllerd
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -L$${TRD}/msvcp/$${PLATE_NAME}/$${DIR_NAME} \
                -luser32 \
                -lsetupapi \
                -L$${TRD}/libusb/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibusb-1.0
    }
}

SOURCES += Robot/RobotController.cpp \
           stdafx.cpp \
           CFBRobotController.cpp \
           Robot/hid.cpp

HEADERS += Robot/RobotController.h \
           stdafx.h \
           CFBRobotController.h \
           Robot/hidapi.h \
           cfbrobotcontroller_global.h

#MOC_DIR         = $${TEMP}/$$TARGET/moc
#UI_DIR          = $${TEMP}/$$TARGET/ui
#UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
#UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
#OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
#RCC_DIR         = $${TEMP}/$$TARGET/rcc
