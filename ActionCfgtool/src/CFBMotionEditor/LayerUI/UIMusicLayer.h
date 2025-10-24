

#ifndef UIMUSICLAYER_H
#define UIMUSICLAYER_H
//#include <QObject>
#include "motioneditor_global.h"
#include <QMap>
#include "ActionBlockDataManager.h"
#include "ILayerCommonWidget.h"
#include "wavelayer.h"
#include "UILayerImpl.h"

class MOTIONEDITOR_EXPORT CUIMusicLayer : public CUILayerImpl
{
    Q_OBJECT

public:
    CUIMusicLayer(ILayerCommonWidget *mp3Widget);
    ~CUIMusicLayer();

public:
    //获取层类型
    virtual emActionLayerType GetType();
    virtual void DrawGroup();
    virtual void DrawBottomCell(QPixmap *pixmap);
    
    virtual void DrawBase();

    
    virtual int GetCellHeight();

    void loadData();;
public:
    //hels 绘制动作组，包含组名等
    void DrawBlockGroup(QPixmap* pixmap);
//    //hels 绘制选中组中Action
//    void DrawBlockAction(QPixmap* pixmap);

//    void SetActionBlockDataManager(CActionBlockDataManager* pActionBlockDataManger);
//    CActionBlockDataManager* GetActionBlockDataManager();
    //void SetSelectGroupBlockCol(int nCol);
    void SetSelectGroupBlockID(int nID);
    void ClearSelectGroupBlock();
    
    bool LoadMusic(const QString& strFilePath);

    
    void ReleaseMusic(QString strFilePath);

    
    bool IsMusicLoaded(QString strFileName);
    //hels 根据TimeLevel计算1ms所占像素
    double GetXScalePerMilliSecond();

    
    void DeleteMusicFromMap(QString strFileName);

    
    void RemoveAllMusicFromMap();



signals:
        
        void SigLoadMusic(const QString& strFilePath);

        
        void SigReleaseMusic();

public slots:
    void SetDrawBlockAction(bool bDraw);

    //hels 音乐卸载完成时调用，完成相关数据删除
    void OnMusicReleased(bool sucessed);

private:
    
    void DrawWaveAction(QPixmap *pixmap, CActionBlockData *pBlockData, int nDisplayStartTime, int nDisplayEndTime);
    //hels 获取选中的组中动作
    void GetSelectGroupAction(int nCol, int& nGroupIndex, int& nActionIndex, int& nTimeOffset);
    bool IsSelectGroupBlock(int nID);
    
    int GetDrawInterval(int nSampleRate);

private:
    QMap<QString ,UIWave*> m_mapUIWave; //波形Map容器，key是音乐文件名，value是波形UI
};

#endif // UIMUSICLAYER_H
