
#ifndef UILAYERMANAGER_H
#define UILAYERMANAGER_H

#include "motioneditor_global.h"
#include "LayerUI/IUILayerManager.h"
#include "LayerUI/IGroupDataPlay.h"
#include "LayerUI/IGroupDataManagerPlay.h"
#include "LayerUI/ILayerCommonWidget.h"
#include "stable.h"
#include "LayerUI/DialogAddLayer.h"
#include <QStyleOption>
#include <QPainter>
#include "mp3player.h"

namespace Ui {
class LayerManagerWidget;
}

class CUILayerCommonContainer;
class CLayerControlWidget;

class MOTIONEDITOR_EXPORT CUILayerManager :public QWidget, public IUILayerManager, public IGroupDataManagerPlay_Event
{
    Q_OBJECT

public:
    explicit CUILayerManager(QWidget *parent = 0);
    ~CUILayerManager();

private:
    Ui::LayerManagerWidget *ui;
    
    void initWidget();

//	ICFBRobot3DView* Get3dView(){return m_pRobot3dView;};

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    virtual void paintEvent(QPaintEvent *);
public:
    virtual bool Connect(QWidget* pParent, IUILayerManager_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget* GetWidget();
    virtual void OnDraw();
    virtual bool SetData(char* pData, int nLen);
//    virtual void AdjustData() ;
    //virtual void SetMusicSource(QString strMusicFile);
    virtual bool GetData(char** ppData, int &nLen);
    virtual QString GetLayerName(int nLayerID);
    virtual int GetCurrentSelTime();
    virtual int SetCurrentSelectCol(int nCol);
    virtual void SetActionDataGroupManagerToOptions(CActionBlockDataManager* pGroupManager, int nCurrentGroupID,int nCurrentActionID, int nOffset = 0);
    //virtual void SetActionToOptions(int nCurrentGroupID , int nCurrentActionID) ;
//    virtual void UpdateScrollBar();

    //leo add 20170228 
    virtual void TreeToView( char *pData, int nLen, emActionLayerType eType, int nCur, QString FileName );
    
    virtual bool LoadMusic(const QString& strFilePath);
    virtual void PlayEvent_PlayMp3();
    virtual void ReleaseMusic() ;
    virtual bool AddActionLayer(ILayerUI* pLayer);
    virtual bool AddAcionLayer(int nLayerID);
//	virtual void SetPosition(int PositionX, int PositionY);
    // 查找层
    virtual ILayerUI* FindLayer(int nTypeID);
    // 删除层
    virtual bool DeleteActionLayer(int nTypeID);
    // 删除所有层
    virtual void DeleteAllLayer();
    // 获取LAY数量
    virtual int GetLayerCount();
    
    virtual emActionLayerType GetLayerTypeByIndex(int nIndex);
    // 根据序号查找层
    virtual ILayerUI* FindLayerByIndex(int nIndex);
//    virtual void SetHScrollOffset(uint nPos);
//    virtual void SetHScrollPos(int nCol) ;
    virtual void Set3DRobotViewInterface(ICFBRobot3DView* pRobotView);
    virtual ICFBRobot3DView* Get3DRobotViewInterface() {return m_pRobot3dView;}
    virtual int GetNewMaxCellCount(int nMax);
    virtual void SetTreeView(ILibTreeNavigation* pView);

    virtual void SetTimeScaleValues(int timeScale);
    virtual void FreshTimeScaleValues(int timeSacle);
    virtual int GetTimeScaleValues();
    virtual int GetLineThick();
    virtual void SetLineThick(int nThick);
    virtual int GetCellWidth();
    virtual void SetCellWidth(int nWidth);
    virtual int GetCellCount();
    virtual void SetCellCount(int nCount);

    virtual int GetCurrentSelectCol();

    virtual void LayerPlaying(int layerType, ILayerOptionHandler* optionHandle);
    virtual void StopLayerDebug();
    virtual void LayerStop();
    virtual void PlayEvent_OnSetPos(int nCol);
    virtual void LayerDebugStop(int layerType);
    virtual void SetOriginPos(int nCol);

    virtual void TotalLayerPlaying();
    virtual void StopTotalLayerDebug();
    virtual void PauseTotalLayerDebug();
    virtual ILayerOptionHandler* GetOptionHandler();
    virtual void IUILayerManager_SetSocketSession(CICFBSocketSession* pSession);
    virtual void IUILayerManager_SwitchLayer(emActionLayerType nType);
    virtual void IUILayerManager_SetMusicFile(int nLayerType , void *pData , int nDataLen , int nDataID) ;
    virtual void IUILayerManager_DeleteMusicFile(int nLayerType) ;
    virtual void IUILayerManager_SetModifiedState(bool bState) ;
    virtual bool IUILayerManager_GetModifiedState() ;
    virtual bool IUILayerManager_IsRobotDebuging() ;
    virtual void IUILayerManager_SingleBlockPlaying(int nLayerType ,CActionBlockData *pActionData) ;
    virtual void IUILayerManager_SingleGroupPlaying(int nLayerType , CActionBlockGroupManager* pGroupManager);
    virtual void IUILayerManager_SingleBlockPlayingStop(int nLayerType);

//    virtual void IUILayerManager_SetMusicModifiedState(bool bState);
//    virtual bool IUILayerManager_GetMusicModifiedState();

    virtual void LayerManager_OnLButtonDown(int layerType);
    
    virtual void UpdateLayerWidget(CActionBlockDataManager* pGroupManager, int nCurrentGroupID , int nCurrentActionID, emActionLayerType nLayerType);

    
    virtual void UpdateLayerAction(int nGroupID, int nStartTime, emActionLayerType nType);
    virtual void FinishMusicPlaying();//音乐播放完成时通知
    virtual void HasMotionData(bool hasData, int nLayerType);

    //Layer层通知更新LayerManager
    
    virtual void OnLayerLButtonDown(emActionLayerType nLayerType, int nCol);

    
    virtual void OnLayerRButtonDown(emActionLayerType nLayerType, int nCol);

    virtual void OnLayerMouseMove(emActionLayerType nLayerType , int nCol);

    
    virtual void IUILayerManager_MusicHasLoaded(const QString& strMp3Path, const QString& strTitle, int nTime);

    
    virtual void OnDoMenuCommand(emActionLayerType nLayerType, int nCommond);

    //停止眼睛灯仿真
    virtual void IUILayerManager_StopPlayEyeData();
    //停止耳朵灯仿真
    virtual void IUILayerManager_StopPlayEarData();

    // 表情帧显示
    virtual void IUILayerManager_ShowExpression(QString strFileName, int nCurFrame);

public:
    bool ParseRawData(char* pData, int nLen);
//    void AdjustLayerPosition();
//    // 根据X坐标获取列号
//    int GetColIndexByPosX(int x);
//    // 根据Y坐标获取行号
//    int GetRowIndexByPosY(int y);
    void DeleteLayerData();
    
    void EnableLayerControlWidget(emActionLayerType nType, bool bShow);
    
    bool AddLayerControlWidget(emActionLayerType nType, QString strName);

    void onShowLayerClicked(emActionLayerType nType);

public slots:
    
    void onScaleSettingClicked();

    
    void onAddLayerClicked();

    
    void slotAllLayerPlay();

    
    void slotAllLayerPause();

    
    void slotAllLayerStop();

    void OnMp3PositionChange(quint64 nPosition);
    void slotSelectLineChanged(quint64 nPosition);
    void slotUpdateTimeLine(quint64 nPosition, int nType);

    void slotEarLedPlay(int RunTime, int LeftLed, int RightLed, int Bright, int LedLightUpTime, int LedLightDownTime , bool bEnd);
    void slotEyeLedPlay(int LeftLed, int RightLed, int Bright, int Color, int runTime, int LightUpTime, int LightDownTime , bool bEnd);

    void slotExpressionPlay(QString strFileName, int nFrameNum);
    
    
    void slotUnitedStop();

signals:
    void SigPositionChanged(quint64 nPosition);
    void SigStop();
    void sigUnitedStop();

private:
    //总的单元块数
    int m_CellCount;
    // 线宽
    int m_lineThick;
    // 单元格宽度
    int m_nCellWidth;

    
    void onDeleteLayerClicked(emActionLayerType nType);
    
    void onPlayLayerClicked(emActionLayerType nType);
    
    void onPauseLayerClicked(emActionLayerType nType);
    
    void onStopLayerClicked(emActionLayerType nType);

    void OnPlayMp3();

protected:
    // 时间粒度
    int m_scaleValue;
    // 仿真类型
    int m_nDebugType;
    QList<ILayerUI*> m_listActionLayer;

    IUILayerManager_Event* m_pEvent;
    IGroupDataPlay*	m_pGroupDataPlay;
    IGroupDataManagerPlay* m_ppManagerPlay;

    IGroupDataManagerPlay* m_pMotionPlay;
    IGroupDataManagerPlay* m_pEarLedPlay;
    IGroupDataManagerPlay* m_pEyeLedPlay;
    IGroupDataManagerPlay* m_pMusicPlay;

    IGroupDataManagerPlay* m_pHandPlay;
    IGroupDataManagerPlay* m_pLegPlay;
    IGroupDataManagerPlay* m_pHeadPlay;

    IGroupDataManagerPlay* m_pExpressionPlay;

    CICFBSocketSession*		m_pSocketSession;
    int m_nCurCol;
    QList<IGroupDataPlay*> m_lstGroupPlay;
    virtual void resizeEvent(QResizeEvent* event);
    CUILayerCommonContainer* m_pILayerCommonContainer;
    ILayerCommonWidget* m_pILayerCommonWidget;

private:
    ICFBRobot3DView*    m_pRobot3dView;
    ILibTreeNavigation      *m_pTreeView;
    bool	m_IsMusicPlaying;
    bool    m_IsDataPlaying;
    bool    m_bModified;
//    bool    m_bMusicModified;
    IGroupDataPlay*	m_pGroupPlay;
//	CMusicChooseDialog *m_pChooseDlg;
//    void ResetHScroll();
    //void SetHScrollOffset(int offset);
//    ILayerUI* GetLastLayer();
    void ClearSelectState();
//    void SetScrollPos(int nPos);
    void ReDrawAll();
    void DrawBackGround(QPixmap* pixmap);
    void DrawPixmapExcludeSelLine(QPixmap* pixmap);
    void DrawTimeLayer(QPixmap* pixmap);
    void setDefaultStyle();

    QPixmap* m_pPixmapMem;//hels 用于最终显示的内存图，其他的绘制最终都会贴到这张图上
    QPixmap* m_pPixmapGroup;//动作组
    QPixmap* m_pPixmapWave;//波形
    QPixmap* m_pPixmapBackGround;//hels 背景部分位图，保存刻度线等

    bool m_bIsTotalPlaying;
    bool m_bMotionPlaying;
    bool m_bEyeLedPlaying;
    bool m_bEarLedPlaying;
    bool m_bMusicPlaying;

    bool m_bHandPlaying;
    bool m_bLegPlaying;
    bool m_bHeadPlaying;

    bool m_bExpressionPlaying;

    bool m_bMusicPause;
    bool m_bHasMotionData;
    bool m_bPausePlay;
    bool m_bTotalPause;

    emActionLayerType m_nLastType;

    MP3Player* m_pMP3Player;
    CDialogAddLayer m_AddLayerDlg;
    QList<CLayerControlWidget*> m_listLayerCtrlWidget;
};

#endif // UILAYERMANAGER_H

