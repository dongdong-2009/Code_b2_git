/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/DirectLineDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// DirectLineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "DirectLineDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DirectLineDialog dialog


DirectLineDialog::DirectLineDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DirectLineDialog::IDD, pParent)
{
    FUNCTION_ENTRY( "DirectLineDialog" );

    // {{AFX_DATA_INIT(DirectLineDialog)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT
    // Setup the tab control

    FUNCTION_EXIT;
}


void DirectLineDialog::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(DirectLineDialog)
    DDX_Control(pDX, IDC_AUTO_CALL_CB, m_auto_call_cb);
    DDX_Control(pDX, IDC_DIRECT_LINE_TAB, m_tabControl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(DirectLineDialog, CDialog)
    // {{AFX_MSG_MAP(DirectLineDialog)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DirectLineDialog message handlers

BOOL DirectLineDialog::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Setup the tab control
    m_tabControl.InsertItem(0,"User");
    m_tabControl.InsertItem(2,"PCP");
    m_tabControl.InsertItem(1,"Stations");
    m_tabControl.InsertItem(2,"OCR");
    m_tabControl.InsertItem(3,"DCR");
    m_tabControl.InsertItem(4,"Others");

    // enable the "Automatically call" checkbox
    m_auto_call_cb.SetCheck(1);

    FUNCTION_EXIT;
    return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


