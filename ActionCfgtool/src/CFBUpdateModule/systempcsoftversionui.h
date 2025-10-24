#ifndef SYSTEMPCSOFTVERSIONUI_H
#define SYSTEMPCSOFTVERSIONUI_H
#include "cfbupdatemodule_global.h"
#include "util.h"
#include "global.h"
#include "cfbwidget.h"


class CUpdateProgressDlg;


namespace Ui {
class SystemEmbeddedUpdateWidget;
}


class CFBUPDATEMODULESHARED_EXPORT systemPCSoftVersionUI : public CFBWidget
{
    Q_OBJECT

public:
    explicit systemPCSoftVersionUI(QWidget *parent = 0);
    ~systemPCSoftVersionUI();


public:
    void initUI();

    static bool getVersionParams(QString &sUpdateURL, QString &sLocalVer, emCheckUpdateType &eCheckType);

    int getUpdateCheckStatus();

protected:


private slots:
    
    void slotOnUpdate();


private:
    Ui::SystemEmbeddedUpdateWidget *ui;

    CUpdateProgressDlg* pUpdateProgressDlg;

    //
    emCheckUpdateType  m_eChkType;
    FILEINFO         m_LatestVersionInfo;//下载的文件信息
    QString          m_sUpdateURL;//更新地址
    QString          m_sLocalVer;//本地版本
};

#endif // SYSTEMPCSOFTVERSIONUI_H
