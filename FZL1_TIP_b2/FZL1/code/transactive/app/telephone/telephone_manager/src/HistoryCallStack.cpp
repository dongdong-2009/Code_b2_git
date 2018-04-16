/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/HistoryCallStack.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// HistoryCallStack.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "HistoryCallStack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HistoryCallStack dialog


HistoryCallStack::HistoryCallStack(CWnd* pParent /*=NULL*/)
	: CDialog(HistoryCallStack::IDD, pParent)
{
    FUNCTION_ENTRY( "HistoryCallStack" );

    // {{AFX_DATA_INIT(HistoryCallStack)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void HistoryCallStack::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(HistoryCallStack)
    DDX_Control(pDX, IDC_HISTORY_STACK, m_historyStack);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(HistoryCallStack, CDialog)
    // {{AFX_MSG_MAP(HistoryCallStack)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HistoryCallStack message handlers

BOOL HistoryCallStack::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    static const COLORREF Gray  = RGB(180,180,180);
    static const COLORREF Green = RGB(0,255,0);
    static const COLORREF Red   = RGB(255,0,0);

    m_historyStack.InsertColumn(0,"Time");
    m_historyStack.InsertColumn(1,"Origin");
    m_historyStack.InsertColumn(2,"Caller/Called");
    m_historyStack.InsertColumn(3,"Location");

    m_historyStack.SetColumnWidth(0,100);
    m_historyStack.SetColumnWidth(1,80);
    m_historyStack.SetColumnWidth(2,180);
    m_historyStack.SetColumnWidth(3,150);

    m_historyStack.SetBkColor(Gray);
    m_historyStack.SetTextBkColor(Gray);

    m_historyStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


