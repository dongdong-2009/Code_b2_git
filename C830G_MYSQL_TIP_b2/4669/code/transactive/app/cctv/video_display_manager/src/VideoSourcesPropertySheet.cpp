/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoSourcesPropertySheet.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "videosourcespropertysheet.h"
#include <afxpriv.h>
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_RESIZEPAGE WM_APP+1

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

// helper function which sets the font for a window and all its children
// and also resizes everything according to the new font
void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
    CRect windowRect;

    // grab old and new text metrics
    TEXTMETRIC tmOld, tmNew;
    CDC * pDC = pWnd->GetDC();
    CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
    pDC->GetTextMetrics(&tmOld);
    pDC->SelectObject(pFont);
    pDC->GetTextMetrics(&tmNew);
    pDC->SelectObject(pSavedFont);
    pWnd->ReleaseDC(pDC);

    long oldHeight = tmOld.tmHeight+tmOld.tmExternalLeading;
    long newHeight = tmNew.tmHeight+tmNew.tmExternalLeading;

    if (nFlag != CDF_NONE)
    {
        // calculate new dialog window rectangle
        CRect clientRect, newClientRect, newWindowRect;

        pWnd->GetWindowRect(windowRect);
        pWnd->GetClientRect(clientRect);
        long xDiff = windowRect.Width() - clientRect.Width();
        long yDiff = windowRect.Height() - clientRect.Height();

        newClientRect.left = newClientRect.top = 0;
        newClientRect.right = clientRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        newClientRect.bottom = clientRect.bottom * newHeight / oldHeight;

        if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
        {
            newWindowRect.left = windowRect.left;
            newWindowRect.top = windowRect.top;
            newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
            newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
        }
        else if (nFlag == CDF_CENTER) // resize with origin at center of window
        {
            newWindowRect.left = windowRect.left -
                            (newClientRect.right - clientRect.right)/2;
            newWindowRect.top = windowRect.top -
                            (newClientRect.bottom - clientRect.bottom)/2;
            newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
            newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
        }
        pWnd->MoveWindow(newWindowRect);
    }

    pWnd->SetFont(pFont);

    // iterate through and move all child windows and change their font.
    CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);

    while (pChildWnd)
    {
        pChildWnd->SetFont(pFont);
        pChildWnd->GetWindowRect(windowRect);

        CString strClass;
        ::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
        strClass.MakeUpper();
        if(strClass==_T("COMBOBOX"))
        {
            CRect rect;
            pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
            windowRect.right = rect.right;
            windowRect.bottom = rect.bottom;
        }

        pWnd->ScreenToClient(windowRect);
        windowRect.left = windowRect.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        windowRect.right = windowRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
        windowRect.top = windowRect.top * newHeight / oldHeight;
        windowRect.bottom = windowRect.bottom * newHeight / oldHeight;
        pChildWnd->MoveWindow(windowRect);

        pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
    }
}
/////////////////////////////////////////////////////////////////////////////
// VideoSourcesPropertySheet

IMPLEMENT_DYNAMIC(VideoSourcesPropertySheet, CPropertySheet)

VideoSourcesPropertySheet::VideoSourcesPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    FUNCTION_ENTRY( "VideoSourcesPropertySheet" );
    FUNCTION_EXIT;
}


VideoSourcesPropertySheet::VideoSourcesPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    FUNCTION_ENTRY( "VideoSourcesPropertySheet" );
    FUNCTION_EXIT;
}


VideoSourcesPropertySheet::~VideoSourcesPropertySheet()
{
    FUNCTION_ENTRY( "~VideoSourcesPropertySheet" );

    if (m_fntPage.m_hObject)
    {
        VERIFY (m_fntPage.DeleteObject ());
    }

    FUNCTION_EXIT;
}


void VideoSourcesPropertySheet::increaseHeight( int units )
{
    FUNCTION_ENTRY( "increaseHeight" );

    CRect rect;
    GetWindowRect( rect );
    SetWindowPos(NULL, 0, 0, rect.Width(), ( rect.Height() + units ),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // set and save the size of the page
    CTabCtrl* pTab = GetTabControl ();
    ASSERT (pTab);

    CPropertyPage* pPage = GetActivePage ();
    ASSERT (pPage);

    pTab->GetWindowRect( rect );
    pTab->SetWindowPos(NULL, 0, 0, rect.Width(), ( rect.Height() + units ),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    if (m_psh.dwFlags & PSH_WIZARD)
    {
        pTab->ShowWindow (SW_HIDE);
        GetClientRect (&m_rctPage);

        CWnd* pButton = GetDlgItem (ID_WIZBACK);
        ASSERT (pButton);
        CRect rc;
        pButton->GetWindowRect (&rc);
        ScreenToClient (&rc);
        m_rctPage.bottom = rc.top-2;
    }
    else
    {
        pTab->GetWindowRect (&m_rctPage);
        ScreenToClient (&m_rctPage);
        pTab->AdjustRect (FALSE, &m_rctPage);
    }

    // resize the page
    pPage->SetWindowPos(NULL, 0, 0, (m_rctPage.right - m_rctPage.left), (m_rctPage.bottom - m_rctPage.top),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    FUNCTION_EXIT;
}


BOOL VideoSourcesPropertySheet::EnableItem( int nItem, BOOL bEnable )
{
    FUNCTION_ENTRY( "EnableItem" );
    FUNCTION_EXIT;
    return m_customTabControl.EnableItem( nItem, bEnable );
}


BEGIN_MESSAGE_MAP(VideoSourcesPropertySheet, CPropertySheet)
    // {{AFX_MSG_MAP(VideoSourcesPropertySheet)
    // }}AFX_MSG_MAP
    ON_MESSAGE (WM_RESIZEPAGE, OnResizePage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VideoSourcesPropertySheet message handlers

void VideoSourcesPropertySheet::BuildPropPageArray()
{
    FUNCTION_ENTRY( "BuildPropPageArray" );

    CPropertySheet::BuildPropPageArray();

    // get first page
    CPropertyPage* pPage = GetPage (0);
    ASSERT (pPage);

    // dialog template class in afxpriv.h
    CDialogTemplate dlgtemp;
    // load the dialog template
    VERIFY (dlgtemp.Load (pPage->m_psp.pszTemplate));
    // get the font information
    CString strFace;
    WORD    wSize;
    VERIFY (dlgtemp.GetFont (strFace, wSize));
    if (m_fntPage.m_hObject)
    {
        VERIFY (m_fntPage.DeleteObject ());
    }

    // create a font using the info from first page
    VERIFY (m_fntPage.CreatePointFont (wSize*10, strFace));

    FUNCTION_EXIT;
}


BOOL VideoSourcesPropertySheet::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    ModifyStyleEx (0, WS_EX_CONTROLPARENT);

    // TODO fix the following so it only does this if not modal!
    HWND hWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
    CRect rectOld;
    ::GetWindowRect(hWnd, &rectOld);
    ScreenToClient(rectOld);
    int nCx = rectOld.Width();
    int nCy = rectOld.Height();
    // move tab control
    ::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    GetWindowRect(&rectOld);
    rectOld.right -= nCx;
    rectOld.bottom -= nCy;
    SetWindowPos(NULL, 0, 0, rectOld.Width(), rectOld.Height(),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);


    CPropertySheet::OnInitDialog();

    // get the font for the first active page
    CPropertyPage* pPage = GetActivePage ();
    ASSERT (pPage);

    // change the font for the sheet
    ChangeDialogFont (this, &m_fntPage, CDF_CENTER);
    // change the font for each page
    for (int iCntr = 0; iCntr < GetPageCount (); iCntr++)
    {
        VERIFY (SetActivePage (iCntr));
        CPropertyPage* pPage = GetActivePage ();
        ASSERT (pPage);
        ChangeDialogFont (pPage, &m_fntPage, CDF_CENTER);
    }

    VERIFY (SetActivePage (pPage));

    // set and save the size of the page
    CTabCtrl* pTab = GetTabControl ();
    ASSERT (pTab);

    if (m_psh.dwFlags & PSH_WIZARD)
    {
        pTab->ShowWindow (SW_HIDE);
        GetClientRect (&m_rctPage);

        CWnd* pButton = GetDlgItem (ID_WIZBACK);
        ASSERT (pButton);
        CRect rc;
        pButton->GetWindowRect (&rc);
        ScreenToClient (&rc);
        m_rctPage.bottom = rc.top-2;
    }
    else
    {
        pTab->GetWindowRect (&m_rctPage);
        ScreenToClient (&m_rctPage);
        pTab->AdjustRect (FALSE, &m_rctPage);
    }

    // resize the page
    pPage->MoveWindow (&m_rctPage);

    m_customTabControl.Install( this );

    FUNCTION_EXIT;
    return TRUE;
}


BOOL VideoSourcesPropertySheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnNotify" );

    NMHDR* pnmh = (LPNMHDR) lParam;

    // the sheet resizes the page whenever it is activated so we need to size it correctly
    if (TCN_SELCHANGE == pnmh->code)
    {
        PostMessage (WM_RESIZEPAGE);
    }

    FUNCTION_EXIT;
    return CPropertySheet::OnNotify(wParam, lParam, pResult);
}


LONG VideoSourcesPropertySheet::OnResizePage (UINT, LONG)
{
    FUNCTION_ENTRY( "OnResizePage" );

    // resize the page
    CPropertyPage* pPage = GetActivePage ();
    ASSERT (pPage);
    pPage->MoveWindow (&m_rctPage);

    FUNCTION_EXIT;
    return 0;
}


BOOL VideoSourcesPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnCommand" );

    // the sheet resizes the page whenever the Apply button is clicked so we need to size it correctly
    if (ID_APPLY_NOW == wParam ||
        ID_WIZNEXT == wParam ||
        ID_WIZBACK == wParam)
    {
        PostMessage (WM_RESIZEPAGE);
    }

    FUNCTION_EXIT;
    return CPropertySheet::OnCommand(wParam, lParam);
}


