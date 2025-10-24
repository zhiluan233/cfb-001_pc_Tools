#include "stdafx.h"
#include "cfbrobot3dcontainer.h"
#include "ui_cfbrobot3dcontrol.h"
#include "cfbrobot3dwidget.h"
#include "cfbrobot3dconstants.h"
#include "configs.h"

bool  CreateCFBRobot3DView(ICFBRobot3DView ** ppRobot3dView)
{
  CFBRobot3DContainer *pProxy = new CFBRobot3DContainer();
  if(pProxy)
  {
      *ppRobot3dView = (ICFBRobot3DView*)pProxy;
      return true;
  }
  return false;
}

CFBRobot3DContainer::CFBRobot3DContainer(QWidget *parent, ICFBRobot3DViewEvent *pCFBRobotEvent) :
    QWidget(parent),
    ui(new Ui::CFBRobot3DControl)
{
	m_pCFBRobot3DWidget = NULL;
    m_pEvent = NULL;
    ui->setupUi(this);
    setDefaultStyle();
    //线程通知窗体enbale时调用
    connect(this, &CFBRobot3DContainer::SigEnable, this, &CFBRobot3DContainer::setEnabled);
}

CFBRobot3DContainer::~CFBRobot3DContainer()
{
    delete ui;
}

//void CFBRobot3DControl::SetSerialPortControl(CCFBSerialPortSesstion *pConnect)
//{
//    m_pConnectPort = pConnect;
//}

void CFBRobot3DContainer::UpdateNodeDate(const char *szNodeName, float angle, float offset, TypeAngleChanged nChangeType)
{
    Q_ASSERT(szNodeName != NULL);
    if(m_pControlWidget)
    {
        m_pControlWidget->UpdateNodeDate(szNodeName, angle, offset, nChangeType);
    }
}

void CFBRobot3DContainer::Set3DViewRotationManual(int nDir, float fRot, const char *pNode, int nTime)
{
    //ManualSetRotation1(nDir, fRot, pNode, nTimer);
}

void CFBRobot3DContainer::SetRotation(float x, float y, float z, float w, char *pNode)
{
    //PlaySetRotation2(x, y, z, w, pNode);源码中也已经注释
}

void CFBRobot3DContainer::OnSetRotation(int nID, int nAngle, int iTime)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->SetAngleData(nID, nAngle);
    //    }
}

bool CFBRobot3DContainer::IsComConnectted()
{
//    if(m_pConnectPort)
//    {
//        return m_pConnectPort->IsConnectted();
//    }
//    else
//    {
//        return false;
    //    }
    return true;
}

bool CFBRobot3DContainer::IsRobotDebuging()
{
    if(m_pEvent)
    {
        return m_pEvent->ICFBRobot3DViewEvent_IsRobotDebuging();
    }
    return false;
}

void CFBRobot3DContainer::LostPowerForRobot(int nID)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ReadBackAngleOrPowerDown(nID);
    //    }
}

void CFBRobot3DContainer::OnReadRotation(int nID)
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ReadBackAngleOrPowerDown(nID);
//    }
}

void CFBRobot3DContainer::ResetAllAngle()
{
//    if(m_pConnectPort)
//    {
//        m_pConnectPort->ResetRobot();
    //    }
}

void CFBRobot3DContainer::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/3dviewdefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void CFBRobot3DContainer::Connect(QWidget *parent, ICFBRobot3DViewEvent *pEvent)
{
    m_pCFBRobot3DWidget = new CFBRobot3DWidget(this, pEvent, this);
    connect(m_pCFBRobot3DWidget, &CFBRobot3DWidget::SigFirstFrameFinished, this, &CFBRobot3DContainer::OnFirstFrameFinished);
    ui->verticalLayout->addWidget(m_pCFBRobot3DWidget);
    m_pControlWidget = new CFBRobot3DControlWidget();
    m_pControlWidget->ConnectToMainWidgets(pEvent);

    ui->verticalLayout->addWidget(m_pControlWidget);
    QObject::connect(m_pControlWidget, &CFBRobot3DControlWidget::angleChanged,
                     m_pCFBRobot3DWidget, &CFBRobot3DWidget::angleChanged);

    QObject::connect(this, &CFBRobot3DContainer::angleChanged,
                     m_pCFBRobot3DWidget, &CFBRobot3DWidget::angleChanged);

    QObject::connect(m_pControlWidget, &CFBRobot3DControlWidget::clickNode,
                     m_pCFBRobot3DWidget, &CFBRobot3DWidget::clickNode);
    QObject::connect(m_pControlWidget, &CFBRobot3DControlWidget::reset,
                     m_pCFBRobot3DWidget, &CFBRobot3DWidget::reset);
    m_pCFBRobot3DWidget->initializeGLContext();
    m_pEvent = pEvent;
}

void CFBRobot3DContainer::Disconnect()
{
    if(m_pControlWidget)
        delete m_pControlWidget;
    if(m_pCFBRobot3DWidget)
        delete m_pCFBRobot3DWidget;
    delete this;
}

bool CFBRobot3DContainer::GetMotorDatas(char **pData, int &nLen, emActionLayerType eActionType)
{
    if(m_pCFBRobot3DWidget)
    {
        return m_pCFBRobot3DWidget->Control_GetMotorDatas(pData, nLen, eActionType);
    }
    return false;
}

void CFBRobot3DContainer::SetMotorDatas(char *pData, int nLen, int runTime)
{
    if(m_pCFBRobot3DWidget)
    {
        m_pCFBRobot3DWidget->Control_SetAllMotorPowerState(POWERON);
        m_pCFBRobot3DWidget->Control_SetMotorDatas(pData, nLen, runTime);
    }
}

void CFBRobot3DContainer::SetMotorDatasManual(char *pData, int nLen, int runTime)
{
    if(m_pCFBRobot3DWidget)
    {
        m_pCFBRobot3DWidget->Control_SetViewMotorDatasManual(pData, nLen, runTime);
    }
}

void CFBRobot3DContainer::SetMotorAngle(int nID, int nAngle)
{
    if(m_pCFBRobot3DWidget)
    {
        m_pCFBRobot3DWidget->Control_SetMotorAngle(nID, nAngle);
    }
}

void CFBRobot3DContainer::Set3DModelPlayState(bool bState)
{
    if(m_pCFBRobot3DWidget)
    {
        emit m_pCFBRobot3DWidget->setPlayState(bState);
    }
}

void CFBRobot3DContainer::SetMotorPowerState(int nMotorID, int nState)
{
    if(m_pCFBRobot3DWidget)
    {
        m_pCFBRobot3DWidget->Control_SetMotorPowerState(nMotorID, nState);
    }
}


void CFBRobot3DContainer::SetAllMotorAngle(char *pData, int nLen, int nRunTime, int nAllTime)
{
    if(pData == NULL || nLen <= 4)
    {
        return;
    }

    int nCfgMotorCount = CConfigs::getCfgMotorCount(); //配置的舵机数
    int nColumnCount = nCfgMotorCount + 2;
    int* pAngleArr = new int[nColumnCount];

    MOTOR_DATA* pMotor = (MOTOR_DATA*)(pData + sizeof(nLen));
    int nCount = (nLen - sizeof(nLen)) / sizeof(MOTOR_DATA);
    for(int i = 0; i < nCount && i < nCfgMotorCount; i++) //源码中此处用16，暂时不知道为什么不用ncount，调试时可以看看
    {
        pAngleArr[i] = pMotor[i].nAngle;
    }
    pAngleArr[nColumnCount - 2] = nRunTime;
    pAngleArr[nColumnCount - 1] = nAllTime;

//    if(m_pConnectPort)
//    {
//        m_pConnectPort->DebugRobot(AngleArr);
    //    }
    SAFE_DELETE_ARRAY(pAngleArr);
}


void CFBRobot3DContainer::OnSetRobotAngle(int nID, int nAngle)
{
    emit angleChanged(nID, nAngle, 100, TAC_ReadBack);
}

bool CFBRobot3DContainer::GetComConnectState()
{
//    if(m_pConnectPort)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
    //    }
    return false;
}

void CFBRobot3DContainer::EnableViewCtrl(bool bEnable)
{
    if(m_pControlWidget)
    {
        //线程通知窗体enbale时调用
        emit SigEnable(bEnable);
        if(bEnable)
        {
            emit m_pCFBRobot3DWidget->updateData();
        }
    }
}

int CFBRobot3DContainer::GetMotorCount()
{
    if(m_pCFBRobot3DWidget)
    {
        return m_pCFBRobot3DWidget->GetMotorCount();
    }
    return 0;
}

QWidget *CFBRobot3DContainer::GetWidget()
{
    return this;
}

void CFBRobot3DContainer::showEvent(QShowEvent *event)
{
    if(isVisible())
    {
        int ratio = windowHandle()->devicePixelRatio();
        QSize sizeRe = size();
        sizeRe.setWidth(sizeRe.width() / ratio);
        resize(sizeRe);
    }
}

void CFBRobot3DContainer::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CFBRobot3DContainer::OnFirstFrameFinished()
{
    ui->verticalLayout->removeWidget(ui->widgetTemp);
    ui->widgetTemp->deleteLater();

    m_pCFBRobot3DWidget->show();
}

CFBRobot3DControlWidget* CFBRobot3DContainer::GetCtrlWidget()
{
    return m_pControlWidget;
}

CFBRobot3DWidget *CFBRobot3DContainer::get3DWidget()
{
    return m_pCFBRobot3DWidget;
}

void CFBRobot3DContainer::SetMotorDatasSmoothly(char* pData, int nLen, int iTime)
{
    // Modify by jianjie 2017/10/23 舵机数据带上ID号，增加运动曲线类型
    if (NULL == pData || nLen < 4)
    {
        return;
    }

    // 舵机数据
    int nCount = (nLen - sizeof(nLen)) / sizeof(MOTOR_DATA);

    MOTOR_DATA* pMotor = new MOTOR_DATA[nCount];
    memcpy(pMotor, (MOTOR_DATA*)(pData + sizeof(nLen)), nCount * sizeof(MOTOR_DATA));

    // 曲线类型
    int nCurveType = 0;
    memcpy(&nCurveType , pData + sizeof(nLen) + nCount * sizeof(MOTOR_DATA) , sizeof(nCurveType));

    if (m_pEvent)
    {
        m_pEvent->ICFBRobot3DViewEvent_SetAnglesToRobot(pMotor, nCount, iTime , nCurveType);
    }

}

