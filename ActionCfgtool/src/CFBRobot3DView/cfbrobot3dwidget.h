

#ifndef CFBROBOT3DWIDGET_H
#define CFBROBOT3DWIDGET_H

#include <QMap>
#include "ICFBRobot3DView.h"
#include "cfbrobotcontrolinterface.h"
#include "cfbrobot3ddisplay.h"


//多线程方式使用QGLWidget http://stackoverflow.com/questions/18345356/qglwidget-in-another-thread-what-is-the-documentation-referring-to
class CFBRobot3DWidget : public QGLWidget, public ICFBRobotControl

{
    Q_OBJECT

public:
    explicit CFBRobot3DWidget(QWidget *parent = 0, ICFBRobot3DViewEvent *pEvent = 0, ICFBRobotViewControlNotify* pNotify = 0);
    ~CFBRobot3DWidget();

    //ICFBRobotControl接口函数实现
    virtual bool Control_GetMotorDatas(char** pData, int& nLen, emActionLayerType eActionType);
    virtual void Control_SetMotorDatas(char* pData, int nLen, int runTime);
    virtual int Control_GetMotorAngles(char* pData, int nDataLen, int nMotorID);
    virtual void Control_SetViewMotorDatasManual(char* pData, int nLen, int runTime);
    virtual void Control_SetMotorAngle(int nID, int nAngle);
    virtual void Control_SetMotorPowerState(int nMotorID, int nState);
    virtual void Control_SetAllMotorPowerState(int nState);
    //virtual void Control_EnableControl(bool bEnable);
public:
    int GetMotorCount();
    void initializeGLContext();

    
    void UpdateNodeDate(const char* szNodeName, float angle, float offset, TypeAngleChanged nChangeType);
protected:

    //以下为QGLWidget 虚函数实现
    
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    //QWidget窗体处理事件
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *);

signals:
    //以下为窗体鼠标事件，分开发送为了对应FbxPlayer中的void MouseCallback(int button, int state, int x, int y);和void MotionCallback(int x, int y);
    void mouseEvent(QMouseEvent *event);//鼠标抬起和按下
    void mouseMotion(QMouseEvent *event);//鼠标移动
    void mouseWheel(QWheelEvent *event);//鼠标中键事件

    void SigUpdateNodeDate(const char* szNodeName, float angle, float offset, TypeAngleChanged nChangeType);
    //
    void clickNode(int nID);
    void angleChanged(int nID, int value, int nTime, TypeAngleChanged nType);
    void reset();
    void resized(int w, int h);
    void nextFrame();
    void setPlayState(bool bstate);
    void RotationSmooth(int nID, float fRot, int nTime);
    void updateData();//请求fbx更新UI角度信息，用于平滑动画后，请求更新

    void SigFirstFrameFinished();

    void sigSoftSimulate(MOTOR_DATA* pMotor , int nMotorCount , int nTime , int nCurveType);

protected slots:
    void OnFirstFrameFinished();

private:
    ICFBRobot3DViewEvent* m_pICFBRobot3DViewEvent;
    ICFBRobotViewControlNotify* m_pNotify;
    CFBRobot3DDisplay* m_pDisplay;
    QThread m_thread;
    QMap<int, int> m_mapPowerState;

    bool m_bEnableShow;
};

#endif // CFBROBOT3DWIDGET_H
