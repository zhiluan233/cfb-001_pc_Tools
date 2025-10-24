#ifndef _CFBMOTIONEDITORIMPL_H_
#define _CFBMOTIONEDITORIMPL_H_

#include "motioneditor_global.h"
#include "CFBMotionEditor.h"
#include "LayerUI/iActionLayerManager.h"


class MOTIONEDITOR_EXPORT CCFBMotionEditorImpl: public ICFBMotionEditor
{
public:
    CCFBMotionEditorImpl(void);
    ~CCFBMotionEditorImpl(void);

public:
    //ICFBMotionEditor 接口实现
    virtual bool Connect(QWidget* pParent);
    virtual void Disconnect();
    virtual QWidget* GetWidget();

    virtual bool GetData(char** ppData, int &nLen);
    virtual bool SetData(char* pData, int nLen);
//    virtual void AdjustData() ;
    virtual void CFBMotionEditor_Set3DRobotViewInterface(ICFBRobot3DView* pRobotView);
    virtual void CFBMotionEditor_SetSocketSession(CICFBSocketSession* pSession);
//    virtual void CFBMotionEditor_UpdateScrollBar();
    virtual void CFBMotionEditor_SetMusicResource(QString strFilePath) ;
    virtual void CFBMotionEditor_SetTreeView(ILibTreeNavigation* pView);
//    virtual void CFBMotionEditor_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName);
    virtual void CFBMotionEditor_StopDebug();
    virtual void CFBMotionEditor_ReleaseMusicSource() ;
    virtual bool CFBMotionEditor_IsProjectModity();
    virtual bool CFBMotionEditor_IsRobotDebuging() ;
    virtual void CFBMotionEditor_SetProjectModity(bool bState) ;

    virtual void switchActionLayer(emActionLayerType eActionType);

//    virtual bool CFBMotionEditor_IsMusicModity();
//    virtual void CFBMotionEditor_SetMusicModity(bool bState);
    //////////////////////////////////////////////////////////////////////////

private:
    //CActionLayerManager*		m_pActionLayerManager;
//    ILayerOptionHandler*			m_pUILayerManager;
    IActionLayerManager*        m_pUILayerManager;

};
#endif  //_CFBMOTIONEDITORIMPL_H_
