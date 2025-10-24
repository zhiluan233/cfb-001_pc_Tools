

#include "cfbupdateadministrator.h"


CFBUpdateAdministrator *CFBUpdateAdministrator::getInstance()
{
    static CFBUpdateAdministrator updateAdm;
    return &updateAdm;
}


void CFBUpdateAdministrator::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    if(nMsgID == WM_MSG_ROBOT_SWITCHMODE_RESULT)
    {
        if (m_pSysEmbedUpdateWidget && m_pSysEmbedUpdateWidget->isForceCopyUpateVersion())
        {
            // 切换U盘模式
           emit sigOnNotifyChangedUdiskResult((bool) wParam);
        }
    }
    else if (nMsgID == WM_MSG_ROBOT_CONNECT)
    {
         AlphaRobotLog::getInstance()->printDebug("Get robot connect in CFBUpdateAdministrator");
         getSysEmbedUpdateWidget();  // 构建对象，为后续使用准备，因为连接上机器人后，软件会自动触发查询嵌入式版本的命令，自动走嵌入式版本更新流程
    }
}

CFBUpdateAdministrator::CFBUpdateAdministrator(QObject *parent) : QObject(parent)
   ,m_pSysEmbedUpdateWidget(NULL)
   ,m_pSystemPCSoftVersionUI(NULL)
   ,m_pVideoTutorialHandle(NULL)
{
    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }
}

CFBUpdateAdministrator::~CFBUpdateAdministrator()
{
    SAFE_DELETE(m_pSysEmbedUpdateWidget);
    SAFE_DELETE(m_pSystemPCSoftVersionUI);
}

CVideoTutorialHandle *CFBUpdateAdministrator::getVideoTutorialHandle()
{
    if(NULL == m_pVideoTutorialHandle)
    {
        m_pVideoTutorialHandle = new CVideoTutorialHandle();
    }

    return m_pVideoTutorialHandle;
}

SystemEmbeddedUpdateWidget *CFBUpdateAdministrator::getSysEmbedUpdateWidget()
{
    if(NULL == m_pSysEmbedUpdateWidget)
    {
        m_pSysEmbedUpdateWidget = new SystemEmbeddedUpdateWidget();

        connect(m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::sigSystemEmbedVersionUpdateStatus,
                this, &CFBUpdateAdministrator::sigSystemEmbedVersionUpdateStatus);
        connect(m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::sigIsSystemEmbedVersionNeedCopyUpdate,
                this, &CFBUpdateAdministrator::sigIsSystemEmbedVersionNeedCopyUpdate);
        connect(this, &CFBUpdateAdministrator::sigOnNotifyChangedUdiskResult, m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::slotOnChangedToUDiskResult);
    }

    return m_pSysEmbedUpdateWidget;
}

void CFBUpdateAdministrator::slotOnTimeout()
{
      getSysEmbedUpdateWidget()->doCheckUpdate(false);
}

systemPCSoftVersionUI *CFBUpdateAdministrator::getSystemPCSoftVersionUI()
{
    if(NULL == m_pSystemPCSoftVersionUI)
    {
        m_pSystemPCSoftVersionUI = new systemPCSoftVersionUI();
    }

    return m_pSystemPCSoftVersionUI;
}



bool CFBUpdateAdministrator::checkUpdateSystemsEmbedded(bool bCanShowMBox)
{
    getSysEmbedUpdateWidget()->doCheckUpdate(bCanShowMBox);
    return true;
}


void CFBUpdateAdministrator::showSytemPCSoftVersionUI()
{
    getSystemPCSoftVersionUI()->initUI();
    getSystemPCSoftVersionUI()->show();
}
