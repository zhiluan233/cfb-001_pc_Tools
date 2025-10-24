


#ifndef MP3CONTAINER_H
#define MP3CONTAINER_H

#include "motioneditor_global.h"
#include <QWidget>
#include <QScrollBar>
#include <QStyleOption>
#include "UILayerCommonWidget.h"

namespace Ui {
class widgetCommonContainer;
}

class MOTIONEDITOR_EXPORT CUILayerCommonContainer : public QWidget
{
    Q_OBJECT

public:
    CUILayerCommonWidget* GetCUILayerCommonWidget()
    {
        return m_pCommonWidget;
    }

    explicit CUILayerCommonContainer(QWidget *parent = 0);
    ~CUILayerCommonContainer();

public slots:
    //hels 设置滚动条的最大最小范围，以及步进大小
    void OnRangeChanged(int min, int max, int nStep);
    //hels 音乐播放时，同步滚动条的位置
    void OnCellIndexChanged(int index);

    
    void onUpdateScaleAndRealTime(int nScaleValue, int nCol);

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    virtual void wheelEvent(QWheelEvent *event);

private:
    Ui::widgetCommonContainer *ui;

    CUILayerCommonWidget* m_pCommonWidget;//公共层窗体指针
    QScrollBar* m_pScrollbar;//滚动条
};

#endif // MP3CONTAINER_H
