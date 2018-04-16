/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/EditNoFocus.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "videodisplaymanager.h"
#include "EditNoFocus.h"
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEditNoFocus

CEditNoFocus::CEditNoFocus()
{
    FUNCTION_ENTRY( "CEditNoFocus" );
    FUNCTION_EXIT;
}


CEditNoFocus::~CEditNoFocus()
{
    FUNCTION_ENTRY( "~CEditNoFocus" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CEditNoFocus, CEdit)
    // {{AFX_MSG_MAP(CEditNoFocus)
    ON_WM_SETFOCUS()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditNoFocus message handlers

void CEditNoFocus::OnSetFocus(CWnd* pOldWnd)
{
    FUNCTION_ENTRY( "OnSetFocus" );

    // CEdit::OnSetFocus(pOldWnd);
    ::SetFocus(pOldWnd->GetSafeHwnd());
    // TODO: Add your message handler code here

    FUNCTION_EXIT;
}


