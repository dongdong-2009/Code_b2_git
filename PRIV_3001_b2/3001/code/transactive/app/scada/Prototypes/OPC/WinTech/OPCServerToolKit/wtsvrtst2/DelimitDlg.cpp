// DelimitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WtSvrTst2.h"
#include "DelimitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelimitDlg dialog


CDelimitDlg::CDelimitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelimitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelimitDlg)
	m_delimiter = _T("");
	//}}AFX_DATA_INIT
}


void CDelimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelimitDlg)
	DDX_Text(pDX, IDC_EDIT1, m_delimiter);
	DDV_MaxChars(pDX, m_delimiter, 1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelimitDlg, CDialog)
	//{{AFX_MSG_MAP(CDelimitDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelimitDlg message handlers
