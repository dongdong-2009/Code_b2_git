/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/ConfigureDirectLinesDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ConfigureDirectLinesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "ConfigureDirectLinesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConfigureDirectLinesDialog dialog


ConfigureDirectLinesDialog::ConfigureDirectLinesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigureDirectLinesDialog::IDD, pParent)
{
    FUNCTION_ENTRY( "ConfigureDirectLinesDialog" );

    // {{AFX_DATA_INIT(ConfigureDirectLinesDialog)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void ConfigureDirectLinesDialog::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(ConfigureDirectLinesDialog)
    DDX_Control(pDX, IDC_DIRECT_LINE_TAB, m_tabControl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(ConfigureDirectLinesDialog, CDialog)
    // {{AFX_MSG_MAP(ConfigureDirectLinesDialog)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConfigureDirectLinesDialog message handlers

BOOL ConfigureDirectLinesDialog::OnInitDialog()
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

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


