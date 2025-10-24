#ifndef CUILAYERIMPL_H
#define CUILAYERIMPL_H

#include "motioneditor_global.h"
#include <QObject>
#include "ActionBlockDataManager.h"
#include "ILayerCommonWidget.h"

class MOTIONEDITOR_EXPORT CUILayerImpl: public QObject
{
    Q_OBJECT
public:
    CUILayerImpl(ILayerCommonWidget *mp3Widget);
    virtual ~CUILayerImpl();
public:
    virtual void loadData();
    virtual void SetActionBlockDataManager(CActionBlockDataManager* pActionBlockDataManger);
    virtual CActionBlockDataManager* GetActionBlockDataManager();
    virtual void SetSelectGroupBlockID(int nID);
    virtual void ClearSelectGroupBlock();

    virtual void DrawGroup();
    virtual void SetDrawBlockAction(bool bDraw);

    
    virtual void DrawBase();
    //hels 绘制动作组，包含组名等
    virtual void DrawBlockGroup(QPixmap* pixmap);
    //hels 绘制选中组中Action
    virtual void DrawBlockAction(QPixmap* pixmap);
    //hels 获取选中的组中动作
    virtual void GetSelectGroupAction(int nCol, int& nGroupIndex, int& nActionIndex, int& nTimeOffset);
    virtual bool IsSelectGroupBlock(int nID);
    //获取层类型
    virtual emActionLayerType GetType(){return ACTION_LAYER_UNKNOWN;};
    
    virtual QPoint GetStartPosition();
    
    virtual void SetStartPosition(QPoint point);

    virtual void DrawBottomCell(QPixmap *pixmap);
    
    virtual QPixmap* GetPixmap();

    
    virtual int GetCellHeight();
protected:
    CActionBlockDataManager* m_pActionBlockDataManager;
    ILayerCommonWidget* m_pCUILayerCommonWidget;
    QPixmap* m_pPixmap;// 该层像素图样，供画图设备最后贴图使用
    QList<int> m_listGroupSelectID;
    bool m_bDrawBlockAction;
    int m_nCurStartTime;
    int m_nCurEndTime;
    QPoint m_startPos;
    int m_nCellHeight;//cell高度

};

#endif // CUILAYERIMPL_H
