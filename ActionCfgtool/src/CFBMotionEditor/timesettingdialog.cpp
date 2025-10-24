#include "timesettingdialog.h"
#include "ui_timesettingdialog.h"
#include <QFile>

CTimeSettingDialog::CTimeSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTimeSettingDialog)
//    m_nTimeScale(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    QFile styleSheet(":/res/qss/layerdefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

CTimeSettingDialog::~CTimeSettingDialog()
{
    delete ui;
}


void CTimeSettingDialog::SetTimeScale(int nTimeScale)
{
//    m_nTimeScale = nTimeScale;
    QString strTimeScale = QString("%1").arg(nTimeScale);
    ui->lESettingTime->setText(strTimeScale);
}


int CTimeSettingDialog::GetTimeScale()
{
      QString strTimeScale = ui->lESettingTime->text();
      int nTimeScale = strTimeScale.toInt();
      return nTimeScale;
}

void CTimeSettingDialog::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CTimeSettingDialog::on_pbClose_clicked()
{
    close();
}

void CTimeSettingDialog::on_pbCancel_clicked()
{
    done(QDialog::Rejected);
}

void CTimeSettingDialog::on_pbOK_clicked()
{
    done(QDialog::Accepted);
}
