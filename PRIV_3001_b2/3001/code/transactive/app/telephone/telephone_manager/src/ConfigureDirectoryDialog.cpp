/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/ConfigureDirectoryDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// ConfigureDirectoryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "ConfigureDirectoryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConfigureDirectoryDialog dialog


ConfigureDirectoryDialog::ConfigureDirectoryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ConfigureDirectoryDialog::IDD, pParent)
{
    FUNCTION_ENTRY( "ConfigureDirectoryDialog" );

    // {{AFX_DATA_INIT(ConfigureDirectoryDialog)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void ConfigureDirectoryDialog::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(ConfigureDirectoryDialog)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(ConfigureDirectoryDialog, CDialog)
    // {{AFX_MSG_MAP(ConfigureDirectoryDialog)
        // NOTE: the ClassWizard will add message map macros here
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConfigureDirectoryDialog message handlers
