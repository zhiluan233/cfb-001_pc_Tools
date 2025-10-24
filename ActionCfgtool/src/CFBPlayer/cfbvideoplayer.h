

#ifndef CFBVIDEOPLAYER_H
#define CFBVIDEOPLAYER_H

#include <QWidget>

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

#include "cfbplayer_global.h"
#include "videoplayercontrlwidget.h"
#include "cfbplayerconst.h"
#include "cfbvideooutputitem.h"

class CVideoCapture;

using namespace CFBPlayerConstants;

namespace Ui {
class CFBVideoPlayer;
}

class CFBPLAYERSHARED_EXPORT CFBVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit CFBVideoPlayer(QWidget *parent = 0);
    ~CFBVideoPlayer();

    
    bool loadVideoFile(const QString &strFile);

    bool setImage(const QString &strImageFile);

    
    void resetVideoPlayer();

    
    QMediaPlayer *getPlayer() { return m_pMediaPlayer; }

signals:
    
    void sigOnVideoLoaded();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private slots:
    
    void slotOnPlay();

    
    void slotOnPause();

    
    void slotOnStop();

    
    void slotOnVolumeChanged(int nValue);

    
    void slotOnUpdatePlayTime(quint64 nValue);

    
    void slotOnVideoPlayStateChanged(QMediaPlayer::State newState);

    
    void slotOnMediaStatusChanged(QMediaPlayer::MediaStatus newStatus);

    
    void slotOnBufferStatusChanged(int nPercentFilled);

    
    void slotOnMousePressed();

private:
    
    void setUpUI();

    
    void init();

    
    void updateSceneRect();

    
    void addPlayItem();

    
    void updatePlayItem();

    
    void removePlayItem();

    
    QString getPlayState(QMediaPlayer::State eState);

    
    QString getMediaStatus(QMediaPlayer::MediaStatus eStatus);

    
    void addVideoPreviewFrameItem();

    
    void updateVideoPreviewFrameItem();

    
    void removeVideoPreviewFrame();

    void removeDescImageItem();

    void showVideoControl(bool bShow);

    
    void adjustShowOrder();

    
    void setDefaultStyle();

private:
    bool  m_bIsPlayVideo;  // 是否播放视频

    QMediaPlayer *m_pMediaPlayer;
     VideoPlayerContrlWidget *m_pVideoPlayContrlWidget;

    CFBVideoOutputItem *m_pVideoGraphicsItem; // 视频播放器输出item
    QGraphicsScene *m_pGraphicsScene; // 场景
    QGraphicsView *m_pGraphicsView; // 视图

    QGraphicsPixmapItem *m_pGraphPixmapPlayItem; // 视频播放item
    QGraphicsPixmapItem *m_pGraphpixmapPreviewFrameItem; // 视频预览图片帧
    QGraphicsPixmapItem *m_pGraphpixmapDescImageItem;  // 动作详情图片
private:
    Ui::CFBVideoPlayer *ui;
};

#endif // CFBVIDEOPLAYER_H
