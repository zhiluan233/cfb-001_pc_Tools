


#ifndef MP3PLAYER_H
#define MP3PLAYER_H
#include "motioneditor_global.h"
#include <QObject>
#include <QMediaPlayer>
#include "LayerUI/IUILayerManager.h"

class MOTIONEDITOR_EXPORT MP3Player : public QObject
{
    Q_OBJECT
public:
    MP3Player();
    ~MP3Player();

    
    void SetFileName(const QString mp3FileName);

    void SetLayerManager(IUILayerManager* pLayerManager);

    
    void Play(quint64 startMilliSecs, quint64 endMilliSecs);

    bool IsPlaying();
    void Pause();
    void Resume();

public slots:
    void slotFilePath(QString strFilePath);

    void slotOnPlayMp3(quint64 startMilliSecs, quint64 endMilliSecs);

    void slotStop();

    void slotRelease();

signals:
    //hels MP3播放时，通知外部播放的时间
    void SigPositionChanged(quint64 position);
    //hels 完成播放时通知外部
    void SigFinishPlaying();

private slots:
    //hels 监听QMediaPlayer信号，获取持续时间和当前播放位置等信息
    void OnDurationChanged(qint64 duration);
    void OnPositionChanged(qint64 position);
    //void OnStateChanged(QMediaPlayer::State state);
    void slotError(QMediaPlayer::Error error);

    void slotStatus(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer* m_pMediaPlayer;//可通过setPlaylist支持多个列表播放，目前暂时只有一个
    QString m_strFileName;
    IUILayerManager* m_pLayerManager;
    bool m_bPause;
    bool m_bStop;
    //播放的起止时间，若m_nEndMilliSecs不指定(默认=0)，则默认播放至结束
    quint64 m_nStartMilliSecs;
    quint64 m_nEndMilliSecs;
    quint64 m_nTotalDuration;//音乐总时长
    bool m_bIsSchedulePlay;//mp3的播放是异步的，调用play后只是Shcedule了一次播放，
                            //此时去调用SetPosition是无效的，导致mp3总是从0开始播放，因此添加记录
};

#endif // MP3PLAYER_H
