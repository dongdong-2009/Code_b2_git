// alarmqtydlg.cpp : implementation file
//

#include "stdafx.h"
#include "alarmgenerator.h"
#include "alarmqtydlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AlarmQtyDlg dialog


AlarmQtyDlg::AlarmQtyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AlarmQtyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AlarmQtyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_AlarmQty = 0;
	m_AlarmGenDlg = static_cast<CAlarmGeneratorDlg*> (pParent);
}


void AlarmQtyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AlarmQtyDlg)
	DDX_Control(pDX, IDC_ALARM_QTY, m_AlarmQtyEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AlarmQtyDlg, CDialog)
	//{{AFX_MSG_MAP(AlarmQtyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AlarmQtyDlg message handlers

void AlarmQtyDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_AlarmQty = 0;
	m_AlarmGenDlg->setAlarmQty(m_AlarmQty);
	
	CDialog::OnCancel();
}

void AlarmQtyDlg::OnOK() 
{
	// TODO: Add extra validation here
	unsigned long editQty(0);
	CString csQty;
	m_AlarmQtyEdit.GetWindowText( csQty );
	std::string sQty(csQty);
	
	if ( !sQty.empty() )
	{
		editQty = atol(sQty.c_str());
	}

	m_AlarmQty = editQty;
	m_AlarmGenDlg->setAlarmQty(m_AlarmQty);
	
	CDialog::OnOK();
}
