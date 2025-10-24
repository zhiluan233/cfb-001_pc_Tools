#include "stable.h"
#include "CFBMotionEditorLayout.h"
#include "ui_CFBMotionEditorLayout.h"
#include "LayerUI/ILayerUI.h"
#include "LayerUI/IUILayerManager.h"
#include <QFile>
#include <QPainter>
#include <QDir>
#include <QResource>
#include "configs.h"

bool CreateIActionLayerManager(IActionLayerManager** ppManager)
{
    CCFBMotionEditorLayout* pProxy = new CCFBMotionEditorLayout();
    if(pProxy)
    {
        *ppManager = pProxy;
        return true;
    }
    return false;
}


CCFBMotionEditorLayout::CCFBMotionEditorLayout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CCFBMotionEditorLayout),
    m_pMainLayer(NULL)
{
    ui->setupUi(this);

    QString rccPath = CConfigs::getAppSubDirPath(SUBDIR_RES) + QDir::separator() + "CFBMotionEditor.rcc";
    QResource::registerResource(rccPath);
    setDefaultStyle();
}

CCFBMotionEditorLayout::~CCFBMotionEditorLayout()
{
    delete ui;
}

bool CCFBMotionEditorLayout::Connect(QWidget* pParent)
{
    if(CreateLayerOption(&m_pLayerOption))
    {
        m_pLayerOption->Connect(this);
    }
    m_pLayerOption->Connect(this);
    if(CreateLayerManager(&m_pMainLayer))
    {

        m_pMainLayer->Connect(this,this);
    }
    return true;
}
void CCFBMotionEditorLayout::ILayerManager_EnablePlay(int nLayerType ,bool bState)
{
    //    if(m_pLayerControl)
    //    {
    //        m_pLayerControl->LayerControlToolsEvent_EnablePlay(nLayerType , bState);
    //    }
}

void CCFBMotionEditorLayout::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CCFBMotionEditorLayout::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/motiondefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}
// void CCFBMotionEditorLayout::EnableControlPlay(int nLayerType , bool bState)
// {
// 	if (m_pLayerControl)
// 	{
// 	}
// }
void CCFBMotionEditorLayout::ReleaseConnection()
{


    if (m_pMainLayer)
    {
        m_pMainLayer->ReleaseConnection();
        m_pMainLayer = NULL;
    }

    if (m_pLayerOption)
    {
        m_pLayerOption->ReleaseConnection();
        m_pLayerOption = NULL;
    }

    disconnect();
    delete this;
}

bool CCFBMotionEditorLayout::GetData(char** ppData, int &nLen)
{
    if (m_pMainLayer)
    {
        return m_pMainLayer->GetData(ppData, nLen);
    }

    return false;
}
//void CCFBMotionEditorLayout::AdjustData()
//{
//    if (m_pMainLayer)
//    {
//        //return m_pMainLayer->SetData(pData, nLen);
//        m_pMainLayer->AdjustData();
//    }

//}
bool CCFBMotionEditorLayout::SetData(char* pData, int nLen)
{
    //    if (m_pLayerControl)
    //    {
    //        m_pLayerControl->LayerControl_DeleteAllControl();
    //    }

    if (m_pLayerOption)
    {
        m_pLayerOption->ResetState();
    }

    if (m_pMainLayer)
    {
        return m_pMainLayer->SetData(pData, nLen);
    }

    return true;
}

//void CCFBMotionEditorLayout::ILayerControlEvent_GetUnUseLayers(int **pLayerID, int& nLayerCount)
//{
//    if (m_pMainLayer)
//    {
//        return m_pMainLayer->LayerManager_GetUnUseLayers(pLayerID, nLayerCount);
//    }
//}

//QString CCFBMotionEditorLayout::ILayerControlEvent_GetLayerName(int nLayerID)
//{
//    if (m_pMainLayer)
//    {
//        return m_pMainLayer->LayerManager_GetLayerName(nLayerID);
//    }

//    return "";
//}
//void CCFBMotionEditorLayout::LayerControlEvent_AddMusic()
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_AddMusic();
//    }
//}
//void CCFBMotionEditorLayout::ILayerControlEvent_AddLayer(int nLayerID)
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_AddLayer(nLayerID);
//    }
//}

//void CCFBMotionEditorLayout::ILayerControlEvent_AddLayerControl(int nLayerType)
//{
////        if (m_pLayerControl)
////        {
////            m_pLayerControl->LayerControlToolsEvent_AddLayerControl(nLayerType);
////        }
//}

//void CCFBMotionEditorLayout::LayerControlEvent_DeleteLayer(int nLayerID)
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_DeleteLayer(nLayerID);
//    }
//}

//void CCFBMotionEditorLayout::LayerControlEvent_SetTimeScaleValue(int timeScale)
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_SetTimeScale(timeScale);
//    }
//}

//int CCFBMotionEditorLayout::LayerControlEvent_GetTimeScaleValue()
//{
//    if (m_pMainLayer)
//    {
//        return m_pMainLayer->LayerManager_GetTimeScaleValue();
//    }

//    return -1;
//}
void CCFBMotionEditorLayout::ILayerManager_SetMusicLayerSourceHandler(ILayerOptionHandler* pHandler , int nLayerType , void *pData , int nDataLen , int nDataID)
{
    if (m_pLayerOption)
    {
        m_pLayerOption->SetOptionHandler(pHandler);
    }
}
void CCFBMotionEditorLayout::ILayerManager_DeleteMusciFile(ILayerOptionHandler* pHandler)
{
    //    if (m_pLayerOption)
    //    {
    //        m_pLayerOption->DeleteMusicResource(pHandler);
    //    }
}
void CCFBMotionEditorLayout::ActionLayerManager_SetMusicResource(QString strFilePath)
{
    // 	if(m_pLayerOption->GetOptionHandler())
    // 	{
    // 		QString str;
    // 		str.Format(_T("%s"),szFilePath);
    // 		ILayerOptionHandler *pOptionHandler= m_pLayerOption->GetOptionHandler();
    // 		//pOptionHandler->SetMusicResource(str);
    // 	}
    if (m_pMainLayer)
    {
        //        QString str;
        //        str.sprintf("%s",szFilePath);
        //        str.Format(_T("%s"),szFilePath);
        // 		ILayerOptionHandler *pOptionHandler= m_pLayerOption->GetOptionHandler();
        // 		pOptionHandler->SetMusicResource(str);
        m_pMainLayer->LayerManager_SetMusicResource(strFilePath);
    }
}
void CCFBMotionEditorLayout::ILayerManager_SetLayOptionHandler(ILayerOptionHandler* pHandler)
{
    if (m_pLayerOption)
    {
        m_pLayerOption->SetOptionHandler(pHandler);
    }
}

void CCFBMotionEditorLayout::ILayerManager_OnDeletOptionHandler(ILayerOptionHandler* pHandler)
{
    if (m_pLayerOption)
    {
        m_pLayerOption->OnDeletOptionHandler(pHandler);
    }
}

void CCFBMotionEditorLayout::ILayerManager_SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID, int nOffset)
{
    if (m_pLayerOption)
    {
        m_pLayerOption->SetActionBlockDataManager(pManager, nCurrentGroupID,nCurrentActionID, nOffset);
    }
}

void CCFBMotionEditorLayout::ActionLayerManager_Set3DRobotViewInterface(ICFBRobot3DView* pRobotView)
{
    if (m_pMainLayer)
    {
        m_pMainLayer->LayerManager_Set3DRobotViewInterface(pRobotView);
    }
}

void CCFBMotionEditorLayout::ActionLayerManager_SetTreeView(ILibTreeNavigation* pView)
{
    if (m_pMainLayer)
    {
        m_pMainLayer->LayerManager_SetTreeView(pView);
    }
}

//void CCFBMotionEditorLayout::ActionLayerManager_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName)
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_TreeToView(pData, nLen, nPoint, FileName);
//    }
//}
void CCFBMotionEditorLayout::ActionLayerManager_SetFileModified(bool bState)
{
    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
    if (pLayer == NULL)
        return ;
        IUILayerManager* pLayerManager = pLayer->GetLayerManager();
        if(pLayerManager)
        {
            //pLayerManager->IUILayerManager_SetModifiedState(true);
            //bMofied = pLayerManager->IUILayerManager_GetModifiedState();
            pLayerManager->IUILayerManager_SetModifiedState(bState);
        }
}

//bool CCFBMotionEditorLayout::ActionLayerManager_IsMusicModified()
//{
//    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MUSIC);
//    if (pLayer == NULL)
//        return false;
//        IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//    bool bMofied =false;
//        if(pLayerManager)
//        {
//            bMofied = pLayerManager->IUILayerManager_GetMusicModifiedState();
//        }
//    return bMofied;
//}

//void CCFBMotionEditorLayout::ActionLayerManager_SetMusicModified(bool bState)
//{
//    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MUSIC);
//    if (pLayer == NULL)
//        return ;
//        IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//        if(pLayerManager)
//        {
//            pLayerManager->IUILayerManager_SetMusicModifiedState(bState);
//        }
//}
bool CCFBMotionEditorLayout::ActionLayerManager_IsRobotDebuging()
{
    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
    //    if (pLayer == NULL)
    //        return FALSE;
    //    IUILayerManager* pLayerManager = pLayer->GetLayerManager();
    bool bMofied =false;
    //    if(pLayerManager)
    //    {
    //        //pLayerManager->IUILayerManager_SetModifiedState(true);
    //        bMofied = pLayerManager->IUILayerManager_IsRobotDebuging();
    //    }
    return bMofied;
}
bool CCFBMotionEditorLayout::ActionLayerManager_IsFileModified()
{
    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(ACTION_LAYER_MOTION);
    if (pLayer == NULL)
        return false;
        IUILayerManager* pLayerManager = pLayer->GetLayerManager();
    bool bMofied =false;
        if(pLayerManager)
        {
            //pLayerManager->IUILayerManager_SetModifiedState(true);
            bMofied = pLayerManager->IUILayerManager_GetModifiedState();
        }
    return bMofied;
}

//void CCFBMotionEditorLayout::LayerControlEvent_LayerPlaying(int layerType)
//{
//    if (m_pMainLayer)
//    {
//        //        ILayerOptionHandler* optionHandle = m_pLayerOption->GetOptionHandler();
//        //        m_pMainLayer->LayerManager_LayerPlaying(layerType, optionHandle);
//    }
//}
//void CCFBMotionEditorLayout::LayerControlEvent_LayerStop()
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_LayerStop();
//    }
//}
//void CCFBMotionEditorLayout::LayerControlEvent_StopLayerDebug()
//{
//    if (m_pMainLayer)
//    {
//        m_pMainLayer->LayerManager_StopLayerDebug();
//    }
//}

void CCFBMotionEditorLayout::ILayerManager_LayerDebugStop(int layerType)
{
    //    if (m_pLayerControl && m_pLayerControl->GetWidget())
    //    {
    //        m_pLayerControl->LayerControl_LayerDebugStop(layerType);
    //    }
}

ILayerOptionHandler* CCFBMotionEditorLayout::ILayerManager_GetOptionHandler()
{
    //    return m_pLayerOption->GetOptionHandler();
    return NULL; //临时处理
}


void CCFBMotionEditorLayout::ActionLayerManager_SetSocketSession(CICFBSocketSession* pSession)
{
    m_pSocketSession = pSession;
    if (m_pMainLayer)
    {
        m_pMainLayer->LayerManager_SetSocketSession(pSession);
    }
}

//void CCFBMotionEditorLayout::LayerControlEvent_SetMusicSource(int nLayerType , void *pData , int nDataLen , int nDataID)
//{
//    if (m_pMainLayer == NULL)
//    {
//        return;
//    }
//    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(nLayerType);
//    if (pLayer == NULL)
//        return;

//    //    IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//    //    if (pLayerManager == NULL)
//    //        return;
//    //    pLayerManager->IUILayerManager_SetMusicFile(nLayerType,pData,nDataLen,nDataID);
//}
//void CCFBMotionEditorLayout::LayerControlEvent_SetCurrentOption(int layerType)
//{
//    if (m_pMainLayer == NULL)
//        return;

//    ILayerUI* pLayer = m_pMainLayer->LayerManager_GetLayer(layerType);
//    if (pLayer == NULL)
//        return;

//    //    IUILayerManager* pLayerManager = pLayer->GetLayerManager();
//    //    if (pLayerManager == NULL)
//    //        return;

//    //    pLayerManager->IUILayerManager_SwitchLayer(layerType);
//}

//void CCFBMotionEditorLayout::ActionLayerManager_UpdateScrollBar()
//{
////    if (m_pMainLayer)
////    {
////        m_pMainLayer->LayerManager_UpdateScrollBar();
////    }
//}
void CCFBMotionEditorLayout::ActionLayerManager_ReleaseMusicSource()
{
    if (m_pMainLayer)
    {
        //m_pMainLayer->LayerManager_StopLayerDebug();
        //m_pMainLayer->la
        m_pMainLayer->LayerManager_ReleaseMusic();
    }
}
void CCFBMotionEditorLayout::ActionLayerManager_StopDebug()
{
    if (m_pMainLayer)
    {
        m_pMainLayer->LayerManager_StopLayerDebug();
    }
}

void CCFBMotionEditorLayout::ActionLayerManager_SwitchActionLayer(emActionLayerType eActionType)
{
    if (m_pMainLayer)
    {
        m_pMainLayer->LayerManager_SwitchActionLayer(eActionType);
    }
}