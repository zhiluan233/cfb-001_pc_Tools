
#ifndef CFBEVENTEDITORWIDGET_H
#define CFBEVENTEDITORWIDGET_H

#include <QWidget>
#include "cfbeventeditor.h"
#include <vector>
using namespace std;

namespace Ui {
class CFBEventEditorWidget;
}

//监听事件数据
typedef struct tagLISTENEVENTDATA
{
    emListenEventType eListenEventType;//监听类型
    int  nListenDistance;  //距离
    QByteArray bytesFlowData; //流程图数据

    tagLISTENEVENTDATA(emListenEventType eListenType, int nDistance)
    {
        eListenEventType = eListenType;
        nListenDistance = nDistance;
    }

}LISTENEVENTDATA;

typedef vector<LISTENEVENTDATA*>  VecListenEventType; //监听事件列表类型

class CFBEventEditorWidget : public QWidget, public ICFBEventEditor
{
    Q_OBJECT

public:
    explicit CFBEventEditorWidget(QWidget *parent = 0);
    ~CFBEventEditorWidget();

    
    virtual bool Connect(QWidget* pWnd, ICFBEventEditorNotify* pNotify);

    
    virtual void ReleaseConnection();

    
    virtual QWidget* GetWidget();

    
    virtual void GetData(char** ppData, int& nLen);

    
    virtual void SetData(char* pData, int nLen);

//    //设置流程图
//    virtual void setFlowControl(ICFBFlowControl* pFlowControl);

protected:
    virtual void paintEvent(QPaintEvent* pEvent);

private slots:
    
    void onAddEventBtnClicked();

    
    void onTabBarClicked(int nIndex);

#if 0
    
    void onEventClicked();
#endif

private:
    
    void initCtrls();

    
    void clearListenEvents();

    
    bool existEvent(emListenEventType eEventType);

    
    LISTENEVENTDATA* findEventData(emListenEventType eEventType);

    
    void setDefaultStyle();

private:
    Ui::CFBEventEditorWidget *ui;
    ICFBEventEditorNotify*    m_pNotify;        //通知地址
    VecListenEventType        m_vecListenEvent; //监听事件列表
    //ICFBFlowControl*         m_pFlowControl;  //流程图控件
};

#endif // CFBEVENTEDITORWIDGET_H
