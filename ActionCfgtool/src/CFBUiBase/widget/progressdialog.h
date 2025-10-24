


#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H
#include "cfbuibase_global.h"
#include <QProgressDialog>
class QKeyEvent;
class QString;
class CFBUIBASE_EXPORT CProgressDialog : public QProgressDialog
{
public:
    CProgressDialog(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    CProgressDialog(const QString &labelText, const QString &cancelButtonText,
                    int minimum, int maximum, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~CProgressDialog();

public:
    void setKeyPressEnable(bool bKeyEnable);

protected:
    void keyPressEvent(QKeyEvent *) override;
    virtual bool eventFilter(QObject *, QEvent *)override;

private:
    void setUpEventFilter();

private:
    bool m_bKeyEnable;
};

#endif // CPROGRESSDIALOG_H
