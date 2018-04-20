/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/DirectorySearchDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// DirectorySearchDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "DirectorySearchDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DirectorySearchDialog dialog


DirectorySearchDialog::DirectorySearchDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DirectorySearchDialog::IDD, pParent)
{
    FUNCTION_ENTRY( "DirectorySearchDialog" );

    // {{AFX_DATA_INIT(DirectorySearchDialog)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void DirectorySearchDialog::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(DirectorySearchDialog)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(DirectorySearchDialog, CDialog)
    // {{AFX_MSG_MAP(DirectorySearchDialog)
        // NOTE: the ClassWizard will add message map macros here
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DirectorySearchDialog message handlers
