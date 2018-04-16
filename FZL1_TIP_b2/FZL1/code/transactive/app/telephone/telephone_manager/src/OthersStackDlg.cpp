/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/OthersStackDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// OthersStackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "OthersStackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OthersStackDlg dialog


OthersStackDlg::OthersStackDlg(CWnd* pParent /*=NULL*/)
    : CDialog(OthersStackDlg::IDD, pParent)
{
    FUNCTION_ENTRY( "OthersStackDlg" );

    // {{AFX_DATA_INIT(OthersStackDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void OthersStackDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(OthersStackDlg)
    DDX_Control(pDX, IDC_OTHERS_CALL_STACK, m_othersCallStack);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(OthersStackDlg, CDialog)
    // {{AFX_MSG_MAP(OthersStackDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OthersStackDlg message handlers

BOOL OthersStackDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    static const COLORREF Gray  = RGB(180,180,180);
    static const COLORREF Green = RGB(0,255,0);
    static const COLORREF Red   = RGB(255,0,0);

    m_othersCallStack.InsertColumn(0,"Time");
    m_othersCallStack.InsertColumn(1,"Caller");
    m_othersCallStack.InsertColumn(2,"Source");
    m_othersCallStack.InsertColumn(3,"Called");
    m_othersCallStack.InsertColumn(4,"Destination");
    m_othersCallStack.InsertColumn(5,"Status");

    m_othersCallStack.SetColumnWidth(0,80);
    m_othersCallStack.SetColumnWidth(1,80);
    m_othersCallStack.SetColumnWidth(2,100);
    m_othersCallStack.SetColumnWidth(3,80);
    m_othersCallStack.SetColumnWidth(4,110);
    m_othersCallStack.SetColumnWidth(5,100);

    m_othersCallStack.SetBkColor(Gray);
    m_othersCallStack.SetTextBkColor(Gray);

    m_othersCallStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


