
#ifndef UIMAINLAYERLAYOUT_H
#define UIMAINLAYERLAYOUT_H

#include "motioneditor_global.h"
#include <QWidget>
#include "LayerUI/iActionLayerManager.h"
#include "LayerUI/ILayerManager.h"
#include "CFBMotionEditor.h"
#include "LayerUI/ILayerOptionHandler.h"
#include "LayerUI/ActionBlockData.h"
#include "ICFBRobot3DView.h"
#include <QTableWidget>
#include <QListWidget>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>


namespace Ui {
class COptionsMotionWidget;
}

class CActionBlockGroupManager;
class MOTIONEDITOR_EXPORT COptionsMotionWidget : public QWidget, public ILayerOptionHandler
{
    Q_OBJECT

public:
    explicit COptionsMotionWidget(QWidget *parent = 0 , emActionLayerType eActionType = ACTION_LAYER_MOTION);
    //    virtual void paintEvent(QPaintEvent *event);
    ~COptionsMotionWidget();
protected:
    bool eventFilter(QObject *obj, QEvent *event);

    void keyPressEvent(QKeyEvent *event);
    //public:
    //    virtual void mousePressEvent(QMouseEvent *event);
public:
    // ILayerOptionHandler接口实现
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget* GetWidget();
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID, int nOffset = 0);
    virtual void Set3DRobotViewInterface(ICFBRobot3DView* pRobotView);
    virtual void SetControlState(bool controlState, int nLayerType = 1);
    virtual void InsertGroupByMenu(int nCol);

    //hels 动作播放停止或者结束
    virtual void MotionPlayStop();
//    virtual void FinishPlayingMusic();
//    virtual void MusicLoaded(const QString& title, int time);//Motion模块更新名称和时长,时间单位为s
//    virtual void UpdateMusicPosition(quint64 position);//音乐播放的位置时间单位为ms

private:
    int GetCurrenGroupManagerID();
    void SetShowID(int *pID, int nCount);
    
    int AddToTableWidget(char* pData, int nLen, QString BlockType, int nRunTime, int nStopTime, int ActionBlockID);

    
    void UpdateTableWidgetShow(int nRow, CActionBlockData* pActionBlock);
    bool IsFullRowSelect(int nRow);
    bool IsTableHasSelectRow();
    // 显示选中的ID
    void ShowSelectRow(int row);
    void initTableWidget();
    void deleteSampleListItem(int nRow);
    void InsertSampleListToTableWidget(int nCurRow);
    bool CopySampleSelsctItem();

    void UpdateUIByData();
    void SetCurrentSelectItem(QListWidgetItem * item, int nIndex);
    void UpdataGridByGroupManager(CActionBlockGroupManager* pGroup);
    void initWidget();
    void onMotionCopy();
    void onMotionPaste();
    void onMotionInsert();
    void onMotionMirror();
//    int  getSelectRowCount();
    int  getFirstSelectRowID();
    MOTOR_DATA* GetMotorData(char* pData, int nLen, int nID);
    void SetMotorAngle(char* pData, int nLen, int nID, int nAngle);
    void ModleView(int nActionBlockID , int nOffset = 0);
    
    void GetRunTimeAndStopTime(int& nRunTime, int& nStopTime);

    
//    int GetCurMotionNodeIDFromFile();


signals:
    void SigMotionPlayStop();
    void SigSetControlState(bool controlState, int nLayerType);

private slots:

    // 响应行号双击事件
    void onSectionDoubleClicked(int logicalIndex);

    void slotMoveCurvesChange(int nCurveModel);

    //hels 响应编辑Item信号
    void onItemCommitData(QWidget * editor);

    void OnMotionPlayStop();
    void OnSetControlState(bool controlState, int nLayerType);

    void on_pbAdd_clicked();

    void on_pbInsert_clicked();

    void on_pbModify_clicked();

    void onMotionDelete();

    void onBatchModify();

//    void on_btnAddGroup_clicked();

    //    void on_m_ListActionGroup_clicked(const QModelIndex &index);

    //    void on_m_ListActionGroup_itemClicked(QListWidgetItem *item);

    //void on_btnDeleteGroup_clicked();

    void on_m_ListActionGroup_itemPressed(QListWidgetItem *item);

    void on_twActionList_itemPressed(QTableWidgetItem *item);

    void on_btnUnitedSimulation_clicked();

    void on_btnStop_clicked();

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:
    Ui::COptionsMotionWidget *ui;
    ILayerManager* m_pMainLayer;
    ILayerOptionHandler_Event* m_pEvent;
    int m_nCurrentGroupListSelectItem;
    int m_nCurrentGroupID;
    vector<int> m_ShowId;
    CActionBlockDataManager* m_pGroupManager;
    ICFBRobot3DView* m_pRobotView;
    QTableWidget * m_pGridTable;

    //    QList<QTableWidgetItem*>m_pItemList;
    QListWidget * m_pGroupListWidget;

    bool IsCompensatory;
    void CreateGroupListPopMenuActions();
    void CreateGridListPopMenuActions();
    void onGroupListDelete();
    bool m_bIsTotalPlaying;//联合仿真
    QString m_strTotalTime;
    int m_nTotalTime;

    void onGroupModifyName();
    void onGroupListCopy();
    void onGroupListPaste();

    int   m_nRunTimeCol;    // 运行时间列索引
    int   m_nStopTimeCol;   // 停止时间列索引
    int   m_nMoveCurvesCol; // 运动曲线列索引

    emActionLayerType m_eActionType;    //全动作、头动作、手动作、腿动作

//    int m_nCurMotionNodeID;

};


#endif // UIMAINLAYERLAYOUT_H
