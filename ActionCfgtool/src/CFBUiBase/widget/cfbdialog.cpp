


#include "cfbdialog.h"

#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QApplication>



#include "cfbwidget.h"



CCFBDialog::CCFBDialog(QWidget *parent):
    QDialog(parent),
    m_bLeftDown(false),
    m_bEnableMove(false),
    m_nTitleBarHeight(TITLE_BAR_HEIGHT),
    m_ptPre(QPoint(-1, -1))
{

}

CCFBDialog::~CCFBDialog()
{

}

void CCFBDialog::paintEvent(QPaintEvent *)
{
    CFBWidget::paintEvent(this);
}


void CCFBDialog::setEnableMove(bool bEnableMove)
{
    m_bEnableMove = bEnableMove;
}

void CCFBDialog::setTitleBarHeight(const int &titleBarHeight)
{
    if(0 <= titleBarHeight)
    {
        m_nTitleBarHeight = titleBarHeight;
    }
}


void CCFBDialog::mousePressEvent(QMouseEvent *event)
{
    //for move
    if(Qt::LeftButton == event->button())
    {
        m_ptPre = event->globalPos();
        m_bLeftDown = true;
    }
    else
    {
        m_bLeftDown = false;
    }
}

void CCFBDialog::mouseMoveEvent(QMouseEvent *event)
{
    //for move
    if(m_bEnableMove)
    {
        QRect rectTitleBar(0, 0, this->width(), m_nTitleBarHeight);
        if(m_bLeftDown && rectTitleBar.contains(mapFromGlobal(m_ptPre)))
        {
            QPoint nowPt = event->globalPos();
            QPoint moveTo = mapToParent(nowPt - m_ptPre);
            move(moveTo);
            m_ptPre = nowPt;
        }
    }
}

void CCFBDialog::mouseReleaseEvent(QMouseEvent * )
{
    //for move
    m_bLeftDown = false;
}
