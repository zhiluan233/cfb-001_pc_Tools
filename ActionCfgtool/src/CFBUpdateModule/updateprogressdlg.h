
#ifndef UPDATEPROGRESSDLG_H
#define UPDATEPROGRESSDLG_H

#include "cfbupdatemodule_global.h"
#include "Global.h"

#include <QDialog>

namespace Ui {
class UpdateProgressDlg;
}

class MyDownloadProc : public QObject
{
    Q_OBJECT
public:
    explicit MyDownloadProc(QWidget* pParent);
    ~MyDownloadProc();

public slots:
    void start();

public:
    
    void DownProc(LPVOID lParam);
private:
    QWidget* m_pParent;

};

class CFBUPDATEMODULESHARED_EXPORT CUpdateProgressDlg : public QDialog
{
    Q_OBJECT    
public:
    explicit CUpdateProgressDlg(QWidget* pParent = NULL);
    ~CUpdateProgressDlg();

    
    void OnDownloadResult(bool bRet, char* pData, int nDataLen);

	
    void DoCheck();

protected:
    virtual void customEvent(QEvent* event);

private slots:
    void OnCancel();

public:
    bool            m_bShowTipOnFailed;//未检测到更新时候是否提示	

    QString          m_sUpdateURL;//更新地址
    QString          m_sLocalVer;//本地版本
    emCheckUpdateType  m_eChkType;
    FILEINFO        m_LatestVersionInfo;//下载的文件信息


    Ui::UpdateProgressDlg *ui;

    MyDownloadProc* m_pDownloadTask;
};

#endif
