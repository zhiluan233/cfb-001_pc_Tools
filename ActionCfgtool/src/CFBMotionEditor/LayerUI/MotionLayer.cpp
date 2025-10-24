//#include "stable.h"
#include "MotionLayer.h"
#include "IUILayerManager.h"


CMotionLayer::CMotionLayer(void)
{

}


CMotionLayer::~CMotionLayer(void)
{
}


// 取得类型，具体见宏定义
emActionLayerType CMotionLayer::GetType()
{
    return ACTION_LAYER_MOTION;
}

QString CMotionLayer::GetLayerName()
{
    QString strName = QObject::tr("Motion");
    return strName;
}


void CMotionLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        delete m_pOptionHandler;
        m_pOptionHandler = NULL;
    }
    CreateMotionLayOptionHandler(&m_pOptionHandler);
    if (m_pOptionHandler)
    {
        m_pOptionHandler->Connect(m_ManagerHandle, this);
    }

    if (m_pOptionHandler)
    {
        m_pOptionHandler->Set3DRobotViewInterface(m_pLayerManager->Get3DRobotViewInterface());
    }
}


// 头部层
CHeadMotionLayer::CHeadMotionLayer(void)
{

}

CHeadMotionLayer::~CHeadMotionLayer(void)
{

}

emActionLayerType CHeadMotionLayer::GetType()
{
    return ACTION_LAYER_HEAD;
}

QString CHeadMotionLayer::GetLayerName()
{
    QString strName = QObject::tr("Head");
    return strName;
}

void CHeadMotionLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        delete m_pOptionHandler;
        m_pOptionHandler = NULL;
    }
    CreateMotionLayOptionHandler(&m_pOptionHandler , ACTION_LAYER_HEAD);
    if (m_pOptionHandler)
    {
        m_pOptionHandler->Connect(m_ManagerHandle, this);
    }

    if (m_pOptionHandler)
    {
        m_pOptionHandler->Set3DRobotViewInterface(m_pLayerManager->Get3DRobotViewInterface());
    }

    
}


// 手部层
CHandMotionLayer::CHandMotionLayer(void)
{

}

CHandMotionLayer::~CHandMotionLayer(void)
{

}

emActionLayerType CHandMotionLayer::GetType()
{
    return ACTION_LAYER_HAND;
}

QString CHandMotionLayer::GetLayerName()
{
    QString strName = QObject::tr("Hand");
    return strName;
}

void CHandMotionLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        delete m_pOptionHandler;
        m_pOptionHandler = NULL;
    }
    CreateMotionLayOptionHandler(&m_pOptionHandler , ACTION_LAYER_HAND);
    if (m_pOptionHandler)
    {
        m_pOptionHandler->Connect(m_ManagerHandle, this);
    }

    if (m_pOptionHandler)
    {
        m_pOptionHandler->Set3DRobotViewInterface(m_pLayerManager->Get3DRobotViewInterface());
    }
}


// 腿部层
CLegMotionLayer::CLegMotionLayer(void)
{

}

CLegMotionLayer::~CLegMotionLayer(void)
{

}

emActionLayerType CLegMotionLayer::GetType()
{
    return ACTION_LAYER_LEG;
}

QString CLegMotionLayer::GetLayerName()
{
    QString strName = QObject::tr("Leg");
    return strName;
}

void CLegMotionLayer::CreateOptionHandler()
{
    if (m_pOptionHandler)
    {
        m_pOptionHandler->ReleaseConnection();
        delete m_pOptionHandler;
        m_pOptionHandler = NULL;
    }
    CreateMotionLayOptionHandler(&m_pOptionHandler , ACTION_LAYER_LEG);
    if (m_pOptionHandler)
    {
        m_pOptionHandler->Connect(m_ManagerHandle, this);
    }

    if (m_pOptionHandler)
    {
        m_pOptionHandler->Set3DRobotViewInterface(m_pLayerManager->Get3DRobotViewInterface());
    }
}