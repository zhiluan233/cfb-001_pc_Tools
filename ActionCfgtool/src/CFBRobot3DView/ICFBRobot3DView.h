


#ifndef ICFBROBOT3DVIEW_H
#define ICFBROBOT3DVIEW_H

#include "cfbrobot3dview_global.h"

class QWidget;

#define POWEROFF    0   //未连接
#define POWERLOST   1   //掉电
#define POWERON     2   //得电

class CFBROBOT3DVIEWSHARED_EXPORT ICFBRobot3DViewEvent
{
public:
    
    virtual void ICFBRobot3DViewEvent_On3DModelLoaded() = 0;

    
    virtual bool ICFBRobot3DViewEvent_IsRobotDebuging() = 0;

    virtual void ICFBRobot3DViewEvent_SetRotation(int nId, int nAngle, int nTime) = 0;
    virtual void ICFBRobot3DViewEvent_ReadRotation(int nID, bool bPowerDown = true) = 0;

    virtual void ICFBRobot3DViewEvent_SetAnglesToRobot(MOTOR_DATA* pMotor, int nMotorCount, int nRunTime , int nCurveType = 0) = 0;

    virtual void ICFBRobot3DViewEvent_StopActionPlay() = 0;
};


class CFBROBOT3DVIEWSHARED_EXPORT ICFBRobot3DView
{
public:
    
    virtual void Connect(QWidget* parent, ICFBRobot3DViewEvent* pEvent) = 0;

    
    virtual void Disconnect() = 0;

    
//    virtual bool SetSerialPortControl(CCFBSerialPortSesstion *pConnect) = 0;

    //获取各舵机角度
    virtual bool GetMotorDatas(char** pData, int& nLen, emActionLayerType eActionType) = 0;
    //设置各舵机角度
    virtual void SetMotorDatas(char* pData, int nLen, int runTime) = 0;
    //设置各舵机角度
    virtual void SetMotorDatasManual(char* pData, int nLen, int runTime) = 0;
    virtual void SetMotorAngle(int nID, int nAngle) = 0;
    //设置3D模型的仿真状态
    virtual void Set3DModelPlayState(bool bState) = 0;
    virtual void SetMotorPowerState(int nMotorID, int nState) = 0;
    //设置所有舵机角度
    virtual void SetAllMotorAngle(char* pData, int nLen, int nRunTime, int nAllTime) = 0;
    //获取串口连接状态
    virtual bool GetComConnectState() = 0;
    //启用/禁用控制界面
    virtual void EnableViewCtrl(bool bEnable) = 0;
    //获取舵机数目
    virtual int GetMotorCount() = 0;
    //获取窗体
    virtual QWidget* GetWidget() = 0;

    virtual void SetMotorDatasSmoothly(char* pData, int nLen, int iTime) = 0;

//    //耳朵灯仿真
//    virtual void PlayEarData(int RunTime,int LeftLed,int RightLed,int Bright,int LedLightUpTime,int LedLightDownTime) = 0;
//    //眼睛灯仿真
//    virtual void PlayEyeData(int LeftLed, int RightLed, int Bright, int Color, int LightUpTime, int LightDownTime, int RunTime) = 0;
//    //停止眼睛灯仿真
//    virtual void StopPlayEyeData() = 0;
//    //停止耳朵灯仿真
//    virtual void StopPlayEarData() = 0;
};

bool CFBROBOT3DVIEWSHARED_EXPORT CreateCFBRobot3DView(ICFBRobot3DView ** pCFBRobot3DView);

#endif // ICFBROBOT3DVIEW_H
