
#ifndef LAYERCONTROLWIDGET_H
#define LAYERCONTROLWIDGET_H

#include "motioneditor_global.h"
#include <QWidget>
#include "stable.h"

namespace Ui {
class CLayerControlWidget;
}

class MOTIONEDITOR_EXPORT CLayerControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CLayerControlWidget(QWidget *parent, emActionLayerType nType, QString strName);
    virtual ~CLayerControlWidget();

    
    emActionLayerType GetLayerType();

    
    void InitWidget();

public slots:
    void OnStop();

signals:
    //删除layer
    void SigDeleteLayerClicked(emActionLayerType nType);
    //仿真
    void SigPlayLayerClicked(emActionLayerType nType);
    //暂停
    void SigPauseLayerClicked(emActionLayerType nType);
    //停止仿真
    void SigStopLayerClicked(emActionLayerType nType);
    //刷新仿真UI
    void sigLaberNameClicked(emActionLayerType nType);

protected:
    void paintEvent(QPaintEvent *e);

    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    
    void onDeleteLayerClicked();
    
    void onPlayLayerClicked();
    
    void onPauseLayerClicked();
    
    void onStopLayerClicked();

private:
    Ui::CLayerControlWidget *ui;
    emActionLayerType m_nType;
    QString m_strLayerName;
};

#endif // LAYERCONTROLWIDGET_H
