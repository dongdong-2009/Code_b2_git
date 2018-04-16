/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioInitMsgDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
// RadioInitMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "radiomanager.h"
#include "RadioInitMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioInitMsgDlg dialog


CRadioInitMsgDlg::CRadioInitMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRadioInitMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRadioInitMsgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRadioInitMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioInitMsgDlg)
	DDX_Control(pDX, IDC_INITIALISE_MSG, m_msgStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioInitMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CRadioInitMsgDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioInitMsgDlg message handlers

void CRadioInitMsgDlg::setMsg(const CString &strMSg)
{
	m_msgStatic.SetWindowText(strMSg);
	UpdateData();
}
