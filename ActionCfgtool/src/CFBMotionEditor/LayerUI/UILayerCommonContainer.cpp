
#include "UILayerCommonContainer.h"
#include "ui_UILayerCommonContainer.h"

CUILayerCommonContainer::CUILayerCommonContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetCommonContainer)
{
    ui->setupUi(this);
    m_pCommonWidget = new CUILayerCommonWidget(this);
    m_pScrollbar = new QScrollBar(Qt::Horizontal);
    m_pScrollbar->setFixedHeight(20);
    ui->verticalLayout->addWidget(m_pCommonWidget);
    ui->SettingHLayout->addWidget(m_pScrollbar);
    connect(m_pScrollbar, &QScrollBar::valueChanged, m_pCommonWidget, &CUILayerCommonWidget::OnScrollBarValueChanged);
    connect(m_pCommonWidget, &CUILayerCommonWidget::SigRangeChanged, this, &CUILayerCommonContainer::OnRangeChanged);
    connect(m_pCommonWidget, &CUILayerCommonWidget::SigCellIndexChanged, this, &CUILayerCommonContainer::OnCellIndexChanged);
    connect(m_pCommonWidget, &CUILayerCommonWidget::SigUpdateScaleAndRealTime, this, &CUILayerCommonContainer::onUpdateScaleAndRealTime);
}

CUILayerCommonContainer::~CUILayerCommonContainer()
{
    delete ui;
}

void CUILayerCommonContainer::OnRangeChanged(int min, int max, int nStep)
{
    m_pScrollbar->setRange(min, max);
    if(m_pScrollbar->singleStep() != nStep)
    {
        m_pScrollbar->setSingleStep(nStep);
        m_pScrollbar->setPageStep(nStep);
    }
}

void CUILayerCommonContainer::OnCellIndexChanged(int index)
{
    //wanxf modify 2017/3/27  解决在Grid表中选中超出当前界面显示的动作帧时，显示不正确的问题
//    int nStep = m_pScrollbar->pageStep();
//    m_pScrollbar->setValue(index * nStep);
    m_pScrollbar->setValue(index);
}


void CUILayerCommonContainer::onUpdateScaleAndRealTime(int nScaleValue, int nCol)
{
    //更新状态
    QString strCol;
    strCol.sprintf("%d", nCol);
    ui->lECurCol->setText(strCol);

    QString strScaleValue;
    strScaleValue.sprintf("%d ms", nScaleValue);
    ui->lEScaleTime->setText(strScaleValue);

    QString strRealTime;
    strRealTime.sprintf("%0.2lf s", (double)nScaleValue * (double)nCol / 1000.0);
    ui->lERealTime->setText(strRealTime);
}

void CUILayerCommonContainer::wheelEvent(QWheelEvent *event)
{
    if(NULL != m_pScrollbar)
    {
        QWheelEvent *glevent=new QWheelEvent(event->pos(),
                                             event->globalPos(),
                                             event->pixelDelta(),
                                             event->angleDelta(),
                                             event->delta(),
                                             event->orientation(),
                                             event->buttons(),
                                             event->modifiers(),
                                             event->phase(),
                                             event->source());
        qApp->postEvent(m_pScrollbar, glevent);
    }
}
