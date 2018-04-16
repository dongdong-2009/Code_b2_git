// RTUAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PLCSimulator.h"
#include "RTUAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTUAboutDlg dialog


CRTUAboutDlg::CRTUAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTUAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTUAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRTUAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTUAboutDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRTUAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CRTUAboutDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTUAboutDlg message handlers
