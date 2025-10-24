

#ifndef CCFBDIALOG_H
#define CCFBDIALOG_H
#include "cfbuibase_global.h"
#include <QDialog>

class CFBUIBASE_EXPORT CCFBDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CCFBDialog(QWidget *parent = 0);
    virtual ~CCFBDialog();

public:
    //设置窗体默认样式
    void setEnableMove(bool bEnableMove);
    void setTitleBarHeight(const int& titleBarHeight);

protected:
    void paintEvent(QPaintEvent *);

    //重写以下函数实现移动窗体功能
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);



private:
    bool m_bLeftDown;
    bool m_bEnableMove;
    int m_nTitleBarHeight;

    QPoint m_ptPre;
};

#endif // CCFBDIALOG_H



