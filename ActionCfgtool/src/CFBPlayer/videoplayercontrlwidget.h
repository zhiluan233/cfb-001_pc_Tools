

#ifndef VIDEOPLAYERCONTRLWIDGET_H
#define VIDEOPLAYERCONTRLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMediaPlayer>

#include "cfbplayerconst.h"
#include "cfbslider.h"

class CFBVideoPlayer;

using namespace CFBPlayerConstants;

namespace Ui {
class VideoPlayerContrlWidget;
}

class VideoPlayerContrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerContrlWidget(QWidget *parent);
    ~VideoPlayerContrlWidget();

    
    void reset();

    
    void doOnPlayStateChanged(QMediaPlayer::State eState);

    
    void doOnMediaStatusChanged(QMediaPlayer::MediaStatus eStatus);

public slots:
    
    void slotOnVideoLoaded();

    
    void slotOnPositionChanged(quint64 nPos);

    
    void slotOnDurationChanged(quint64 nDuration);

private slots:
    
    void slotOnPlayClicked();

    
    void slotOnPauseClicked();

    
    void slotOnStopClicked();

    
    void slotOnTimeSliderMoved(quint64 nValue);

    
    void slotOnTimeSliderPressed();

    
    void slotOnVolumnButtonToggle(bool bChecked);

signals:
    
    void sigOnPlay();

    
    void sigOnPause();

    
    void sigOnStop();

    
    void sigOnVolumeChanged(int nValue);

    
    void sigOnUpdatePlayTime(quint64 nValue);

private:
    
    void setUpUI();

    
    void initUI();

    
    void resetButtons();

    
    void setPlayButtonState(bool bEnable);

    
    void setPauseButtonState(bool bEnable);

    
    void setStopButtonState(bool bEnable);

    
    void setButtoneState(QMediaPlayer::State eState);

    
    void setDurationPlayTime(quint64 nTime);

    
    QString getFormatPlayTime(quint64 nTime);

private:
    int m_nPlayPosNotChangedCnt; // positionChanged中播放位置一直不变的次数，此时播放卡顿，超过了指定次数需要先暂停后再播放
    quint64 m_nLastPlayPosition;  // 记录上一个postionChanged播放的位置

    QLabel *m_pEndTime;

    CFBSlider *m_pTimeSlider;
    CFBSlider *m_pVolumeSlider;

    CFBVideoPlayer *m_pParentWidget;

private:
    Ui::VideoPlayerContrlWidget *ui;
};

#endif // VIDEOPLAYERCONTRLWIDGET_H
