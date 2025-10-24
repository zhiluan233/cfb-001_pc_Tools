#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T14:30:55
#
#-------------------------------------------------
#include ($$(PWD)../plate.pri)

TEMPLATE  = lib
QT       -= gui
QT       += network xml
CONFIG   += c++11
DEFINES  += CFBSOCKETSESSION_LIBRARY

INCLUDEPATH += $$(PWD)../common \
               $$(PWD)../CFBEnv \
               $$(PWD)../Protocol \
               $$(PWD)../Protocol/src \
               $${TRD}/protolbuf/include \
               $${SRC}/cfbbase/common \
               $${SRC}/cfbbase/utils

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBSocketSession
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -lCFBEnv \
                -lProtocol \
                -L$${TRD}/protolbuf/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibprotobuf
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBSocketSessiond
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -lCFBEnvd \
                -lProtocold \
                -L$${TRD}/protolbuf/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibprotobufd
    }
}

SOURCES += cfbsocketsession.cpp \
           stdafx.cpp \
           CFBSocketSessionImpl.cpp \
           CFBBroadcastSocketImpl.cpp \
           CFBSendFileSocketImpl.cpp \
           CFBUpLoadSocketImpl.cpp \
           Header.cpp

HEADERS +=  cfbsocketsession.h\
            cfbsocketsession_global.h \
            stdafx.h \
            CFBSocketSessionImpl.h \
            CFBBroadcastSocket.h \
            CFBBroadcastSocketImpl.h \
            CFBSendFileSocketImpl.h \
            CFBUpLoadSocketImpl.h \
            ICFBTransFileSession.h \
            ICFBSocketSession.h \
            Header.h

#MOC_DIR         = $${TEMP}/$$TARGET/moc
#UI_DIR          = $${TEMP}/$$TARGET/ui
#UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
#UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
#OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
#RCC_DIR         = $${TEMP}/$$TARGET/rcc
