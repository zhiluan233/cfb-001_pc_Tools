#include "stdafx.h"
#include "cfbrobot3ddisplay.h"
#include "FbxPlayer.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

CFBRobot3DDisplay::CFBRobot3DDisplay(QGLContext *ctx)
    : m_pGLContext(ctx),
      m_bVisible(true)
{
    //定时刷新3D绘制
    m_timer.start(20, this);
    m_bIsFirstFrame = true;
}

CFBRobot3DDisplay::~CFBRobot3DDisplay()
{
    m_timer.stop();
}

int CFBRobot3DDisplay::getMotorCount()
{
    return GetMotorCount();
}

QMap<int, int> CFBRobot3DDisplay::getMotorInfo()
{
    return GetMotorInfo();
}

void CFBRobot3DDisplay::onSetModelPlayState(bool bstate)
{
    SetModelPlayState(bstate);
}

void CFBRobot3DDisplay::slotSoftSimulate(MOTOR_DATA* pMotor , int nMotorCount , int nTime , int nCurveType)
{
    OnRotationSmooth(pMotor , nMotorCount , nTime , nCurveType);
}

void CFBRobot3DDisplay::onUpdateData()
{
    OnUpdateData();
}

void CFBRobot3DDisplay::onSize(int w, int h)
{
    //在线程事件中调用FbxPlayer对应的函数
    m_pGLContext->makeCurrent();
    ReshapeCallback(w, h);
}


void CFBRobot3DDisplay::onNextFrame()
{
    //在线程事件中调用FbxPlayer对应的函数
    //qDebug()<<"CFBRobot3DDisplay::onNextFrame()::"<<this->thread()<<"\n";
    m_pGLContext->makeCurrent();
    DisplayCallback();
    m_pGLContext->swapBuffers();
}

void CFBRobot3DDisplay::onTriggerModelFreshed(int eActionType)
{
    TriggerModelFreshed(LIBFBX_3DMODEL_FRESH_SMOOTHLY, TAC_MotionEditor , (emActionLayerType)eActionType);
}

void CFBRobot3DDisplay::onClickNode(int nID)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnClickNode(nID);
}

void CFBRobot3DDisplay::onReset()
{
    //在线程事件中调用FbxPlayer对应的函数
    OnReset();
}

void CFBRobot3DDisplay::onAngleChanged(int nID, int value, int nTime, TypeAngleChanged nType)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnAngleChanged(nID, value, nTime, nType);
}


void CFBRobot3DDisplay::onMouseEvent(QMouseEvent *event)
{
    qDebug()<<"CFBRobot3DDisplay::"<<this->thread()<<"\n";
    //在线程事件中调用FbxPlayer对应的函数
    OnMouseEvent(event);
    delete event;
}

void CFBRobot3DDisplay::onMouseMotion(QMouseEvent *event)
{
   //在线程事件中调用FbxPlayer对应的函数
   OnMotionEvent(event);
   delete event;
}

void CFBRobot3DDisplay::onMouseWheel(QWheelEvent *event)
{
    //在线程事件中调用FbxPlayer对应的函数
    OnWheelEvent(event);
    delete event;
}

void CFBRobot3DDisplay::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timer.timerId())
    {
        if(m_bVisible)
        {
            onNextFrame();
            if(m_bIsFirstFrame)
            {
                emit SigFirstFrameFinished();
                m_bIsFirstFrame = false;
            }
        }
    }
}

//获取是否可见
bool CFBRobot3DDisplay::isVisible()
{
    return m_bVisible;
}

//设置是否可见
void CFBRobot3DDisplay::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
}
