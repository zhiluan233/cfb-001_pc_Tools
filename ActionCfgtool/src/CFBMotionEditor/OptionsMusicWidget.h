#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "motioneditor_global.h"
#include <QWidget>
#include <QStyleOption>
#include <QListWidget>
#include "LayerUI/ILayerManager.h"
#include "LayerUI/IUILayerManager.h"
#include "UIMainLayerLayout.h"
#include "LayerUI/ILayerOptionHandler.h"

#define MAX_PATH_SIZE 500

namespace Ui {
class COptionsMusicWidget;
}

class MOTIONEDITOR_EXPORT COptionsMusicWidget : public QWidget, public ILayerOptionHandler
{
    Q_OBJECT

public:
    explicit COptionsMusicWidget(QWidget *parent = 0);
    ~COptionsMusicWidget();
    
    void initWidget();

private:
    
    void CopyMusicFileTo(QString strSourcePath, QString strMusicName);

    void deleteMusicFile();

public:
    //ILayerOptionHandler接口实现
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
    virtual void ReleaseConnection();
    virtual QWidget *GetWidget();
    
     virtual void MusicLoaded(const QString& strTitle, int nTime);

    
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID, int nOffset = 0);

    virtual bool IsMusicLoaded();//判断音乐是否加载，供Motion模块调用

    virtual void SetControlState(bool controlState, int nLayerType = 1);//ACTION_LAYER_MOTION		1
    virtual void InsertGroupByMenu(int nCol);
    virtual void AddGroupByMenu() {}

    //signals:
    //    //定义成信号,停止函数调用在其他线程中，因此通过信号和槽来完成
    //    void SigMotionPlayStop();
    //    void SigSetControlState(bool controlState, int nLayerType);
    //private slots:
    //    void OnMotionPlayStop();
    //    void OnSetControlState(bool controlState, int nLayerType);
    //    void OnReleaseMusic();
    void OnLoadedMusic();

    //    void OnFinishMusicPlaying();

    //    void OnLButtonDoubleClick(int nCol);
    //    void on_btnAddMusic_clicked();

    //    void on_playMP3_clicked();

    //    void on_stopMP3_clicked();

    //    void on_playMotion_clicked();

    //    void on_stopMotion_clicked();

    //    void on_btnDeleteMusic_clicked();

    //    void on_playEarLed_clicked();

    //    void on_playEyeLed_clicked();

    //    void on_stopEarLed_clicked();

    //    void on_stopEyeLed_clicked();

    int GetRawData(char **ppData);
    void SetRawData(char *pData, int nLen);
    int GetCurrenGroupManagerID();
    
    void UpdateBlockList(CActionBlockGroupManager *pGroup);
    void SetCurSelectBlockItem(int nBlockItem);
    

    
    void UpdateUIByData();

    
    void SetCurrentSelectItem(QListWidgetItem *item, int nIndex);

    
    void UpdataActionByGroupManager(CActionBlockGroupManager *pGroup);

    // 显示选中的ID
    void ShowSelectRow(int row);

    
    void UpdateBlockOptionUI(CActionBlockData* pBlockData);
private slots:
    
    void onPbChoosePathClicked();

    
    void onPbAddClicked();

    
    void onPbInsertClicked();

    
    void slotDeleteGroup();

    
    void slotDeleteLayerBlock();

    
    void onPbModifyClicked();

    void on_m_ListActionGroup_itemPressed(QListWidgetItem *item);

    void on_m_ListActionBlock_itemPressed(QListWidgetItem *item);

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
    Ui::COptionsMusicWidget *ui;
    //    CUILayerCommonContainer*  m_pMp3Container;
    ILayerOptionHandler_Event *m_pEvent;
    //    CUILayerCommonWidget* m_pCUILayerCommonWidget;
    bool m_bMotionPlaying;
    bool m_bEyeLedPlaying;
    bool m_bEarLedPlaying;
    bool m_bMusicPlaying;
    bool m_bMusicPause;
    bool m_bHasMotionData;
    QString m_strEditFilePath;
    QString m_strMusicTitle;

    int m_nCurrentGroupListSelectItem;
    int m_nCurrentGroupID;
    CActionBlockDataManager* m_pGroupManager;
    QListWidget * m_pGroupListWidget;
    QListWidget * m_pBlockListWidget;
};

#endif // MAINWIDGET_H
