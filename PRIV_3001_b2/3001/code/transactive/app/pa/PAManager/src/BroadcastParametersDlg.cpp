/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/BroadcastParametersDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// BroadcastParametersDlg.cpp : implementation file
//

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/BroadcastParametersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BroadcastParametersDlg dialog


BroadcastParametersDlg::BroadcastParametersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BroadcastParametersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(BroadcastParametersDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BroadcastParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BroadcastParametersDlg)
	DDX_Control(pDX, IDC_EDIT1, m_dataEb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BroadcastParametersDlg, CDialog)
	//{{AFX_MSG_MAP(BroadcastParametersDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BroadcastParametersDlg message handlers

void BroadcastParametersDlg::OnOK() 
{
	this->ShowWindow(SW_HIDE);	
}
