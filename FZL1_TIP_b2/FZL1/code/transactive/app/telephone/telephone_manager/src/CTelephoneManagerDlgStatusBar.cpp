/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CTelephoneManagerDlgStatusBar.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include <afxpriv.h>
#include "core/utilities/src/DebugUtil.h"

#include "app/telephone/telephone_manager/src/CTelephoneManagerDlgStatusBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTelephoneManagerDlgStatusBar

CTelephoneManagerDlgStatusBar::CTelephoneManagerDlgStatusBar()
{
    FUNCTION_ENTRY( "CTelephoneManagerDlgStatusBar" );
    FUNCTION_EXIT;
}


CTelephoneManagerDlgStatusBar::~CTelephoneManagerDlgStatusBar()
{
    FUNCTION_ENTRY( "~CTelephoneManagerDlgStatusBar" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneManagerDlgStatusBar, CStatusBar)
    // {{AFX_MSG_MAP(CTelephoneManagerDlgStatusBar)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTelephoneManagerDlgStatusBar message handlers
//      OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is
//      used to update the status of user-interface elements within the MFC
//      framework.
//
//      We have to get a little tricky here: CStatusBar::OnUpdateCmdUI
//      expects a CFrameWnd pointer as its first parameter.  However, it
//      doesn't do anything but pass the parameter on to another function
//      which only requires a CCmdTarget pointer.  We can get a CWnd pointer
//      to the parent window, which is a CCmdTarget, but may not be a
//      CFrameWnd.  So, to make CStatusBar::OnUpdateCmdUI happy, we will call
//      our CWnd pointer a CFrameWnd pointer temporarily.

LRESULT CTelephoneManagerDlgStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
    FUNCTION_ENTRY( "OnIdleUpdateCmdUI" );

    if (IsWindowVisible())
    {
        CFrameWnd *pParent = (CFrameWnd *)GetParent();
        if (pParent)
        {
            OnUpdateCmdUI(pParent, (BOOL)wParam);
        }

    }

    FUNCTION_EXIT;
    return 0L;
}


BOOL CTelephoneManagerDlgStatusBar::SetPaneText( int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
    FUNCTION_ENTRY( "SetPaneText" );

    m_lastMsgDisplayed = lpszNewText;

    FUNCTION_EXIT;
    return CStatusBar::SetPaneText(nIndex, lpszNewText, bUpdate);
}


