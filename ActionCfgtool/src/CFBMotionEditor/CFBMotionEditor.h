
#ifndef _CFBMOTIONEDITOR_H_
#define _CFBMOTIONEDITOR_H_

#include "motioneditor_global.h"
#include <QWidget>
#include <QString>
#include <QPoint>
#include "stable.h"

class ICFBRobot3DView;
class CICFBSocketSession;
class ILibTreeNavigation;
//// 动作编辑接口类
//class ICFBMotionEditor_Event
//{
//    //是否允许通过MotorItemDlg对3D模型进行编辑（模型仿真的时候禁止编辑）
//    virtual void CFBMotionEditorEvent_Enable3DViewOperation(bool bEnable) = 0;
//};
class MOTIONEDITOR_EXPORT ICFBMotionEditor
{
public:
    virtual ~ICFBMotionEditor(){}
    virtual bool Connect(QWidget* pParent) = 0;
    virtual void Disconnect() = 0;
    virtual QWidget* GetWidget() = 0;

    // 读取数据
    virtual bool GetData(char** ppData, int &nLen) = 0;
    // 设置数据
    virtual bool SetData(char* pData, int nLen) = 0;
    // 转换旧数据
//    virtual void AdjustData() = 0;
    // 保存3D模型的接口
    virtual void CFBMotionEditor_Set3DRobotViewInterface(ICFBRobot3DView* pRobotView) = 0;
    //virtual void CFBMotionEditor_Set3DRobotViewInterface(ICFBRobot3DView* pRobotView) = 0;
    virtual void CFBMotionEditor_SetSocketSession(CICFBSocketSession* pSession) = 0;
//    virtual void CFBMotionEditor_UpdateScrollBar() = 0;
    virtual void CFBMotionEditor_SetMusicResource(QString strFilePath) = 0;
    virtual void CFBMotionEditor_StopDebug() = 0;
    virtual void CFBMotionEditor_ReleaseMusicSource() = 0;
    virtual void CFBMotionEditor_SetTreeView(ILibTreeNavigation* pView) = 0;
//    virtual void CFBMotionEditor_TreeToView(char *pData, int nLen, QPoint* nPoint, QString FileName) = 0;
    virtual bool CFBMotionEditor_IsProjectModity() = 0;
    virtual bool CFBMotionEditor_IsRobotDebuging() = 0;
    virtual void CFBMotionEditor_SetProjectModity(bool bState) = 0;

    virtual void switchActionLayer(emActionLayerType eActionType) = 0;

//    virtual bool CFBMotionEditor_IsMusicModity() = 0;
//    virtual void CFBMotionEditor_SetMusicModity(bool bState) = 0;
};

MOTIONEDITOR_EXPORT bool CreateCFBMotionEditor(ICFBMotionEditor** ppEditor);
#endif  //_CFBMOTIONEDITOR_H_
