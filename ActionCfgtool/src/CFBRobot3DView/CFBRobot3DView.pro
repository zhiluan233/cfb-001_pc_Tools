#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T17:47:04
#
#-------------------------------------------------
#include ($$(PWD)../plate.pri)

TEMPLATE = lib
QT      += widgets opengl xml
CONFIG  += precompile_header
PRECOMPILED_HEADER += $$PWD/stdafx.h
DEFINES += CFBROBOT3DVIEW_LIBRARY USEXML
DEFINES += FBXSDK_SHARED

INCLUDEPATH += $$(PWD)../common \
               $$(PWD)../CFBEnv \
               $$(PWD)../CFBRobotController \
               $$(PWD)../CFBRobotController/Robot \
               $$(PWD)../LibFBX \
               $${TRD}/fbxsdk/include \
               $${SRC}/cfbbase/common \
               $${SRC}/cfbbase/utils \
               $${SRC}/cfbbase/qutils

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBRobot3DView
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -lqutils \
                -lCFBEnv \
                -lCFBRobotController \
                -lLibFBX \
                -L$${TRD}/fbxsdk/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -lglut32 \
                -lglew32 \
                -llibfbxsdk
        PRE_TARGETDEPS += $${DESTDIR}/LibFBX.lib
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBRobot3DViewd
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME} 
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -lqutilsd \
                -lCFBEnvd \
                -lCFBRobotControllerd \
                -lLibFBXd \
                -L$${TRD}/fbxsdk/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -lglut32 \
                -lglew32 \
                -llibfbxsdk
        PRE_TARGETDEPS += $${DESTDIR}/LibFBXd.lib
    }
}

SOURCES += stdafx.cpp \
           ICFBRobot3DView.cpp \
           cfbrobot3dwidget.cpp \
           cfbrobot3ddisplay.cpp \
           cfbrobot3dcontainer.cpp \
           cfbrobot3dcontrolwidget.cpp

HEADERS += stdafx.h \
           cfbrobot3dview_global.h \
           ICFBRobot3DView.h \
           cfbrobot3ddisplay.h \
           cfbrobot3dwidget.h \
           cfbrobot3dcontainer.h \
           cfbrobot3dcontrolwidget.h \
           cfbrobotcontrolinterface.h \
           $$(PWD)../CFBRobotController/CFBRobotController.h

FORMS += cfbrobot3dcontrol.ui \
         cfbrobot3dcontrolwidget.ui

RESOURCES += res.qrc

TRANSLATIONS += $${SRC}/res/languages/CFBRobot3DView_zh_CN.ts \
                $${SRC}/res/languages/CFBRobot3DView_en.ts

#MOC_DIR         = $${TEMP}/$$TARGET/moc
#UI_DIR          = $${TEMP}/$$TARGET/ui
#UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
#UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
#OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
#RCC_DIR         = $${TEMP}/$$TARGET/rcc
