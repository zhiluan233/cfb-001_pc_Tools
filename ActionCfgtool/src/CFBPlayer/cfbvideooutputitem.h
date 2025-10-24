

#ifndef CFBVIDEOOUTPUTWIDGET_H
#define CFBVIDEOOUTPUTWIDGET_H

#include <QVideoWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QAbstractVideoSurface>
#include <QGraphicsItem>

class CFBVideoOutputItem : public QAbstractVideoSurface, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit CFBVideoOutputItem(QGraphicsItem *parentItem = 0);
    ~CFBVideoOutputItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    //video surface
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;

    bool start(const QVideoSurfaceFormat &format) override;
    void stop() override;
    bool present(const QVideoFrame &frame) override;

signals:
    
    void sigOnMousePressed();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) override;

private:
    QImage::Format imageFormat;
    QSize imageSize;

    QVideoFrame currentFrame;
    bool framePainted;

};

#endif // CFBVIDEOOUTPUTWIDGET_H
