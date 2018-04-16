/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/currentcallstack.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CurrentCallStack.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "CurrentCallStack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CurrentCallStack dialog


CurrentCallStack::CurrentCallStack(CWnd* pParent /*=NULL*/)
	: CDialog(CurrentCallStack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CurrentCallStack)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CurrentCallStack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CurrentCallStack)
	DDX_Control(pDX, IDC_HOLD_CALL_STACK, m_holdCallStack);
	DDX_Control(pDX, IDC_MY_CALL_STACK, m_incomingStack);
	DDX_Control(pDX, IDC_EMERGENCY_CALL_STACK, m_emergencyCallStack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CurrentCallStack, CDialog)
	//{{AFX_MSG_MAP(CurrentCallStack)
	ON_BN_CLICKED(IDC_ANSWER, OnAnswerIncom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CurrentCallStack message handlers

BOOL CurrentCallStack::OnInitDialog() 
{
	static const COLORREF Gray  = RGB(180,180,180);
	static const COLORREF Green = RGB(0,255,0);
	static const COLORREF Red   = RGB(255,0,0);

	CDialog::OnInitDialog();
	
	// Setup call stack headings
	m_incomingStack.InsertColumn(0,"Time");
	m_incomingStack.InsertColumn(1,"Caller");
	m_incomingStack.InsertColumn(2,"Number");
	m_incomingStack.InsertColumn(3,"Location");
	m_incomingStack.SetColumnWidth(0,80);
	m_incomingStack.SetColumnWidth(1,140);
	m_incomingStack.SetColumnWidth(2,110);
	m_incomingStack.SetColumnWidth(3,100);
	m_incomingStack.SetBkColor(Gray);
	m_incomingStack.SetTextBkColor(Gray);

	m_emergencyCallStack.InsertColumn(0,"Time");
	m_emergencyCallStack.InsertColumn(1,"Caller");
	m_emergencyCallStack.InsertColumn(2,"Number");
	m_emergencyCallStack.InsertColumn(3,"Location");
	m_emergencyCallStack.SetColumnWidth(0,80);
	m_emergencyCallStack.SetColumnWidth(1,140);
	m_emergencyCallStack.SetColumnWidth(2,110);
	m_emergencyCallStack.SetColumnWidth(3,100);

	m_emergencyCallStack.SetBkColor(Gray);
	m_emergencyCallStack.SetTextBkColor(Red);

	m_holdCallStack.InsertColumn(0,"Time");
	m_holdCallStack.InsertColumn(1,"Called");
	m_holdCallStack.InsertColumn(2,"Number");
	m_holdCallStack.InsertColumn(3,"Location");
	m_holdCallStack.SetColumnWidth(0,80);
	m_holdCallStack.SetColumnWidth(1,140);
	m_holdCallStack.SetColumnWidth(2,110);
	m_holdCallStack.SetColumnWidth(3,100);

	m_holdCallStack.SetBkColor(Gray);
	m_holdCallStack.SetTextBkColor(Gray);

	m_incomingStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_emergencyCallStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_holdCallStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CurrentCallStack::OnAnswerIncom() 
{
	// TODO: Add your control notification handler code here
	
}
