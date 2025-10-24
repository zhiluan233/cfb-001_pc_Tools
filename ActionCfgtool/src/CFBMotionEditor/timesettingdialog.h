#ifndef TIMESETTINGDIALOG_H
#define TIMESETTINGDIALOG_H

#include "motioneditor_global.h"
#include <QDialog>

namespace Ui {
class CTimeSettingDialog;
}

class MOTIONEDITOR_EXPORT CTimeSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CTimeSettingDialog(QWidget *parent = 0);
    ~CTimeSettingDialog();
public:
    
    void SetTimeScale(int nTimeScale);

    
    int GetTimeScale();

protected:
    virtual void paintEvent(QPaintEvent *);

private slots:
    void on_pbClose_clicked();

    void on_pbCancel_clicked();

    void on_pbOK_clicked();

private:
    Ui::CTimeSettingDialog *ui;
//    int m_nTimeScale;
};

#endif // TIMESETTINGDIALOG_H
