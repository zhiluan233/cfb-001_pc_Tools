//#include "stable.h"
#include "MotionDataPlay.h"
#include "UILayerManager.h"
//#include <QElapsedTimer>
//#include "CFBPublic.h"

//CMotionUpdateProcess::CMotionUpdateProcess(IUILayerManager* pLayerManager, int nGroupStartTime)
//{
//    QObject::connect(this, &CMotionUpdateProcess::SigMotionPositionChanged, (CUILayerManager*)pLayerManager->GetWidget(), &CUILayerManager::slotSelectLineChanged);
//    m_nGroupTimeCount = nGroupStartTime;
////    m_nGroupStartTime = nGroupStartTime;
//}

//CMotionUpdateProcess::~CMotionUpdateProcess(void)
//{
//}

//void CMotionUpdateProcess::OnUpdateTimeLine(quint64 nPosition)
//{
//    m_nGroupTimeCount += nPosition;
//    emit SigMotionPositionChanged(m_nGroupTimeCount);
//}

CMotionDataPlay::CMotionDataPlay(IUILayerManager* pLayerManager, int nGroupStartTime)
{
//    m_pUpdateProcess = new CUpdateTimeLineProcess(pLayerManager, nGroupStartTime);
}

CMotionDataPlay::~CMotionDataPlay(void)
{
//    SAFE_DELETE(m_pUpdateProcess);
}


void CMotionDataPlay::ProcessPlayActionBlock(CActionBlockData* pActionBlock, int nRunTime, int TimeScale, bool bEnd)
{
    char* pData = NULL;
    int nLen = 0;
    pActionBlock->GetExtBlockData(&pData, nLen);
    m_pRobot3dView->SetMotorDatas(pData, nLen, nRunTime);
    m_pRobot3dView->SetMotorDatasSmoothly(pData, nLen, nRunTime); //仿真动作改为平滑执行
    delete []pData;

//    int nTotalTime = pActionBlock->GetRunTime() + pActionBlock->GetStopTime();
//    m_pUpdateProcess->OnUpdateTimeLine(nTotalTime);
}

void CMotionDataPlay::OnStopPlay()
{

}

void CMotionDataPlay::StartPlay(int nEndTime, void* PlayHandle, int nBlockIndex, int nTimeOffset)
{
    m_pRobot3dView = (ICFBRobot3DView*)PlayHandle;
    SetProcessHander(this);
    CGroupDataPlayThread::StartPlay(nEndTime, m_pRobot3dView , nBlockIndex , nTimeOffset);
}

void CMotionDataPlay::StopPlay()
{
//    m_nGroupTimeCount = 0;
    CGroupDataPlayThread::StopPlay();
}
