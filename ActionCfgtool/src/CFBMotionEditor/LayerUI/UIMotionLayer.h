


#ifndef ACTIONLAYER_H
#define ACTIONLAYER_H
//#include <QObject>
#include "motioneditor_global.h"
#include "ActionBlockDataManager.h"
#include "ILayerCommonWidget.h"
#include "UILayerImpl.h"
//class CUILayerCommonWidget;

class MOTIONEDITOR_EXPORT CUIMotionLayer : public CUILayerImpl
{
    Q_OBJECT
public:
    CUIMotionLayer(ILayerCommonWidget *mp3Widget);

    //获取层类型
    virtual emActionLayerType GetType();
};


// 头部层
class CUIHeadMotionLayer : public CUILayerImpl
{
    Q_OBJECT
public:
    CUIHeadMotionLayer(ILayerCommonWidget *mp3Widget);

    //获取层类型
    virtual emActionLayerType GetType();
};


// 手部层
class CUIHandMotionLayer : public CUILayerImpl
{
    Q_OBJECT
public:
    CUIHandMotionLayer(ILayerCommonWidget *mp3Widget);

    //获取层类型
    virtual emActionLayerType GetType();
};


// 腿部层
class CUILegMotionLayer : public CUILayerImpl
{
    Q_OBJECT
public:
    CUILegMotionLayer(ILayerCommonWidget *mp3Widget);

    //获取层类型
    virtual emActionLayerType GetType();
};

#endif // ACTIONLAYER_H
