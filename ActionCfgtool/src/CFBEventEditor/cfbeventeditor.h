#ifndef CFBEVENTEDITOR_H
#define CFBEVENTEDITOR_H

#include "cfbeventeditor_global.h"
#include <QWidget>
#include "libflowcontrol.h"

//事件编辑器的通知接口
class CFBEVENTEDITORSHARED_EXPORT ICFBEventEditorNotify
{
public:
    
    virtual void getEventFlowData(int nEventType, char** ppData, int& nLen) = 0;

    
    virtual void setEventFlowData(int nEventType, char* pData, int nLen) = 0;

    
    virtual void showEventPage(int nIndex) = 0;

    
    virtual void enableFlowChart(int nEventType) = 0;
};

class CFBEVENTEDITORSHARED_EXPORT ICFBEventEditor
{
public:
    virtual bool Connect(QWidget* pWnd, ICFBEventEditorNotify* pNotify) = 0;
    virtual void ReleaseConnection() = 0;
    virtual QWidget* GetWidget() = 0;
    virtual void GetData(char** ppData, int& nLen) = 0;
    virtual void SetData(char* pData, int nLen) = 0;
//	virtual void Set3DRobotView(ICFBRobot3dView* pView) = 0;
//	virtual void SetTreeView(ITreeNavigation* pView) = 0;
//	virtual void ClearFlowChart() = 0;
//	virtual ICFBFlowControl* GetEventFlowCtr() = 0;
//	virtual void SetSocketSession(CICFBSocketSession* pSession) = 0;
//    virtual void setFlowControl(ICFBFlowControl* pFlowControl) = 0;
};

bool CFBEVENTEDITORSHARED_EXPORT CreateEventEditor(ICFBEventEditor** ppEventEditor);

#endif
