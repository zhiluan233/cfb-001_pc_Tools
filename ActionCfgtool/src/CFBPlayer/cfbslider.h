

#ifndef CFBSLIDER_H
#define CFBSLIDER_H

#include <QSlider>
#include <QPoint>
#include <QEvent>
#include <QMouseEvent>
#include <QStyleOptionSlider>

class CFBSlider : public QSlider
{
    Q_OBJECT
public:
    explicit CFBSlider(QWidget *parent = 0);
    ~CFBSlider();

signals:
    void sigOnEnter();
    void sigOnLeave();
    void sigOnHover(int pos, int value);

public slots:

protected:
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent *eevent);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);

    inline int pick(const QPoint &pt) const;
    int pixelPosToRangeValue(int pos) const;
    void initStyleOption_Qt430(QStyleOptionSlider *option) const;

};

#endif // CFBSLIDER_H
