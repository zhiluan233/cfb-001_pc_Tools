

#ifndef CFBUPDATEADMINISTRATOR_H
#define CFBUPDATEADMINISTRATOR_H

#include <QObject>

#include "systemembeddedupdatewidget.h"
#include "cfbupdatemodule_global.h"
#include "systempcsoftversionui.h"
#include "videotutorialhandle.h"

class CFBUPDATEMODULESHARED_EXPORT CFBUpdateAdministrator : public QObject,  public zxObserverBase
{
    Q_OBJECT

public:
    
    static CFBUpdateAdministrator *getInstance();

    
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

public:
    // 设置、获取版本更新地址
    void setUpdateBasicUrl(const QString &sUrl) { m_sUpdateBasicUrl = sUrl; }
    QString getUpdateBasicUrl() { return m_sUpdateBasicUrl; }

    
    bool checkUpdateSystemsEmbedded(bool bCanShowMBox);
    void showSytemPCSoftVersionUI();

    systemPCSoftVersionUI *getSystemPCSoftVersionUI();
    SystemEmbeddedUpdateWidget *getSysEmbedUpdateWidget();
    CVideoTutorialHandle *getVideoTutorialHandle();

private slots:
    void slotOnTimeout();

signals:
    void sigSystemEmbedVersionUpdateStatus(int nStatus);
    void sigIsSystemEmbedVersionNeedCopyUpdate(bool bMustUpdate); // 嵌入式版本信息
    void sigOnNotifyChangedUdiskResult(bool bOk);  // 是否切换U盘模式成功信号

private:
    explicit CFBUpdateAdministrator(QObject *parent = 0);
    ~CFBUpdateAdministrator();

private:
    QString m_sUpdateBasicUrl;   // 版本更新地址

    SystemEmbeddedUpdateWidget *m_pSysEmbedUpdateWidget;
    systemPCSoftVersionUI *m_pSystemPCSoftVersionUI;
    CVideoTutorialHandle *m_pVideoTutorialHandle;
};

#endif // CFBUPDATEADMINISTRATOR_H
