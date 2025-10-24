


#ifndef CFBROBOT3DCONTROL_H
#define CFBROBOT3DCONTROL_H

#include <QWidget>
#include <QThread>
#include <QStyleOption>

#include "cfbrobot3dwidget.h"
#include "ICFBRobot3DView.h"
#include "cfbrobot3ddisplay.h"
#include "cfbrobot3dcontrolwidget.h"
#include "cfbrobotcontrolinterface.h"

namespace Ui {
class CFBRobot3DControl;
}

class CFBROBOT3DVIEWSHARED_EXPORT CFBRobot3DContainer : public QWidget, public ICFBRobot3DView, public ICFBRobotViewControlNotify
{
    Q_OBJECT

public:
    explicit CFBRobot3DContainer(QWidget *parent = 0, ICFBRobot3DViewEvent *pCFBRobotEvent = 0);
    ~CFBRobot3DContainer();

public:
    void UpdateNodeDate(const char* szNodeName, float angle, float offset, TypeAngleChanged nChangeType);
public:
    //ICFBRobotViewControlNotify接口实现
    virtual void Set3DViewRotationManual(int nDir, float fRot, const char* pNode, int nTime = 200);
    virtual void SetRotation(float x, float y, float z, float w, char* pNode);
    virtual void OnSetRotation(int nID, int nAngle, int iTime);
    virtual bool IsComConnectted();
    virtual bool IsRobotDebuging();
    virtual void LostPowerForRobot(int nID);
    virtual void OnReadRotation(int nID);
    virtual void ResetAllAngle();

    void setDefaultStyle();

    CFBRobot3DControlWidget *GetCtrlWidget();
	CFBRobot3DWidget* get3DWidget();
public slots:
    void OnSetRobotAngle(int nID, int nAngle);

public:
    //ICFBRobot3DView接口实现
    virtual void Connect(QWidget* parent, ICFBRobot3DViewEvent* pEvent);
    virtual void Disconnect();
    //virtual bool SetSerialPortControl(CCFBSerialPortSesstion *pConnect);
    //获取各舵机角度
    virtual bool GetMotorDatas(char** pData, int& nLen, emActionLayerType eActionType);
    //设置各舵机角度
    virtual void SetMotorDatas(char* pData, int nLen, int runTime);
    //设置各舵机角度
    virtual void SetMotorDatasManual(char* pData, int nLen, int runTime);
    virtual void SetMotorAngle(int nID, int nAngle);
    //设置3D模型的仿真状态
    virtual void Set3DModelPlayState(bool bState);
    virtual void SetMotorPowerState(int nMotorID, int nState);
    //设置所有舵机角度
    virtual void SetAllMotorAngle(char* pData, int nLen, int nRunTime, int nAllTime);
    //获取串口连接状态
    virtual bool GetComConnectState();
    //启用/禁用控制界面
    virtual void EnableViewCtrl(bool bEnable);
    //获取舵机数目
    virtual int GetMotorCount();
    //获取窗体
    virtual QWidget *GetWidget();

    virtual void SetMotorDatasSmoothly(char* pData, int nLen, int iTime);

signals:
    void SigEnable(bool bEnable);
    void angleChanged(int nID, int value, int nTime, TypeAngleChanged nType);
protected:
    void showEvent(QShowEvent * event);
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);

protected slots:
    void OnFirstFrameFinished();

private:
    Ui::CFBRobot3DControl *ui;
    CFBRobot3DWidget*           m_pCFBRobot3DWidget; //3DViewWidget用于显示3D Robot
    CFBRobot3DControlWidget*    m_pControlWidget; //3DControl提供控制界面
    ICFBRobot3DViewEvent*       m_pEvent;
    //CCFBSerialPortSesstion* m_pConnectPort; //串口接口指针
};

#endif // CFBROBOT3DCONTROL_H
