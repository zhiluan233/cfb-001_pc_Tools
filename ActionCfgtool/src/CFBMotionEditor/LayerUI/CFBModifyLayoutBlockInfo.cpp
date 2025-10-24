//#include "stdafx.h"
//#include "CFBModifyLayoutBlockInfo.h"
////#include "afxdialogex.h"


//// BlockDescriptionDlg 对话框

//IMPLEMENT_DYNAMIC(CCFBModifyLayoutBlockInfo, CDialog)

//// CCFBModifyLayoutBlockInfo::CCFBModifyLayoutBlockInfo(QWidget* pParent )
//// 	: CDialog(CCFBModifyLayoutBlockInfo::IDD, pParent)
//// {
////
//// }

//CCFBModifyLayoutBlockInfo::CCFBModifyLayoutBlockInfo(QWidget* pParent, QString DescriptionInfo)
//	: CDialog(CCFBModifyLayoutBlockInfo::IDD, pParent)
//{
//	if (DescriptionInfo.IsEmpty())
//	{
//		m_BlockDescriptionInfo = _T("DefaultName");
//	} else {
//		m_BlockDescriptionInfo = DescriptionInfo;
//	}
	
//	m_StartTime = 0;
//	m_EndTime = 0;
//}

//CCFBModifyLayoutBlockInfo::~CCFBModifyLayoutBlockInfo()
//{
//}

//void CCFBModifyLayoutBlockInfo::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT_START_TIME, m_StartTime);
//	DDX_Text(pDX, IDC_EDIT_END_TIME, m_EndTime);
//}


//BEGIN_MESSAGE_MAP(CCFBModifyLayoutBlockInfo, CDialog)
//	ON_BN_CLICKED(IDOK, &CCFBModifyLayoutBlockInfo::OnBnClickedOk)
//END_MESSAGE_MAP()


//// BlockDescriptionDlg 消息处理程序


//void CCFBModifyLayoutBlockInfo::OnBnClickedOk()
//{

//	GetDlgItemText(IDC_ED_ADDBLOCKBASE_DESCRIPTION, m_BlockDescriptionInfo);

//	CDialog::OnOK();
//}


//bool CCFBModifyLayoutBlockInfo::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//	SetDlgItemText(IDC_ED_ADDBLOCKBASE_DESCRIPTION, m_BlockDescriptionInfo);

//	GetDlgItem(IDC_EDIT_START_TIME)->EnableWindow(false);
//	GetDlgItem(IDC_EDIT_END_TIME)->EnableWindow(false);

//	return true;  // return true unless you set the focus to a control
//	// 异常: OCX 属性页应返回 false
//}
