/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAManager/src/NumbericEdit.cpp $
  * @author  Huang Qi
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */
#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/NumbericEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NumbericEdit

NumbericEdit::NumbericEdit()
{
}

NumbericEdit::~NumbericEdit()
{
}


BEGIN_MESSAGE_MAP(NumbericEdit, CEdit)
	//{{AFX_MSG_MAP(NumbericEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NumbericEdit message handlers

void NumbericEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    const UINT unCtrlVPasteChar = 22;
    if ( unCtrlVPasteChar == nChar )
    {
        if ( !PrePaste() )
        {
            return;
        }
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

bool NumbericEdit::PrePaste()
{
    OpenClipboard();

    HANDLE hData = ::GetClipboardData( CF_TEXT );
    std::string strBuffer = static_cast<char*>(::GlobalLock(hData));
    ::GlobalUnlock( hData );

    CloseClipboard();

    return numberStringValid( strBuffer );
}

LRESULT NumbericEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if ( WM_PASTE == message )
    {
        if ( !PrePaste() )
        {
            return 0;
        }
    }

	return CEdit::WindowProc(message, wParam, lParam);
}

bool NumbericEdit::numberStringValid( const std::string& strNumber )
{
    if ( std::string::npos != strNumber.find_first_not_of( "0123456789" ))
    {
        return false;
    }

    // All rules OK
    return true;
}