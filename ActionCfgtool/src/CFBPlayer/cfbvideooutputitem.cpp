

#include <QPainter>
#include <QDebug>

#include "cfbplayerconst.h"
#include "configs.h"
#include "cfbvideooutputitem.h"

#include <QPainter>
#include <QTransform>
#include <QVideoSurfaceFormat>
#include <QGraphicsScene>

CFBVideoOutputItem::CFBVideoOutputItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , imageFormat(QImage::Format_Invalid)
    , framePainted(false)
{
}

CFBVideoOutputItem::~CFBVideoOutputItem()
{
}

QRectF CFBVideoOutputItem::boundingRect() const
{
//    return this->scene()->sceneRect();
    return QRectF(QPointF(0,0), QSizeF(this->scene()->sceneRect().width(), this->scene()->sceneRect().height()));
//    return QRectF(QPointF(0,0), surfaceFormat().sizeHint());
}

void CFBVideoOutputItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {

        const QTransform oldTransform = painter->transform();
        QRectF rectBounding = boundingRect();
        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -rectBounding.height());
        }

        QImage imageVideo(currentFrame.bits(), currentFrame.width(), currentFrame.height(), imageFormat);
        QImage imageScaled = imageVideo.scaled(rectBounding.width(), rectBounding.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter->drawImage(rectBounding, imageScaled);

        painter->setTransform(oldTransform);

        framePainted = true;
        currentFrame.unmap();
    }
}

QList<QVideoFrame::PixelFormat> CFBVideoOutputItem::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool CFBVideoOutputItem::start(const QVideoSurfaceFormat &format)
{
    if (isFormatSupported(format)) {
        imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
        imageSize = format.frameSize();
        framePainted = true;

        QAbstractVideoSurface::start(format);

        prepareGeometryChange();

        return true;
    } else {
        return false;
    }
}

void CFBVideoOutputItem::stop()
{
    currentFrame = QVideoFrame();
    framePainted = false;

    QAbstractVideoSurface::stop();
}

bool CFBVideoOutputItem::present(const QVideoFrame &frame)
{
    if (!framePainted) {
        if (!QAbstractVideoSurface::isActive())
            setError(StoppedError);

        return false;
    } else {
        currentFrame = frame;
        framePainted = false;

        update();

        return true;
    }
}

void CFBVideoOutputItem::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
}

void CFBVideoOutputItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
//    setFullScreen(!isFullScreen());
//    event->accept();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void CFBVideoOutputItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    emit sigOnMousePressed();
    QGraphicsItem::mousePressEvent(event);
}
