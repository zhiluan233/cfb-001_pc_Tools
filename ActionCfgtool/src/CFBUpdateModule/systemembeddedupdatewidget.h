

#ifndef SYSTEMEMBEDDEDUPDATEWIDGET_H
#define SYSTEMEMBEDDEDUPDATEWIDGET_H

#include <QWidget>
#include <QProgressDialog>
#include <QMouseEvent>
#include <QProcess>
#include "CFBRobotController.h"
#include "global.h"
#include "cfbupdatemodule_global.h"
#include "cthreadcopyfile.h"
#include "cthreadwaitforudiskshowup.h"

class CProgressDialog;
#ifdef Q_OS_MAC
#include "myusbdevice.h"
#endif

namespace Ui {
class SystemEmbeddedUpdateWidget;
}


class CFBUPDATEMODULESHARED_EXPORT SystemEmbeddedUpdateWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SystemEmbeddedUpdateWidget(QWidget *parent = 0);
    ~SystemEmbeddedUpdateWidget();

    
    bool doCheckUpdate(bool bCanShowMBox);

    // 判断是是否是需要强制拷贝升级的版本
    bool isForceCopyUpateVersion();

    
    static QString getVersionInNumber(const QString &sVersion);

public slots:
    // 切换到U盘模式结果
    void slotOnChangedToUDiskResult(bool bOk);

    // 切换到U盘模式后，等待U盘出现的结果
    void slotOnRobotUDiskShowUp(bool bShowup);

signals:
    
    void sigReadRobotVersionFinished();

    // 嵌入式系统版本是否需要需要切换到U盘模式拷贝升级
    void sigIsSystemEmbedVersionNeedCopyUpdate(bool bMustUpdate);

    // 嵌入式版本状态，当前版本和后台版本比较看是否需要更新
    void sigSystemEmbedVersionUpdateStatus(int nStatus);

protected:

    virtual void timerEvent(QTimerEvent *event) override;
    virtual void customEvent(QEvent * event) override;

    
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *) override;

    
    virtual void mouseMoveEvent(QMouseEvent*event) override;

    
    virtual void mousePressEvent(QMouseEvent*event) override;

    
    virtual void mouseReleaseEvent(QMouseEvent*event) override;

private slots:

    
    void slotOnUpdate();

    
    void slotOnCancelUpdate();

    
    void slotOnClosed();

    
    void slotUpdateUI();

    // U盘格式化完成
    void slotOnFormatUdiskFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotOnFormatUdiskStateChanged(QProcess::ProcessState newState);

    // 拷贝文件完成
    void slotOnCopyFileFinished();

private:
    
    void setOnMenuClickedTrigger(bool bIsMenuClickedTrigger);

    
    bool downloadUpdateToLocalComputer();

    
    bool downloadUpdateToRobot();

    
    QString getLocalDownloadFilePath();

    
    void reqRobotHardwareVersion();

    
    void reqLocalSystemEmbedVersion();

    
    void reqLatestSysEmbededVersionOnServer();

    
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam) override;

    
    bool processRobotSoftwareVersion(WPARAM wParam, LPARAM lParam);

    
    bool processRobotHardwareVersion(WPARAM wParam, LPARAM lParam);

    
    void onDownloadUpdateProgress(WPARAM wParam, LPARAM lParam);

    
    void onDownloadUpdateProgressEnd(WPARAM wParam, LPARAM lParam);

private:

    // 判断指定路径下是否含有中文名字文件
    bool isActionContainChineseChar(const QString &strFilePath);

    // 判断字符串中是否含有中文字符
    bool isContainsChineseChar(const QString &strText);

    // 格式化移动盘
    void formatUDisk(const QString &strFormat, const QString &strDiskName, const QString strBSDName = "");

    // 判断机器人是否是1P，不是1P即1S
    bool isRobotType1P();

    // 获取bin文件路径
    QString getBinFilePath(bool bIsChinese);

//    CString GetDiskFlag(const CString& sDeviceName, const CString& sDeviceLink);

private:
    Ui::SystemEmbeddedUpdateWidget *ui;

    // 在实现机器人嵌入式版本、机器人类型版本获取、检查更新、弹出更新提示是按顺序依次串行执行，
    // 因此如果在其他多个地方调用获取嵌入式版本和机器人型号的接口，根据MSG_ID都会
    // 接收到对应消息，从而会触发整个串行执行，因此在这里定义标识，只有在用户点击菜单栏的固件
    // 版本时，才会接收消息
    bool m_bOnMenuClickedTrigger;
    bool m_bCanShowMBox; // 是否显示版本信息界面

    bool m_bIsChinese; // 判断中英文

    QPoint   m_dragPosition; // 记录位置信息

    QLabel *m_pProgressLabText;
    QProcess m_formatUdiskProcess;  // 格式化U盘线程
    CProgressDialog* m_pProgressDialog;

    emCheckUpdateType m_eRobotType;  // 版本类型
    QString m_strLocalSysEmbedVersion; // 当前嵌入式版本

    QtHttpOperSync *m_httpOperSync;  // 下载
    FILEINFO m_LatestVersionInfo;  // 最新版本信息
    TRANSFILEINFO  m_TransFileInfo; // 文件信息

    CThreadCopyFile m_oThreadCopyFile;
    CThreadWaitForUdiskShowup m_oThreadWaitUdiskShowUp;

    int m_nUpdateProgressTimerID;

    int m_nTimeCount;
};

#endif // SYSTEMEMBEDDEDUPDATEWIDGET_H
