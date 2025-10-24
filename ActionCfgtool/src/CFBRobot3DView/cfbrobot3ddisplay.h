

#ifndef CFBROBOT3DDISPLAY_H
#define CFBROBOT3DDISPLAY_H
#include <QGLContext>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QPointer>
#include <QThread>
#include <QWindow>


class CFBRobot3DDisplay : public QObject
{
    Q_OBJECT
public:
    explicit CFBRobot3DDisplay(QGLContext *ctx);
    ~CFBRobot3DDisplay();
public:
    int getMotorCount();
    QMap<int, int> getMotorInfo();

    //获取是否可见
    bool isVisible();

    //设置是否可见
    void setVisible(bool bVisible);


public slots:
    //响应3D窗体大小改变的槽函数
    void onSize(int w, int h);
    //响应定时器槽函数，定时刷新3D显示
    void onNextFrame();
    //响应平滑动画槽函数，进行平滑设置
    void onTriggerModelFreshed(int eActionType);
    //响应控制界面中选中节点时的槽函数
    void onClickNode(int nID);
    //响应控制界面reset事件的槽函数
    void onReset();

    
    void onAngleChanged(int nID, int value, int nTime, TypeAngleChanged nType);

    void onSetModelPlayState(bool bstate);
    void onUpdateData();

    void slotSoftSimulate(MOTOR_DATA* pMotor , int nMotorCount , int nTime , int nCurveType);

    //鼠标事件槽函数
    void onMouseEvent(QMouseEvent *event);//鼠标抬起和按下
    void onMouseMotion(QMouseEvent *event);//鼠标移动
    void onMouseWheel(QWheelEvent *event);//鼠标中键事件
protected:
    void timerEvent(QTimerEvent *event);

signals:
    void SigFirstFrameFinished();//第一帧显示完成

private:
    QGLContext* m_pGLContext;//封装了OpenGL 渲染上下文，具体的用法参考Qt帮助文档
    QBasicTimer m_timer;//基础定时器，用于控制刷新的帧率
    bool m_bIsFirstFrame;
    bool m_bVisible;   //是否可见
};

#endif // CFBROBOT3DDISPLAY_H
