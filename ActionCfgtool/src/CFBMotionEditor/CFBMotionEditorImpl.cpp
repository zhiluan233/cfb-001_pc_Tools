#include "stable.h"
#include "CFBMotionEditorImpl.h"

bool CreateCFBMotionEditor(ICFBMotionEditor** ppEditor)
{
    CCFBMotionEditorImpl* pProxy = new CCFBMotionEditorImpl();
    if (pProxy) {
        *ppEditor = pProxy;
        return true;
    }
    return false;
}

CCFBMotionEditorImpl::CCFBMotionEditorImpl(void)
    : m_pUILayerManager(NULL)
    //, m_pActionLayerManager(NULL)
{
}

CCFBMotionEditorImpl::~CCFBMotionEditorImpl()
{
}

bool CCFBMotionEditorImpl::Connect(QWidget* pParent)
{
//    CAutolockRes res;
    if (CreateIActionLayerManager(&m_pUILayerManager))
    {
        m_pUILayerManager->Connect(pParent);
        return true;
    }

    return false;
}

void CCFBMotionEditorImpl::Disconnect()
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ReleaseConnection();
        m_pUILayerManager = NULL;
    }

    delete this;
}

QWidget* CCFBMotionEditorImpl::GetWidget()
{
    if (m_pUILayerManager)
    {
        return m_pUILayerManager->GetWidget();
    }
    return NULL;
}

bool CCFBMotionEditorImpl::GetData(char** ppData, int &nLen)
{
    if (m_pUILayerManager)
    {
        return m_pUILayerManager->GetData(ppData, nLen);
    }
    return false;
}
//void CCFBMotionEditorImpl::AdjustData()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->AdjustData();
//    }
//}
bool CCFBMotionEditorImpl::SetData(char* pData, int nLen)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_StopDebug();
    }

    if (m_pUILayerManager)
    {
        m_pUILayerManager->SetData(pData, nLen);
    }
    return false;
}

void CCFBMotionEditorImpl::CFBMotionEditor_Set3DRobotViewInterface(ICFBRobot3DView* pRobotView)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_Set3DRobotViewInterface(pRobotView);
    }
}

void CCFBMotionEditorImpl::CFBMotionEditor_SetTreeView(ILibTreeNavigation* pView)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_SetTreeView(pView);
    }
}

//void CCFBMotionEditorImpl::CFBMotionEditor_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->ActionLayerManager_TreeToView(pData, nLen, nPoint, FileName);
//    }
//}

void CCFBMotionEditorImpl::CFBMotionEditor_SetSocketSession(CICFBSocketSession* pSession)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_SetSocketSession(pSession);
    }
}

//void CCFBMotionEditorImpl::CFBMotionEditor_UpdateScrollBar()
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->ActionLayerManager_UpdateScrollBar();
//    }
//}
void CCFBMotionEditorImpl::CFBMotionEditor_SetMusicResource(QString strFilePath)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_SetMusicResource(strFilePath);
    }
}
void CCFBMotionEditorImpl::CFBMotionEditor_ReleaseMusicSource()
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_ReleaseMusicSource();
    }
}
void CCFBMotionEditorImpl::CFBMotionEditor_StopDebug()
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_StopDebug();
    }
}
void CCFBMotionEditorImpl::CFBMotionEditor_SetProjectModity(bool bState)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_SetFileModified(bState);
    }
}

//bool CCFBMotionEditorImpl::CFBMotionEditor_IsMusicModity()
//{
//    if (m_pUILayerManager)
//    {
//        return m_pUILayerManager->ActionLayerManager_IsMusicModified();
//    }
//    else
//        return false;
//}

//void CCFBMotionEditorImpl::CFBMotionEditor_SetMusicModity(bool bState)
//{
//    if (m_pUILayerManager)
//    {
//        m_pUILayerManager->ActionLayerManager_SetMusicModified(bState);
//    }
//}
bool CCFBMotionEditorImpl::CFBMotionEditor_IsRobotDebuging()
{
    if (m_pUILayerManager)
    {
        return m_pUILayerManager->ActionLayerManager_IsRobotDebuging();
    }
    else
        return false;
}
bool CCFBMotionEditorImpl::CFBMotionEditor_IsProjectModity()
{
// 	if (m_pUILayerManager)
// 	{
// 		char* pData = NULL;
// 		int nLens = 0;
// 		m_pUILayerManager->GetData(&pData, nLens);
// 		delete [] pData;
//
// 		if (nLens > 200)
// 			return true;
// 	}
//
// 	return false;
    if (m_pUILayerManager)
    {
        return m_pUILayerManager->ActionLayerManager_IsFileModified();
    }
    else
        return false;
}


void CCFBMotionEditorImpl::switchActionLayer(emActionLayerType eActionType)
{
    if (m_pUILayerManager)
    {
        m_pUILayerManager->ActionLayerManager_SwitchActionLayer(eActionType);
    }
}
