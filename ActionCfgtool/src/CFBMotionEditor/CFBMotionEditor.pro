#-------------------------------------------------
#
# Project created by QtCreator 2015-12-03T13:43:23
#
#-------------------------------------------------
#include ($$(PWD)../plate.pri)

TEMPLATE = lib
QT      += core gui widgets multimedia xml
CONFIG  += precompile_header
PRECOMPILED_HEADER = stable.h
DEFINES += MOTIONEDITOR_LIB USEXML

INCLUDEPATH += $$(PWD)../common \
               $$(PWD)../CFBEnv \
               $$(PWD)../mp3proj \
               $$(PWD)../CFBSocketSession \
               $$(PWD)../CFBRobot3DView \
               $${TRD}/libmpg123/include \
               $${TRD}/libsndfile/include \
               $${SRC}/cfbbase/common \
               $${SRC}/cfbbase/utils \
               $${SRC}/cfbbase/cfbactiondata \
               $${SRC}/cfbbase/qutils

win32{
    CONFIG(release, debug|release){
        TARGET  = CFBMotionEditor
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutils \
                -lqutils \
                -lCFBEnv \
                -lcfbactiondata \
                -lCFBRobot3DView \
                -lmp3proj \
                -lCFBSocketSession \
                -L$${TRD}/libmpg123/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibmpg123 \
                -L$${TRD}/libsndfile/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibsndfile-1
    }
    CONFIG(debug, debug|release){
        TARGET   = CFBMotionEditord
        DESTDIR = $${BIN}/$${PLATE_NAME}/$${DIR_NAME}
        LIBS += -L$${DESTDIR} \
                -lutilsd \
                -lqutilsd \
                -lCFBEnvd \
                -lcfbactiondatad \
                -lCFBRobot3DViewd \
                -lmp3projd \
                -lCFBSocketSessiond \
                -L$${TRD}/libmpg123/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibmpg123 \
                -L$${TRD}/libsndfile/lib/$${PLATE_NAME}/$${DIR_NAME} \
                -llibsndfile-1
    }
}

SOURCES += CFBMotionEditorImpl.cpp \
           CFBMotionEditor.cpp \
           stable.cpp \
           OptionsMotionWidget.cpp \
           UIMainLayerLayout.cpp \
           CFBMotionEditorLayout.cpp \
           UILayerManager.cpp \
           UILayerOptions.cpp \
           UIDialogBlockDescription.cpp \
           dialogcommandbox.cpp \
           OptionsEarLedWidget.cpp \
           mp3player.cpp \
           OptionsMusicWidget.cpp \
           OptionEyeLedWidget.cpp \
           LayerUI/ActionBlockData.cpp \
           LayerUI/ActionBlockDataManager.cpp \
           LayerUI/ActionBlockGroupManager.cpp \
           LayerUI/MotionDataPlay.cpp \
           LayerUI/EyeLedDataPlay.cpp \
           LayerUI/EarLedDataPlay.cpp \
           LayerUI/UILayerCommonWidget.cpp \
           LayerUI/UILayerCommonContainer.cpp \
           LayerUI/LayerCommonImpl.cpp \
           LayerUI/UIEyeLedLayer.cpp \
           LayerUI/UIMusicLayer.cpp \
           LayerUI/UIMotionLayer.cpp \
           LayerUI/EyeLedLayer.cpp \
           LayerUI/MotionLayer.cpp \
           LayerUI/MusicLayer.cpp \
           LayerUI/TimeLayer.cpp \
           LayerUI/UIEarLedLayer.cpp \
           LayerUI/EarLedLayer.cpp \
           LayerUI/UILayerImpl.cpp \
           LayerUI/DialogAddLayer.cpp \
           LayerUI/LayerControlWidget.cpp \
           LayerUI/GroupDataPlayThread.cpp \
           LayerUI/GroupDataManagerPlay.cpp \
           LayerUI/MusicDataPlay.cpp \
           LayerUI/UpdateTimeLineProcess.cpp \
           LayerUI/ExpressionLayer.cpp \
           LayerUI/ExpressionDataPlay.cpp \
           LayerUI/UIExpressionLayer.cpp \
           BatchModifyDialog.cpp \
           timesettingdialog.cpp \
           ledswidget.cpp

HEADERS += stable.h \
           CFBMotionEditorImpl.h \
           CFBMotionEditor.h \
           OptionsMotionWidget.h \
           UIMainLayerLayout.h \
           LayerUI/ILayerManager.h \
           LayerUI/ActionBlockData.h \
           LayerUI/ActionBlockDataManager.h \
           LayerUI/ActionBlockGroupManager.h \
           LayerUI/ILayerOptionHandler.h \
           LayerUI/MotionDataPlay.h \
           CFBMotionEditorLayout.h \
           LayerUI/ILayerUI.h \
           UILayerManager.h \
           LayerUI/ILayerOption.h \
           UILayerOptions.h \
           UIDialogBlockDescription.h \
           dialogcommandbox.h \
           LayerUI/IActionLayerManager.h \
           mp3projconstants.h \
           mp3player.h \
           OptionsMusicWidget.h \
           OptionEyeLedWidget.h \
           LayerUI/IUILayerManager.h \
           LayerUI/EyeLedDataPlay.h \
           LayerUI/EarLedDataPlay.h \
           ledswidget.h \
           LayerUI/UILayerCommonWidget.h \
           LayerUI/UILayerCommonContainer.h \
           LayerUI/ILayerCommonWidget.h \
           LayerUI/LayerCommonImpl.h \
           LayerUI/UIEarLedLayer.h \
           LayerUI/UIMotionLayer.h \
           LayerUI/UIMusicLayer.h \
           LayerUI/UIEyeLedLayer.h \
           LayerUI/EarLedLayer.h \
           LayerUI/EyeLedLayer.h \
           LayerUI/MotionLayer.h \
           LayerUI/MusicLayer.h \
           LayerUI/TimeLayer.h \
           OptionsEarLedWidget.h \
           timesettingdialog.h \
           LayerUI/UILayerImpl.h \
           LayerUI/DialogAddLayer.h \
           LayerUI/LayerControlWidget.h \
           LayerUI/GroupDataPlayThread.h \
           LayerUI/GroupDataManagerPlay.h \
           LayerUI/IGroupDataManagerPlay.h \
           LayerUI/MusicDataPlay.h \
           LayerUI/UpdateTimeLineProcess.h \
           BatchModifyDialog.h \
           LayerUI/ExpressionLayer.h \
           LayerUI/ExpressionDataPlay.h\
           LayerUI/UIExpressionLayer.h

FORMS += UIOptionMotionWidget.ui \
         UIMainLayerLayout.ui \
         CFBMotionEditorLayout.ui \
         UILayerManager.ui \
         UILayerOptions.ui \
         UIDialogBlockDescription.ui \
         dialogcommandbox.ui \
         OptionsEarLedWidget.ui \
         OptionsMusicWidget.ui \
         OptionEyeLedWidget.ui \
         ledswidget.ui \
         UILayerCommonContainer.ui \
         UILayerCommonWidget.ui \
         timesettingdialog.ui \
         DialogAddLayer.ui \
         LayerControlWidget.ui \
         BatchModifyDialog.ui

RESOURCES += res.qrc

TRANSLATIONS += $${SRC}/res/languages/CFBMotionEditor_zh_CN.ts \
                $${SRC}/res/languages/CFBMotionEditor_en.ts

#QMAKE_POST_LINK += \
#    rcc -binary $$PWD/res.qrc -o $$PWD/../res/rcc/CFBMotionEditor.rccs

#MOC_DIR         = $${TEMP}/$$TARGET/moc
#UI_DIR          = $${TEMP}/$$TARGET/ui
#UI_HEADERS_DIR  = $${TEMP}/$$TARGET/ui
#UI_SOURCES_DIR  = $${TEMP}/$$TARGET/ui
#OBJECTS_DIR     = $${TEMP}/$$TARGET/obj
#RCC_DIR         = $${TEMP}/$$TARGET/rcc
