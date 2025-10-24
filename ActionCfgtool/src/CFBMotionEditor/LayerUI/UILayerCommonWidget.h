

#ifndef MP3WIDGET_H
#define MP3WIDGET_H

#include "motioneditor_global.h"
#include <QPainter>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QThread>
#include <QDrag>
#include <QMimeData>

#include "stable.h"
#include "mp3player.h"
#include "mp3projconstants.h"
//#include "UIMotionLayer.h"
//#include "UIEarLedLayer.h"
//#include "UIEyeLedLayer.h"
//#include "UIMusicLayer.h"
#include "IActionLayerManager.h"
#include "ILayerCommonWidget.h"
#include "robotdef.h"

namespace Ui
{
class LayerCommonWidget;
}

enum PASTE_TYPE
{
    NO_PASTE = 0,
    PASTE_AFTER_COPY,
    PASTE_AFTER_CUT
};

class IUILayerManager;
class CUILayerImpl;

class MOTIONEDITOR_EXPORT CUILayerCommonWidget : public QWidget, public ILayerCommonWidget
{
    Q_OBJECT
public:
    explicit CUILayerCommonWidget(QWidget *parent = 0);
    ~CUILayerCommonWidget();


public:
    void loadData(emActionLayerType nLayerType);
    //实现ILayerCommonWidget接口
    //hels 获取显示内容的宽度
    virtual int GetContentWidth();

//    
//    virtual int GetDrawInterval(int nSampleRate);
    
    virtual void SetTimeScale(int nScale);
    
    virtual int GetTimeScale();
    
    virtual int GetLineThick();

    
    virtual int GetCurrentSelectCol();

    
    virtual void SetCurrentSelectCol( int nCol);

    //IUILayerManager_Event对应的接口调用
    virtual bool Connect(IUILayerManager* pParent, IUILayerManager_Event* pEvent);
    virtual void ReleaseConnection();
    
    virtual void UpdateLayerAction(int nGroupID, int nStartTime, emActionLayerType nType);
    
    virtual void UpdateLayerUIByCol(int nCol);

    
    virtual void UpdateLayerWidget(CActionBlockDataManager* pActionBlockDataManger, int nCurrentGroupID , int nCurrentActionID, emActionLayerType nLayerType);
    //    
    //    virtual void SetOptionHandlerEvent(ILayerOptionHandler_Event* pEvent);
    virtual bool LoadMusic(const QString& strFilePath);

    
    virtual bool MusicHasLoaded(int nTime, QString strWavePath, QString strTitle);

    
    virtual void  MusicHasReleased();

    
    virtual int GetCellWidth();

//    
//    virtual int GetCellHeight();

//    
//    virtual int GetMusicCellHeight();

    
    virtual int GetCellStartIndex();

    
    virtual int GetCellEndIndex();

    //ms为单位
    virtual long GetDisplayStartTime();
    virtual long GetDisplayEndTime();

    
    virtual QWidget* GetWidget();


    virtual void SetCellCount(int nCount);
    virtual int GetCellCount();

    //以下是针对UI层界面的更新
    
    virtual bool AddUILayer(emActionLayerType nType);

    
    virtual void DeleteAllUILayer();

    virtual CUILayerImpl *FindLayerByIndex(int nIndex);
    virtual void AdjustLayerPosition();
    virtual bool DeleteUILayerByID(int nTypeID);
    virtual CUILayerImpl *FindLayerByID(int nTypeID);
	virtual void SetPlaying(bool bState);
    
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pActionBlockDataManger, emActionLayerType nLayerType);

public:
    //    bool IsMusicLoaded();
//    void SetPlaying(bool play);
    void SetSinglePlaying(bool play);
//    
//    void initUILayer();

//    
//    bool AddUILayer(int nLayerID);

    
    CUILayerImpl* FindUILayerByLayerType(emActionLayerType nType);

    
    bool AddUILayer(CUILayerImpl *pUILayer);

signals:
    
    void SigRangeChanged(int min, int max, int nStep);

    //    
    //    void SigLoadMusic(const QString& strFilePath);

    //    
    //    void SigReleaseMusic();

    //加载成功时发送，通知主窗体设置按钮状态
    void SigNotifyLoadedMusic();
    void SigNotifyReleasedMusic();

    //hels 音乐同步时，cell索引改变时发送，通知滚动条改变位置
    void SigCellIndexChanged(int index);

    void SigLButtonDoubleClick(int nCol);

    void SigReDrawAll();
    void SigRedDrawTimeAndAction();
    //    void SigOnPlayMP3(int nStartTime, int nEndTime, int nPlayState);
    void SigFinishMusicPlaying();

    //更新时间粒度和实际时间到UI
    void SigUpdateScaleAndRealTime(int nScaleValue, int nCol);

public slots:
    //hels 滚动条位置改变时调用，重绘所有波形相关绘图（时间、波形、底部cell）
    void OnScrollBarValueChanged(int value);

    //hels 音乐播放位置更新
    void OnPositionChanged(quint64 position);

    //hels 重绘所有图形，一般发生在大小改变，或者仿真时
    //改为信号和槽方式排队触发，Motion 线程模块更新时也会调用到此函数，之前的直接调用方式会导致程序崩溃
    void ReDrawAll();
    void OnReDrawTimeAndAction();
    void OnDeleteSelectedMusic();

public:
    //hels 音乐播放时，同步时间线和波形位置等
    void UpdateTimeLineWidthMusic(quint64 position);

    //提供给仿真结束时，恢复时间选择的位置
    //void StoreCurrentX();
    //void RestoreCurrentX();

    
//    CUILayerImpl *GetLastLayer();

    
//    CUILayerImpl *GetPreLastLayer();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent* event);

    //leo add 20170228
    emActionLayerType getDropDataType(const char* pData, const int nLen);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent * event);

protected:
    //hels 绘制背景部分
    void DrawBackGround(QPixmap* pixmap);

//    
//    void DrawBottomCell(QPixmap* pixmap);

//    void DrawEarLedCell(QPixmap* pixmap);
//    void DrawEyeLedCell(QPixmap* pixmap);

    
    void DrawTimeLayer(QPixmap* pixmap);

    //绘制时间选择线,由于鼠标点击切换时间点时，不需要重绘全部内容，为提高效率，将时间线专门绘制
    void DrawTimeSelLine(QPixmap* pixmap);
    //将除时间线外的其他pixmap绘制到m_pPixmapMem中,贴图的操作，很快,主要给鼠标点击时间线时使用，其他时候用不到
    void DrawPixmapExcludeSelLine(QPixmap* pixmap);
    //创建动作组右键动作组菜单
    void CreateMotionLayerGroupListPopMenuActions();
    //创建音乐右键动作组菜单
    void CreateMusicLayerGroupListPopMenuActions();

//    void SetActStartTime(int nActStartTime);
//    void SetActEndTime(int nActEndTime);

    
    emActionLayerType GetLayerTypeByPos(QPoint point);

    
    int GetCurCellByPos(QPoint point);
private:
    enum TimeLevel
    {
        //毫秒级别
        ddd_10,
        ddd_50,
        ddd_100,
        //秒级别
        ss_1,
        ss_5,
        ss_10,
        //分钟级别
        mm_1,
        mm_5,
        mm_10
    };


//    
//    int GetCellEndIndex();

    
    QString GetTimeString(int nCurIndex);

    //hels 获取鼠标点击时，能切换时间线的有效区域
    //    QRect GetContainRect();

    //hels 鼠标中键改变enum TimeLevel，此值会影响波形绘制的精度
//    void TimeLevelChanged(TimeLevel timeLevel);

    //    //hels 初始化MP3播放
    //    void PrepareMP3();

    //    //hels 重置MP3播放
    //    void ResetMP3();

    void emitRangeChanged();

    void ClearDir(const QString& fullPath);

    //新UI如果cellcount太小，背景纯黑不好看，因此调整cellcount保证大于等于widget width
    void AdjustCellCount();

    //画动作组
    void onReDrawAction();

private slots:

    void OnCopy();
    void OnPaste();
    void OnCut();
    void OnDeleteSelectGroup();
    void OnSelectedBlockPlay();
    void OnCancelBlockPlay();
    void OnInsertBlankTime();
    void OnSaveToCommandBox();
    void OnModifyGroup();

    void on_btnZoomOut_clicked();

    void on_btnZoomIn_clicked();

private:
    Ui::LayerCommonWidget*  ui;

//    CActionBlockDataManager* m_pActionDataManager;
    int m_nCurrentGroupID;
    int m_nCurrentActionID;

    //    UIWave m_waveLayer;//hels 波形绘制对象
    MP3Player* m_pMP3Player;//hels MP3播放类对象
    //QThread* m_pMP3Thread;//hels MP3线程对象
    QString m_strMp3Path;//hels mp3路径
    QString m_strWavPath;
//    CUIMotionLayer m_actionLayer;
//    CUIEarLedLayer m_earLedLayer;
//    CUIEyeLedLayer m_eyeLedLayer;
//    CUIMusicLayer m_musicLayer;
    QList<CUILayerImpl*> m_listUILayer; //所有层UI的列表

    QPixmap* m_pPixmapMem;//hels 用于最终显示的内存图，其他的绘制最终都会贴到这张图上
//    QPixmap* m_pPixmapGroup;//动作组
//    QPixmap* m_pEarPixmapGroup;//耳朵组
//    QPixmap* m_pEyePixmapGroup;//眼睛组
//    QPixmap* m_pPixmapWave;//波形
    QPixmap* m_pPixmapBackGround;//hels 背景部分位图，保存刻度线等

    //Time 层的宽高
    int m_nTimeCellWidth;
    int m_nTimeCellHeight;

    //Music
    //宽高
    int m_nMusicCellWidth;
    int m_nMusicCellHeight;
//    int m_nPixelPerSec;//每1个cell像素大小(默认100像素)

    //底部默认cell
    int m_nCellWidth;//cell宽度
//    int m_nCellHeight;//cell高度
    int m_nCellCount;//cell总数
    int m_nLineThick;//cell线宽

    quint64 m_nCellTotalSeconds;//记录原始Cell个数对应的时间s为单位,当进行波形缩放时，需要用到此时间复原cell的正确个数

    //波形控件显示的初始位置
    int m_nStartX;
    int m_nStartY;

    int m_nSelLineLength;//hels 时间选择线的长度
    int m_nCurCol;//Joker 之前hels以像素为单位，如今改为以格为单位，具体时间要乘以时间粒度
    //hels 当前选中的时间线位置，以像素为单位，总长度为（m_nCellCount * (m_nCellWidth + m_nLineThick)）

    //hels 一个m_cellWidth + m_nLineThick即为一个cell,与TimeLevel无关
    //hels TimeLevel影响的是波形时间精度，即会影响m_nCount的大小
    int m_nCellStartIndex; //当前显示的第一个cell索引（此值与滚动条的位置有关，用于计算开始绘制的点）

    TimeLevel m_timeLevel;//显示时间精度

    //MP3起止播放时间，ms为单位
    quint64 m_nStartMilliSecs;
    quint64 m_nEndMilliSecs;

    //
    int m_timeScale;//时间粒度，由Motion模块提供

    int m_nWheelCount;//累计中键滚动2次缩放一次波形比例
    bool m_bForwards;//记录是向前滚动

    int m_bIsPlaying;
    int m_bIsSinglePlaying;
    int m_nCurChosenAction;
    int m_nPasteType;
    IUILayerManager* m_pIUILayerManager;
    double m_nCurSelXStore;
    int m_dragMode;
    //    int m_nCurActionLayer;
    IUILayerManager_Event* m_pEvent;

    //时间曾专用变量
    int m_nDivisor; //分隔基准，如果是他的倍数则画数字，否则不需要画
    int m_nNewUILayerStartY;
};

#endif // MP3WIDGET_H
