#ifndef _MOTIONDATAPLAY_H_
#define _MOTIONDATAPLAY_H_

#include "motioneditor_global.h"
#include "IGroupDataPlay.h"
#include "ActionBlockData.h"
#include "LayerUI/ILayerUI.h"
#include "ActionBlockDataManager.h"
#include "GroupDataPlayThread.h"
#include "ActionBlockData.h"
#include "ICFBRobot3DView.h"
#include "UpdateTimeLineProcess.h"
//#include <QThread>
//#include <QMutex>
//#include <QWaitCondition>

class MOTIONEDITOR_EXPORT CMotionDataPlay: public CGroupDataPlayThread, public CActionBlockProcess
{
public:
    CMotionDataPlay(IUILayerManager* pLayerManager, int nGroupStartTime);
    ~CMotionDataPlay(void);

private:
//    CUpdateTimeLineProcess* m_pUpdateProcess;    //处理仿真时刷新当前时间线位置
    ICFBRobot3DView* m_pRobot3dView;

public:
    //CGroupDataPlayThread 接口重写
    virtual void StartPlay(int nEndTime, void* PlayHandle, int nBlockIndex = 0, int nTimeOffset = 1);
    // CActionBlockProcess 接口实现
    
    virtual void ProcessPlayActionBlock(CActionBlockData* pActionBlock, int nRunTime, int TimeScale, bool bEnd = false);
    virtual void OnStopPlay();
    virtual void StopPlay();
};
#endif  //_MOTIONDATAPLAY_H_
