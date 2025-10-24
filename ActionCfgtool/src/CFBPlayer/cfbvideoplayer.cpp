

#include <QVBoxLayout>
#include <QFile>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "cfbvideoplayer.h"
#include "ui_cfbvideoplayer.h"
#include "configs.h"
#include "videocapture.h"

CFBVideoPlayer::CFBVideoPlayer(QWidget *parent) :
    QWidget(parent),
    m_pMediaPlayer(NULL),
    m_pVideoGraphicsItem(NULL),
    m_pVideoPlayContrlWidget(NULL),
    m_pGraphicsScene(NULL),
    m_pGraphicsView(NULL),
    m_pGraphPixmapPlayItem(NULL),
    m_pGraphpixmapPreviewFrameItem(NULL),
    m_pGraphpixmapDescImageItem(NULL),
    m_bIsPlayVideo(true),
    ui(new Ui::CFBVideoPlayer)
{
    ui->setupUi(this);

    setUpUI();
    init();

    setDefaultStyle();
}

CFBVideoPlayer::~CFBVideoPlayer()
{
    if (m_pMediaPlayer->state() != QMediaPlayer::StoppedState)
    {
        m_pMediaPlayer->stop();
    }

    delete m_pMediaPlayer;
    delete ui;
}


bool CFBVideoPlayer::loadVideoFile(const QString &strFile)
{
    if (!m_pMediaPlayer)
    {
        return false;
    }

    showVideoControl(true);

    //AlphaRobotLog::getInstance()->printDebug(QString("Start load video file: %1").arg(strFile));

    // 先添加播放按钮，在捕获视频第一帧画面时界面就不会太单调
   addPlayItem();

    const QString strCaptureImgPath = CConfigs::getLocalTempPath(VIDEO_CAPTURE_PREVIEW_FRAME);
    CVideoCapture videoCapture;
    if (videoCapture.captureFrame(strFile, strCaptureImgPath, VIDEO_CAPTURE_FRAME_INDEX, CFBPlayerConstants::CAPTURE_VIDEO_FRAME_TIMEOUT))
    {
        addVideoPreviewFrameItem();
    }

    //AlphaRobotLog::getInstance()->printDebug("Finish capture video frame");

    m_pMediaPlayer->setMedia(QUrl(strFile));

//    emit sigOnVideoLoaded();
    return true;
}

bool CFBVideoPlayer::setImage(const QString &strImageFile)
{
    resetVideoPlayer();

    showVideoControl(false);

    // 由于初始化时UI还没有显示出来，默认给的scene rect的值
    // 不一定合适，因此在UI显示后需要重新刷新scene rect
    updateSceneRect();

    QPixmap pixmapImageItem;
    pixmapImageItem.fill(Qt::transparent);
    if (!QFile::exists(strImageFile))
    {
        return false;
    }

    if (pixmapImageItem.load(strImageFile))
    {
        int nWidth = (int)(m_pGraphicsScene->sceneRect().width());
        int nHeight = (int) (m_pGraphicsScene->sceneRect().height());
        QPixmap pixmapScaled = pixmapImageItem.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_pGraphpixmapDescImageItem = m_pGraphicsScene->addPixmap(pixmapScaled);
        m_pGraphpixmapDescImageItem->setPos(m_pGraphicsScene->sceneRect().topLeft());
    }

    repaint();

    return true;
}


void CFBVideoPlayer::resetVideoPlayer()
{
    const QString strCaptureImgPath = CConfigs::getLocalTempPath(VIDEO_CAPTURE_PREVIEW_FRAME);
    if(QFile::exists(strCaptureImgPath))
    {
        QFile::remove(strCaptureImgPath);
    }

    // 清除上次数据
    removePlayItem();
    removeVideoPreviewFrame();
    removeDescImageItem();

//    // 先清除原来的图片
//    m_pGraphicsScene->clear();

    if (m_pMediaPlayer)
    {
        m_pMediaPlayer->setMedia(QMediaContent());
    }

    if (m_pVideoPlayContrlWidget)
    {
        m_pVideoPlayContrlWidget->reset();
    }
}

void CFBVideoPlayer::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 通过在painEvent中实时设置场景的sceneRect，解决了视频界面
    // 根据播放器大小自适应调整画面比例和位置的问题
    updateSceneRect();

    // 根据场景Rect调整播放按钮、视频预览图片
    updatePlayItem();
    updateVideoPreviewFrameItem();
}

void CFBVideoPlayer::hideEvent(QHideEvent *event)
{
    if (m_pMediaPlayer)
    {
        m_pMediaPlayer->stop();
    }

    QWidget::hideEvent(event);
}

void CFBVideoPlayer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}


void CFBVideoPlayer::slotOnPlay()
{
    qDebug() << "Play: current state: " << m_pMediaPlayer->state() << endl;
    if (m_pMediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        // 对于Mac系统，文件播放结束不会触发EndofMedia状态，导致人为的
        // 在视频文件播放到距离视频解决还有MEIDA_PLAY_END_OFF_SET时
        // 就人为视频播放结束，播放按钮可用，此时如果点击播放按钮，应该从
        // 头开始播放视频
#ifdef Q_OS_MAC
        if (m_pMediaPlayer->position() >= (m_pMediaPlayer->duration() - MEIDA_PLAY_END_OFF_SET))
        {
            m_pMediaPlayer->stop();
            m_pMediaPlayer->play();
        }
#endif
    }
    else
    {
        m_pMediaPlayer->play();
    }
}


void CFBVideoPlayer::slotOnPause()
{
    m_pMediaPlayer->pause();
}


void CFBVideoPlayer::slotOnStop()
{
    if (m_pMediaPlayer->state() != QMediaPlayer::StoppedState)
    {
        m_pMediaPlayer->stop();
    }
}


void CFBVideoPlayer::slotOnVolumeChanged(int nValue)
{
    if (!m_pMediaPlayer)
    {
        return;
    }

    m_pMediaPlayer->setVolume(nValue);
}


void CFBVideoPlayer::slotOnUpdatePlayTime(quint64 nValue)
{
    if (m_pMediaPlayer->isSeekable())
    {
        m_pMediaPlayer->setPosition(nValue);
    }
}


void CFBVideoPlayer::slotOnVideoPlayStateChanged(QMediaPlayer::State newState)
{
    ////////////////////////////Log信息///////////////////////
    qDebug() << "Current Play State: " << newState << endl;
    //AlphaRobotLog::getInstance()->printDebug(QString("Current Play State: %1").arg(getPlayState(newState)));
    ////////////////////////////Log信息///////////////////////

    if (m_pVideoPlayContrlWidget)
    {
        m_pVideoPlayContrlWidget->doOnPlayStateChanged(newState);
    }

    // 如果是停止状态，则显示视频预览帧
    // 否则隐藏视频预览帧
    if (newState == QMediaPlayer::StoppedState)
    {
        addVideoPreviewFrameItem();
    }
    else
    {
        removeVideoPreviewFrame();
    }

    // 如果是非播放状态，需要显示播放按钮；
    // 如果是播放状态，需要移除播放按钮
    if (newState != QMediaPlayer::PlayingState)
    {
       addPlayItem();
    }
    else
    {
        removePlayItem();
    }
}


void CFBVideoPlayer::slotOnMediaStatusChanged(QMediaPlayer::MediaStatus newStatus)
{
    ////////////////////////////Log信息///////////////////////
    qDebug() << "Current Media Status: " << newStatus << endl;
    //AlphaRobotLog::getInstance()->printDebug(QString("Current Media Status: %1").arg(getMediaStatus(newStatus)));
    ////////////////////////////Log信息///////////////////////

    // 由于在mac系统下获取不到缓冲的进度，因此只能通过
    // 媒体加载状态来提示信息
#ifdef Q_OS_MAC
    if (newStatus == QMediaPlayer::LoadingMedia || newStatus == QMediaPlayer::StalledMedia)
    {
         ui->labelBuffering->setText(tr("Buffering"));
    }
    else
    {
        ui->labelBuffering->setText("");
    }
#endif

    if (newStatus == QMediaPlayer::LoadedMedia)
    {
        emit sigOnVideoLoaded();
    }

    if (m_pVideoPlayContrlWidget)
    {
        m_pVideoPlayContrlWidget->doOnMediaStatusChanged(newStatus);
    }
}


void CFBVideoPlayer::slotOnBufferStatusChanged(int nPercentFilled)
{
    if (!m_pMediaPlayer)
    {
        return;
    }

    // 正在播放
    if (m_pMediaPlayer->mediaStatus() != QMediaPlayer::StalledMedia)
    {
        ui->labelBuffering->setText("");
    }
    else  // 正在缓冲
    {
        ui->labelBuffering->setText(tr("Buffering") + QString(" %1%").arg(nPercentFilled));
    }
}


void CFBVideoPlayer::slotOnMousePressed()
{
    if (!m_pMediaPlayer->isVideoAvailable())
    {
        return;
    }

    if (m_pMediaPlayer->state() != QMediaPlayer::PlayingState) {
        m_pMediaPlayer->play();
    }
    else
    {
        m_pMediaPlayer->pause();
    }
}


void CFBVideoPlayer::setUpUI()
{
    m_pMediaPlayer = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    m_pMediaPlayer->setNotifyInterval(CFBPlayerConstants::MEDIA_PLAYER_NOTIFY_INTERNAL);
    m_pVideoGraphicsItem = new CFBVideoOutputItem;

    m_pGraphicsScene = new QGraphicsScene(this);
    m_pGraphicsView = new QGraphicsView(m_pGraphicsScene, this);
    m_pGraphicsView->setAlignment(Qt::AlignCenter);
    m_pGraphicsView->setBackgroundBrush(QBrush(Qt::black));
    m_pGraphicsView->setCacheMode(QGraphicsView::CacheBackground);
    m_pGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_pGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGraphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
    m_pGraphicsView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    m_pGraphicsScene->addItem(m_pVideoGraphicsItem);

    connect(m_pVideoGraphicsItem, &CFBVideoOutputItem::sigOnMousePressed, this, &CFBVideoPlayer::slotOnMousePressed);
    m_pMediaPlayer->setVideoOutput(m_pVideoGraphicsItem);
    ui->horizontalLayoutVideoOutput->addWidget(m_pGraphicsView);

    m_pVideoPlayContrlWidget = new VideoPlayerContrlWidget(this);
    ui->verticalLayoutContrl->addWidget(m_pVideoPlayContrlWidget);

    connect(m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::sigOnPlay, this, &CFBVideoPlayer::slotOnPlay);
    connect(m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::sigOnPause, this, &CFBVideoPlayer::slotOnPause);
    connect(m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::sigOnStop, this, &CFBVideoPlayer::slotOnStop);
    connect(m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::sigOnVolumeChanged, this, &CFBVideoPlayer::slotOnVolumeChanged);
    connect(m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::sigOnUpdatePlayTime, this, &CFBVideoPlayer::slotOnUpdatePlayTime);

    connect(m_pMediaPlayer, &QMediaPlayer::durationChanged, m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::slotOnDurationChanged);
    connect(m_pMediaPlayer, &QMediaPlayer::positionChanged, m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::slotOnPositionChanged);
    connect(m_pMediaPlayer, &QMediaPlayer::stateChanged, this, &CFBVideoPlayer::slotOnVideoPlayStateChanged);
    connect(m_pMediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &CFBVideoPlayer::slotOnMediaStatusChanged); // !!!!@@@@Qt在Mac存在Bug，在mac系统下，播放结束时并不会马上触发EndofMedia:
                                                                                                                                                                                                         // https://bugreports.qt.io/browse/QTBUG-33761
    connect(m_pMediaPlayer, &QMediaPlayer::bufferStatusChanged, this, &CFBVideoPlayer::slotOnBufferStatusChanged);  // !!!!!@@@@在mac系统下，该信号从来没有发出来过@@@@!!!!!!

    connect(this, &CFBVideoPlayer::sigOnVideoLoaded, m_pVideoPlayContrlWidget, &VideoPlayerContrlWidget::slotOnVideoLoaded);
}


void CFBVideoPlayer::init()
{
    ui->labelBuffering->setText("");

    updateSceneRect();
}


void CFBVideoPlayer::updateSceneRect()
{
    QWidget *pViewPort = m_pGraphicsView->viewport();
    int nVideoOutputItemHeight = this->height() - m_pVideoPlayContrlWidget->height() - ui->labelBuffering->height();

    // 如果显示图片，则隐藏播放器控制控件和缓存按钮
    if (!m_bIsPlayVideo)
    {
        nVideoOutputItemHeight = this->height();
    }

    pViewPort->resize(this->width(), nVideoOutputItemHeight);
    m_pGraphicsScene->setSceneRect(0, 0, pViewPort->width(), pViewPort->height());

//    m_pGraphicsView->fitInView(m_pVideoGraphicsItem);
//    m_pGraphicsView->resize(pViewPort->size());
//    m_pGraphicsView->scale(CFBPlayerConstants::GRAPHVIEW_X_SCALE, CFBPlayerConstants::GRAPHVIEW_Y_SCALE); // 通过放大方式让视频、图片填满view
}


void CFBVideoPlayer::addPlayItem()
{
    // 如果已经显示出来了，就不要再重复添加了
    if (m_pGraphPixmapPlayItem)
    {
        return;
    }

    QPixmap pixmapVideoPlayBg;
    pixmapVideoPlayBg.fill(Qt::transparent);
    if (pixmapVideoPlayBg.load(":/res/images/btn_play_video.png"))
    {
        int nWidth = CFBPlayerConstants::PLAYER_BACKGROUND_PLAY_BUTTON_WIDTH/CFBPlayerConstants::GRAPHVIEW_X_SCALE;
        int nHeight = CFBPlayerConstants::PLAYER_BACKGROUND_PLAY_BUTTON_HEIGHT/CFBPlayerConstants::GRAPHVIEW_Y_SCALE;
        QPixmap pixmapScaled = pixmapVideoPlayBg.scaled(nWidth, nHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_pGraphPixmapPlayItem = m_pGraphicsScene->addPixmap(pixmapScaled);
        m_pGraphPixmapPlayItem->setTransformationMode(Qt::SmoothTransformation);

        QPointF pointTarget = m_pGraphicsScene->sceneRect().center() - QPointF(nWidth/2, nHeight/2);
        m_pGraphPixmapPlayItem->setPos(pointTarget);
    }

    adjustShowOrder();
}


void CFBVideoPlayer::updatePlayItem()
{
    // 自适应调整功能，当伸缩主窗口时，更新播放按钮位置

    // 如果没有显示出来，则不需要更新
    if (!m_pGraphPixmapPlayItem)
    {
        return;
    }

    int nWidth = CFBPlayerConstants::PLAYER_BACKGROUND_PLAY_BUTTON_WIDTH/CFBPlayerConstants::GRAPHVIEW_X_SCALE;
    int nHeight = CFBPlayerConstants::PLAYER_BACKGROUND_PLAY_BUTTON_HEIGHT/CFBPlayerConstants::GRAPHVIEW_Y_SCALE;
    QPointF pointTarget = m_pGraphicsScene->sceneRect().center() - QPointF(nWidth/2, nHeight/2);
    m_pGraphPixmapPlayItem->setPos(pointTarget);

    adjustShowOrder();
}


void CFBVideoPlayer::removePlayItem()
{
    if (m_pGraphicsScene && m_pGraphPixmapPlayItem)
    {
        m_pGraphicsScene->removeItem(m_pGraphPixmapPlayItem);
    }
    m_pGraphPixmapPlayItem = NULL;
}


QString CFBVideoPlayer::getPlayState(QMediaPlayer::State eState)
{
    QString strState = "";
    switch (eState) {
    case QMediaPlayer::StoppedState:
        strState = "QMediaPlayer::StoppedState";
        break;
    case QMediaPlayer::PlayingState:
        strState = "QMediaPlayer::PlayingState";
        break;
    case QMediaPlayer::PausedState:
        strState = "QMediaPlayer::PausedState";
        break;
    default:
        break;
    }

    return strState;
}


QString CFBVideoPlayer::getMediaStatus(QMediaPlayer::MediaStatus eStatus)
{
    QString strMediaStatus = "";
    switch (eStatus) {
    case QMediaPlayer::UnknownMediaStatus:
        strMediaStatus = "QMediaPlayer::UnknownMediaStatus";
        break;
    case QMediaPlayer::NoMedia:
        strMediaStatus = "QMediaPlayer::NoMedia";
        break;
    case QMediaPlayer::LoadingMedia:
        strMediaStatus = "QMediaPlayer::LoadingMedia";
        break;
    case QMediaPlayer::LoadedMedia:
        strMediaStatus = "QMediaPlayer::LoadedMedia";
        break;
    case QMediaPlayer::StalledMedia:
        strMediaStatus = "QMediaPlayer::StalledMedia";
        break;
    case QMediaPlayer::BufferingMedia:
        strMediaStatus = "QMediaPlayer::BufferingMedia";
        break;
    case QMediaPlayer::BufferedMedia:
        strMediaStatus = "QMediaPlayer::BufferedMedia";
        break;
    case QMediaPlayer::EndOfMedia:
        strMediaStatus = "QMediaPlayer::EndOfMedia";
        break;
    case QMediaPlayer::InvalidMedia:
        strMediaStatus = "QMediaPlayer::InvalidMedia";
        break;
    default:
        break;
    }

    return strMediaStatus;
}


void CFBVideoPlayer::addVideoPreviewFrameItem()
{
    // 如果已经显示出来了，就不要再重复添加了
    if (m_pGraphpixmapPreviewFrameItem)
    {
        return;
    }

    QPixmap pixmapVideoPreviewItem;
    pixmapVideoPreviewItem.fill(Qt::transparent);
    const QString strCaptureImgFile = CConfigs::getLocalTempPath(CFBPlayerConstants::VIDEO_CAPTURE_PREVIEW_FRAME);
    if (!QFile::exists(strCaptureImgFile))
    {
        return;
    }
    if (pixmapVideoPreviewItem.load(strCaptureImgFile))
    {
        int nWidth = (int)(m_pGraphicsScene->sceneRect().width());
        int nHeight = (int) (m_pGraphicsScene->sceneRect().height());
        QPixmap pixmapScaled = pixmapVideoPreviewItem.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_pGraphpixmapPreviewFrameItem = m_pGraphicsScene->addPixmap(pixmapScaled);
        m_pGraphpixmapPreviewFrameItem->setPos(m_pGraphicsScene->sceneRect().topLeft());
    }

    adjustShowOrder();
}


void CFBVideoPlayer::updateVideoPreviewFrameItem()
{
    // 自适应调整功能，当伸缩主窗口时，更新视频预览图片的大小和位置

    // 如果没有显示出来，则不需要更新
    if (!m_pGraphpixmapPreviewFrameItem)
    {
        return;
    }

    QPixmap pixmapVideoPreviewItem;
    pixmapVideoPreviewItem.fill(Qt::transparent);
    const QString strCaptureImgFile = CConfigs::getLocalTempPath(CFBPlayerConstants::VIDEO_CAPTURE_PREVIEW_FRAME);
    if (!QFile::exists(strCaptureImgFile))
    {
        return;
    }
    if (pixmapVideoPreviewItem.load(strCaptureImgFile))
    {
        int nWidth = (int)(m_pGraphicsScene->sceneRect().width());
        int nHeight = (int) (m_pGraphicsScene->sceneRect().height());
        QPixmap pixmapScaled = pixmapVideoPreviewItem.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_pGraphpixmapPreviewFrameItem->setPixmap(pixmapScaled);
        m_pGraphpixmapPreviewFrameItem->setPos(m_pGraphicsScene->sceneRect().topLeft());
    }

    adjustShowOrder();
}


void CFBVideoPlayer::removeVideoPreviewFrame()
{
    if (m_pGraphicsScene && m_pGraphpixmapPreviewFrameItem)
    {
        m_pGraphicsScene->removeItem(m_pGraphpixmapPreviewFrameItem);
    }
    m_pGraphpixmapPreviewFrameItem = NULL;
}

void CFBVideoPlayer::removeDescImageItem()
{
    if (m_pGraphicsScene && m_pGraphpixmapDescImageItem)
    {
        m_pGraphicsScene->removeItem(m_pGraphpixmapDescImageItem);
    }
    m_pGraphpixmapDescImageItem = NULL;
}

void CFBVideoPlayer::showVideoControl(bool bShow)
{
    m_bIsPlayVideo = bShow;

    if (bShow)
    {
        if (m_pVideoPlayContrlWidget && m_pVideoPlayContrlWidget->isHidden())
        {
            m_pVideoPlayContrlWidget->show();
        }

        if (ui->labelBuffering && ui->labelBuffering->isHidden())
        {
            ui->labelBuffering->show();
        }
    }
    else
    {
        if (m_pVideoPlayContrlWidget && !m_pVideoPlayContrlWidget->isHidden())
        {
            m_pVideoPlayContrlWidget->hide();
        }

        if (ui->labelBuffering && !ui->labelBuffering->isHidden())
        {
            ui->labelBuffering->hide();
        }
    }
}


void CFBVideoPlayer::adjustShowOrder()
{
    if (!m_pVideoGraphicsItem)
    {
        return;
    }

    QList<QGraphicsItem *> lstCollideItems = m_pGraphicsScene->collidingItems(m_pVideoGraphicsItem, Qt::IntersectsItemShape);
    for (int id = 0; id < lstCollideItems.size(); id++)
    {
        if (lstCollideItems[id] == m_pGraphPixmapPlayItem)
        {
            // 播放按钮显示在视频预览图片上面
            if (m_pGraphpixmapPreviewFrameItem)
            {
                m_pGraphpixmapPreviewFrameItem->stackBefore(m_pGraphPixmapPlayItem);
            }
        }
        else if (lstCollideItems[id] == m_pGraphpixmapPreviewFrameItem) // 视频预览图片显示在视频播放上面
        {
            m_pVideoGraphicsItem->stackBefore(m_pGraphpixmapPreviewFrameItem);
        }
    }
}


void CFBVideoPlayer::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/cfbplayer.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}
