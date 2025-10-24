
#include "LayerControlWidget.h"
#include "ui_LayerControlWidget.h"

CLayerControlWidget::CLayerControlWidget(QWidget *parent, emActionLayerType nType, QString strName) :
    QWidget(parent),
    ui(new Ui::CLayerControlWidget),
    m_nType(nType),
    m_strLayerName(strName)
{
    ui->setupUi(this);
//    connect(ui->pbDeleteLayer, &QPushButton::clicked, this, &CLayerControlWidget::onDeleteLayerClicked);
    connect(ui->pbPauseLayer, &QPushButton::clicked, this, &CLayerControlWidget::onPauseLayerClicked);
    connect(ui->pbPlayLayer, &QPushButton::clicked, this, &CLayerControlWidget::onPlayLayerClicked);
    connect(ui->pbStopLayer, &QPushButton::clicked, this, &CLayerControlWidget::onStopLayerClicked);
    ui->labelLayer->installEventFilter(this);
    InitWidget();
}

CLayerControlWidget::~CLayerControlWidget()
{
    delete ui;
}


emActionLayerType CLayerControlWidget::GetLayerType()
{
    return m_nType;
}


void CLayerControlWidget::InitWidget()
{
//    if (m_nType == ACTION_LAYER_MOTION)
//    {
//        ui->pbDeleteLayer->hide();
//    }
    ui->pbPauseLayer->hide();
    ui->labelLayer->setText(m_strLayerName);
}


void CLayerControlWidget::onDeleteLayerClicked()
{
    emit SigDeleteLayerClicked(m_nType);
//    this->deleteLater();
}


void CLayerControlWidget::onPlayLayerClicked()
{
    emit SigPlayLayerClicked(m_nType);
    ui->pbPauseLayer->show();
    ui->pbPlayLayer->hide();
}


void CLayerControlWidget::onPauseLayerClicked()
{
    ui->pbPlayLayer->show();
    ui->pbPauseLayer->hide();
    emit SigPauseLayerClicked(m_nType);
}


void CLayerControlWidget::onStopLayerClicked()
{
    emit SigStopLayerClicked(m_nType);
}

void CLayerControlWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

bool CLayerControlWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (ui->labelLayer == obj)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            emit sigLaberNameClicked(m_nType);
        }
    }

    return QObject::eventFilter(obj , event);
}

void CLayerControlWidget::OnStop()
{
    ui->pbPauseLayer->hide();
    ui->pbPlayLayer->show();
}
