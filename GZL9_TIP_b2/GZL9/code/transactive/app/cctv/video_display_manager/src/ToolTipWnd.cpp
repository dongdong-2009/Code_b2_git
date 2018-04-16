/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/ToolTipWnd.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif


#include "stdafx.h"
#include "ToolTipWnd.h"
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ToolTipWnd

ToolTipWnd::ToolTipWnd()
{
    FUNCTION_ENTRY( "ToolTipWnd" );

    m_szMargins = CSize(4,4);

    FUNCTION_EXIT;
}


ToolTipWnd::~ToolTipWnd()
{
    FUNCTION_ENTRY( "~ToolTipWnd" );
    FUNCTION_EXIT;
}


IMPLEMENT_DYNAMIC(ToolTipWnd,CWnd)
BEGIN_MESSAGE_MAP(ToolTipWnd, CWnd)
    // {{AFX_MSG_MAP(ToolTipWnd)
    ON_WM_PAINT()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_WM_TIMER()
    ON_WM_NCHITTEST()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ToolTipWnd message handlers

// CPopupText is intended to be used on the stack,
// not heap, so don't auto-delete.
//
void ToolTipWnd::PostNcDestroy()
{
    FUNCTION_ENTRY( "PostNcDestroy" );

    // don't delete this

    FUNCTION_EXIT;
}


BOOL ToolTipWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    static CString sClassName;
   if (sClassName.IsEmpty())
   {
       sClassName = AfxRegisterWndClass(0);
   }

   cs.lpszClass = sClassName;
   cs.style = WS_POPUP|WS_BORDER;
   cs.dwExStyle |= WS_EX_TOOLWINDOW;

   FUNCTION_EXIT;
   return CWnd::PreCreateWindow(cs);
}


void ToolTipWnd::OnPaint()
{
    FUNCTION_ENTRY( "OnPaint" );

    CRect rc;
    GetClientRect(&rc);
    CString s;
    GetWindowText(s);
    CPaintDC dc(this);
    DrawText(dc, s, rc, /*DT_SINGLELINE|DT_VCENTER|DT_CENTER*/DT_LEFT);   //TD17254

    FUNCTION_EXIT;
}


/*void ToolTipWnd::OnSetFocus(CWnd* pOldWnd)
{
    // we don't ever want the "tool-tip" to take focus
    if ( m_listBox->GetCurSel() != m_itemIndex)
    {
        Cancel();
        m_listBox->SetCurSel(m_itemIndex);
        ShowDelayed(500);
        ::SetFocus(m_listBox->GetSafeHwnd());
    }
    ::SetFocus(m_listBox->GetSafeHwnd());
    ShowWindow(SW_SHOWNA);
//    Invalidate();
//    UpdateWindow();
}*/

void ToolTipWnd::OnTimer(UINT nIDEvent)
{
    FUNCTION_ENTRY( "OnTimer" );

    ShowWindow(SW_SHOWNA);
    Invalidate();
    UpdateWindow();
    KillTimer(1);

    FUNCTION_EXIT;
}


// Someone changed the text: resize to fit new text
//
LRESULT ToolTipWnd::OnSetText(WPARAM wp, LPARAM lp)
{
    FUNCTION_ENTRY( "OnSetText" );

    CRect rc;
    GetWindowRect(&rc);
    int x = (m_nStyle & PTS_JUSTIFYRIGHT) ? rc.right : rc.left;
    int y = rc.top;
    CClientDC *pDc = static_cast<CClientDC*>(this->GetDC());
	DrawText(*pDc, CString((LPCTSTR)lp), rc, DT_CALCRECT);
    rc.InflateRect(m_szMargins);
    if (m_nStyle & PTS_JUSTIFYRIGHT) {
        x -= rc.Width();
    }
    SetWindowPos(NULL,x,y,rc.Width(),rc.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

    FUNCTION_EXIT;
    return Default();
}


void ToolTipWnd::DrawText(CDC&dc, LPCTSTR lpText, CRect &rc, UINT flags)
{
    FUNCTION_ENTRY( "DrawText" );

    CBrush b(GetSysColor(COLOR_INFOBK)); // use tooltip bg color
    dc.FillRect(&rc, &b);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(GetSysColor(COLOR_INFOTEXT)); // tooltip text color
    CFont* pOldFont = dc.SelectObject(m_fontWnd->GetFont());
    dc.DrawText(lpText, &rc, flags);
    dc.SelectObject(pOldFont);

    FUNCTION_EXIT;
}


// Show window with delay. No delay means show now.
//
void ToolTipWnd::ShowDelayed(UINT msec)
{
    FUNCTION_ENTRY( "ShowDelayed" );

    if (msec==0)
    {
        // no delay: show it now
        OnTimer(1);
    }

    else
        // delay: set time
        SetTimer(1, msec, NULL);

    FUNCTION_EXIT;
}


// Cancel text kill timer and hide window
//
void ToolTipWnd::Cancel()
{
    FUNCTION_ENTRY( "Cancel" );

    KillTimer(1);
    ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


// Create window. pt is upper-left or upper-right corner depending on
// nStyle.
//
BOOL ToolTipWnd::Create(CPoint pt, CWnd* pListBox, UINT nStyle, UINT nID)
{
    FUNCTION_ENTRY( "Create" );

    m_nStyle = nStyle;
    m_fontWnd = pListBox;

    FUNCTION_EXIT;
    return CreateEx(0,
        NULL,
        NULL,
        WS_POPUP|WS_VISIBLE,
        CRect(pt,CSize(0,0)),
        pListBox,
        nID);
}


LRESULT ToolTipWnd::OnNcHitTest(CPoint point)
{
    FUNCTION_ENTRY( "OnNcHitTest" );

    if (m_nStyle & PTS_TRANSPARENT)
    {
        // transparent?
        FUNCTION_EXIT;
        // return HTTRANSPARENT;
		return S_OK;
    }
                 // ..make it so
    FUNCTION_EXIT;
    return CWnd::OnNcHitTest(point);             // otherwise return default
}


