/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/TheToolTipCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TheToolTipCtrl.cpp : implementation file
//

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "TheToolTipCtrl.h"
#include "core/utilities/src/FunctionEntry.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTheToolTipCtrl


CTheToolTipCtrl::CTheToolTipCtrl()
{
    FUNCTION_ENTRY( "CTheToolTipCtrl" );
    FUNCTION_EXIT;
}


CTheToolTipCtrl::~CTheToolTipCtrl()
{
    FUNCTION_ENTRY( "~CTheToolTipCtrl" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTheToolTipCtrl, CToolTipCtrl)
    // {{AFX_MSG_MAP(CTheToolTipCtrl)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheToolTipCtrl message handlers
BOOL CTheToolTipCtrl::AddWindowTool( CWnd* pWnd, LPCTSTR pszText )
{
    FUNCTION_ENTRY( "AddWindowTool" );

    TOOLINFO ti;
    ti.cbSize = sizeof( TOOLINFO );
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = pWnd->GetParent()->GetSafeHwnd();
    ti.uId = (UINT) pWnd->GetSafeHwnd();
    ti.hinst = AfxGetInstanceHandle();
    ti.lpszText = (LPTSTR) pszText;

    LRESULT  message = SendMessage( TTM_ADDTOOL, 0, (LPARAM) &ti );

    FUNCTION_EXIT;
    return (BOOL) message;
}


