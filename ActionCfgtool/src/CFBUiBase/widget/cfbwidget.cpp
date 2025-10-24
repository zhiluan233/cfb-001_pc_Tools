#include "cfbwidget.h"


#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QApplication>


CFBWidget::CFBWidget(QWidget *parent):
    QWidget(parent),
    m_bLeftDown(false),
    m_bEnableMove(false),
    m_nTitleBarHeight(TITLE_BAR_HEIGHT),
    m_ptPre(QPoint(-1, -1))
{

}

CFBWidget::~CFBWidget()
{

}


bool CFBWidget::setDefaultStyle(QWidget *pWidget, const QString& qssPath)
{
    bool bSetOk = false;
    QFile styleSheet(qssPath);
    if(pWidget && styleSheet.open(QIODevice::ReadOnly))
    {
        pWidget->setStyleSheet(styleSheet.readAll());

        bSetOk = true;
    }

    return bSetOk;
}


void CFBWidget::paintEvent(QPaintEvent *)
{
    paintEvent(this);
}

void CFBWidget::paintEvent(QWidget *pWidget)
{
    if(pWidget)
    {
        QStyleOption opt;
        opt.init(pWidget);
        QPainter p(pWidget);
        pWidget->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, pWidget);
    }
}




void CFBWidget::CenterWidget(const QWidget * const pParent, QWidget * const pChild)
{
    if(!pChild)
    {
        return;
    }

    QPoint posCenter;
    if(pParent)
    {
        posCenter.setX(pParent->width()/2);
        posCenter.setY(pParent->height()/2);
    }
    else
    {
        QDesktopWidget* pDeskTop = QApplication::desktop();
        posCenter.setX(pDeskTop->width()/2);
        posCenter.setY(pDeskTop->height()/2);
    }

    const int nChildWidth = pChild->width();
    const int nChildHeight = pChild->height();

    QPoint posMove;
    posMove.setX(posCenter.x() - nChildWidth/2);
    posMove.setY(posCenter.y() - nChildHeight/2);

    pChild->move(posMove);
}

void CFBWidget::setEnableMove(bool bEnableMove)
{
    m_bEnableMove = bEnableMove;
}

void CFBWidget::setTitleBarHeight(const int &titleBarHeight)
{
    if(0 <= titleBarHeight)
    {
        m_nTitleBarHeight = titleBarHeight;
    }
}

void CFBWidget::applyCFBWinFlags(QWidget *pWidget)
{
    if(pWidget)
    {
        pWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        pWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    }
}

void CFBWidget::execByAppModal(QWidget *pWidget)
{
    if(pWidget)
    {
        applyCFBWinFlags(pWidget);
        pWidget->setWindowModality(Qt::ApplicationModal);
        pWidget->show();
    }
}

void CFBWidget::showAlwayOnTop(QWidget *pWidget)
{
    if(pWidget)
    {
        CFBWidget::applyCFBWinFlags(pWidget);

        pWidget->setWindowFlags(pWidget->windowFlags()| Qt::WindowStaysOnTopHint);
        pWidget->show();
    }
}

void CFBWidget::mousePressEvent(QMouseEvent *event)
{
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

void CFBWidget::mouseMoveEvent(QMouseEvent *event)
{
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

void CFBWidget::mouseReleaseEvent(QMouseEvent * )
{
    m_bLeftDown = false;
}
