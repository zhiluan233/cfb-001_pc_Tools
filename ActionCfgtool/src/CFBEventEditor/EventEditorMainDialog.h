#ifndef EVENTEDITORMAINDIALOG
#define EVENTEDITORMAINDIALOG

#include "EventEditorToolsDialog.h"
#include "EventManager.h"

// CEventEditorMainDialog 对话框

class CEventEditorMainDialog : public CDialog, public ICFBEventEditor, public CEventManager
{
	DECLARE_DYNAMIC(CEventEditorMainDialog)

public:
	CEventEditorMainDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEventEditorMainDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_EVENTEDITOR_MAIN };

public:
	//ICFBEventEditor 的实现
	virtual bool Connect(CWnd* pwnd);
	virtual void ReleaseConnection();
	virtual CWnd* GetCWnd();
	virtual void GetData(char** ppData, int& nLen);
	virtual void SetData(char* pData, int nLen);
	virtual void Set3DRobotView(ICFBRobot3dView* pView);
	virtual void SetTreeView(ITreeNavigation* pView);
	virtual void ClearFlowChart();
	virtual ICFBFlowControl* GetEventFlowCtr();
	virtual void SetSocketSession(CICFBSocketSession* pSession);

	// 重写MANAGER方法
	virtual void OnEventHandlerSelChange(CIEventHandler* pEvent);
	virtual void OnShowConfigureWindow(bool Show);
	virtual bool IsShowConfigure(){return m_bShowConfigureUI;};

private:
	void SaveAllDataToEvent();

private:
	CEventEditorToolsDialog	m_dlgTools;
	CIEventHandler*			m_pCurrentEventHandler;
	ICFBFlowControl*		m_pFlowControl;
	// 是否显示配置UI
	bool					m_bShowConfigureUI;	
	CWnd*					m_pWndLastConfigueShow;
	CICFBSocketSession*		m_pSocketSession;
	ICFBRobot3dView*		m_pRobot3DView;
	ITreeNavigation*		m_pTreeNavigation;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
};

#endif
