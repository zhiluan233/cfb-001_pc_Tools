

#include <QDebug>
#include <QTime>
#include <QCursor>

#include "videoplayercontrlwidget.h"
#include "ui_videoplayercontrlwidget.h"
#include "cfbvideoplayer.h"

VideoPlayerContrlWidget::VideoPlayerContrlWidget(QWidget *parent) :
    QWidget(parent),
    m_pParentWidget(NULL),
    m_pTimeSlider(NULL),
    m_pVolumeSlider(NULL),
    m_nLastPlayPosition(0),
    m_nPlayPosNotChangedCnt(0),
    ui(new Ui::VideoPlayerContrlWidget)
{
    ui->setupUi(this);

    m_pParentWidget = (CFBVideoPlayer *) parent;

    setUpUI();
    initUI();
}

VideoPlayerContrlWidget::~VideoPlayerContrlWidget()
{
    delete ui;
}


void VideoPlayerContrlWidget::reset()
{
    resetButtons();
    m_nLastPlayPosition = 0;
    m_nPlayPosNotChangedCnt = 0;
    setDurationPlayTime(0);
}


void VideoPlayerContrlWidget::slotOnVideoLoaded()
{
    setButtoneState(QMediaPlayer::StoppedState);
}


void VideoPlayerContrlWidget::slotOnPositionChanged(quint64 nPos)
{
    m_pTimeSlider->setValue(nPos);
    m_nLastPlayPosition = nPos;

    qDebug() << "Position changed: " << nPos << endl;
//    qDebug() << "Buffer percent: " << m_pParentWidget->getPlayer()->bufferStatus() << endl;

    if (!m_pParentWidget)
    {
        return;
    }

    QMediaPlayer *pPlayer = m_pParentWidget->getPlayer();
    if (!pPlayer)
    {
        return;
    }

    // Mac系统下停止播放后positionChanged竟然还会不断触发！！！！！
    // 你妹的
    if  (pPlayer->state() != QMediaPlayer::PlayingState)
    {
        return;
    }

    //////////////////////////////////////////////////////////////
    // 由于Qt在Mac系统下存在bug，文件播放结束后
    // 不一定会马上触发EndofMedia状态变化，因此
    // 在这里认为当当前位置大于等于视频长度-允许误差便宜值
    // 时，认为播放结束
    ////////////////////////////////////////////////////////////
#ifdef Q_OS_MAC
    quint64 nDuration = pPlayer->duration();
    if (nPos >= (nDuration - MEIDA_PLAY_END_OFF_SET))
    {
        slotOnStopClicked();
    }
    else
    {
        // 由于在Mac系统下部分视频缓冲很慢，播放的时候会很卡
        // 因此当发现播放的位置没有发生变化时，暂停播放后再开始
        // 播放，可以缓解该问题
        if ((nPos > 0) && (nPos == m_nLastPlayPosition))
        {
            // 如果连续N次位置没有变化，则先暂停后播放
            if (m_nPlayPosNotChangedCnt < CFBPlayerConstants::PLAY_POSITION_NOT_CHANGED_MAX_CNT)
            {
                m_nPlayPosNotChangedCnt++;
            }
            else
            {
                pPlayer->pause();
                pPlayer->play();
                m_nPlayPosNotChangedCnt = 0;
                qDebug() << "Video player pause and play" << endl;
            }
        }
        else
        {
            m_nPlayPosNotChangedCnt = 0;
        }
    }

#endif
}


void VideoPlayerContrlWidget::slotOnDurationChanged(quint64 nDuration)
{
    qDebug() << "Duration changed: " << nDuration << endl;
    setDurationPlayTime(nDuration);
    m_pTimeSlider->setRange(0, nDuration);
}


void VideoPlayerContrlWidget::doOnPlayStateChanged(QMediaPlayer::State eState)
{
    setButtoneState(eState);
}


void VideoPlayerContrlWidget::doOnMediaStatusChanged(QMediaPlayer::MediaStatus eStatus)
{
    switch (eStatus) {
    case QMediaPlayer::LoadingMedia:
    {
        break;
    }
    case QMediaPlayer::LoadedMedia:
    {
#ifdef Q_OS_MAC
        // Mac系统下反复加载同一个视频文件时，不会触发durationChanged()信号
        // 导致视频时长不会设置
        QMediaPlayer *pPlayer = m_pParentWidget->getPlayer();
        if (pPlayer)
        {
            slotOnDurationChanged(pPlayer->duration());
        }
#endif
        break;
    }
    case QMediaPlayer::StalledMedia:
    {
        break;
    }
    case QMediaPlayer::BufferingMedia:
    {
        break;
    }
    case QMediaPlayer::BufferedMedia:
    {
        break;
    }
    case QMediaPlayer::EndOfMedia:
    {
//        setPlayButtonState(true);
//        setStopButtonState(true);

//        ui->pbVideoPause->hide();
//        ui->pbVideoPlay->show();
        slotOnStopClicked();
        break;
    }
    default:
        break;
    }
}


void VideoPlayerContrlWidget::slotOnPlayClicked()
{
//    setButtoneState(eStateVideoPlayerPlay);
    emit sigOnPlay();
}


void VideoPlayerContrlWidget::slotOnPauseClicked()
{
//    setButtoneState(eStateVideoPlayerPause);
    emit sigOnPause();
}


void VideoPlayerContrlWidget::slotOnStopClicked()
{
//    setButtoneState(eStateVideoPlayerStop);
    m_pTimeSlider->setValue(0);
    emit sigOnStop();
}


void VideoPlayerContrlWidget::slotOnTimeSliderMoved(quint64 nValue)
{
    emit sigOnUpdatePlayTime(nValue);
}


void VideoPlayerContrlWidget::slotOnTimeSliderPressed()
{
    // emit sigOnUpdatePlayTime(ui->horizontalTimeSlider->value());
}


void VideoPlayerContrlWidget::slotOnVolumnButtonToggle(bool bChecked)
{
    if (!m_pParentWidget)
    {
        return;
    }

    QMediaPlayer *pPlayer = m_pParentWidget->getPlayer();
    if (!pPlayer)
    {
        return;
    }

    pPlayer->setMuted(bChecked);

    if (bChecked)
    {
        ui->tbVolume->setStyleSheet("background-color: transparent;"
                                                            "qproperty-icon: url(:/res/images/icon_volume_mute.png);"
                                                            "qproperty-iconSize: 24px 24px;");
    }
    else
    {
        ui->tbVolume->setStyleSheet("background-color: transparent;"
                                                            "qproperty-icon: url(:/res/images/icon_changed_volume.png);"
                                                            "qproperty-iconSize: 24px 24px;");
    }
}


void VideoPlayerContrlWidget::setUpUI()
{
    // 播放进度条
    m_pTimeSlider = new CFBSlider(this);
    m_pTimeSlider->setOrientation(Qt::Horizontal);
    ui->horizontalLayoutTimeProgress->addWidget(m_pTimeSlider);

    // 视频时长
    m_pEndTime = new QLabel(this);
    m_pEndTime->setObjectName("labelVideoPlayEndTime");
    ui->horizontalLayoutTimeProgress->addWidget(m_pEndTime);

    // 声音调节进度条
    m_pVolumeSlider = new CFBSlider(this);
    m_pVolumeSlider->setOrientation(Qt::Horizontal);
    ui->horizontalLayoutVoice->addWidget(m_pVolumeSlider);

    ui->tbVolume->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->tbVolume->setAutoRaise(true);
    ui->tbVolume->setCheckable(true);

    connect(ui->tbVolume, &QToolButton::toggled, this, &VideoPlayerContrlWidget::slotOnVolumnButtonToggle);

    connect(ui->pbVideoPlay, &QPushButton::clicked, this, &VideoPlayerContrlWidget::slotOnPlayClicked);
    connect(ui->pbVideoPause, &QPushButton::clicked, this, &VideoPlayerContrlWidget::slotOnPauseClicked);
    connect(ui->pbVideoStop, &QPushButton::clicked, this, &VideoPlayerContrlWidget::slotOnStopClicked);

    connect(m_pVolumeSlider, &QSlider::valueChanged, this, &VideoPlayerContrlWidget::sigOnVolumeChanged);

    connect(m_pTimeSlider, &QSlider::sliderMoved, this, &VideoPlayerContrlWidget::slotOnTimeSliderMoved);
    connect(m_pTimeSlider, &QSlider::sliderPressed, this, &VideoPlayerContrlWidget::slotOnTimeSliderPressed);

}


void VideoPlayerContrlWidget::initUI()
{
    reset();

    ui->tbVolume->setChecked(false);
    ui->tbVolume->setStyleSheet("background-color: transparent;"
                                                        "qproperty-icon: url(:/res/images/icon_changed_volume.png);"
                                                        "qproperty-iconSize: 24px 24px;");

    m_pVolumeSlider->setRange(0, PLAYER_MAX_VOLUME);
    m_pVolumeSlider->setValue(PLAYER_MAX_VOLUME/2);
}


void VideoPlayerContrlWidget::resetButtons()
{
    ui->pbVideoPause->hide();
    setPlayButtonState(false);
    setPauseButtonState(false);
    setStopButtonState(false);
}


void VideoPlayerContrlWidget::setPlayButtonState(bool bEnable)
{
    ui->pbVideoPlay->setEnabled(bEnable);
}


void VideoPlayerContrlWidget::setPauseButtonState(bool bEnable)
{
    ui->pbVideoPause->setEnabled(bEnable);
}


void VideoPlayerContrlWidget::setStopButtonState(bool bEnable)
{
    ui->pbVideoStop->setEnabled(bEnable);
}


void VideoPlayerContrlWidget::setButtoneState(QMediaPlayer::State eState)
{
    switch (eState) {
    case QMediaPlayer::PlayingState:
    {
        setPauseButtonState(true);
        setStopButtonState(true);
        setPlayButtonState(false);

        ui->pbVideoPlay->hide();
        ui->pbVideoPause->show();
        break;
    }
    case QMediaPlayer::PausedState:
    {
        setPlayButtonState(true);
        setPauseButtonState(false);
        setStopButtonState(true);

        ui->pbVideoPause->hide();
        ui->pbVideoPlay->show();
        break;
    }
    case QMediaPlayer::StoppedState:
    {
        setPlayButtonState(true);
        setStopButtonState(false);

        ui->pbVideoPause->hide();
        ui->pbVideoPlay->show();

        m_nLastPlayPosition = 0;
        m_nPlayPosNotChangedCnt = 0;
        break;
    }
    default:
        break;
    }
}


void VideoPlayerContrlWidget::setDurationPlayTime(quint64 nTime)
{
    m_pEndTime->setText(getFormatPlayTime(nTime));
}


QString VideoPlayerContrlWidget::getFormatPlayTime(quint64 nTime)
{
     return (QTime(0, 0, 0).addMSecs(nTime).toString(QString::fromLatin1("HH:mm:ss")));
}
