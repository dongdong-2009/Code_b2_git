/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/VideoSourcesPropertySheet.cpp $
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

VideoSourcesPropertySheet::VideoSourcesPropertySheet(UINT nIDCaption,RECT rect, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage),m_rctCtrl(rect)
{
    FUNCTION_ENTRY( "VideoSourcesPropertySheet" );
    FUNCTION_EXIT;
}


VideoSourcesPropertySheet::VideoSourcesPropertySheet(LPCTSTR pszCaption,RECT rect, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage),m_rctCtrl(rect)
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
	m_rctPage.left=1;
	m_rctPage.top=25;
	m_rctPage.right=m_rctCtrl.right-m_rctCtrl.left-5;
	m_rctPage.bottom=m_rctCtrl.bottom-m_rctCtrl.top-2;

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
	MoveWindow(&m_rctCtrl);
    CTabCtrl* pTab = GetTabControl ();
    ASSERT (pTab);
	pTab->MoveWindow (0,0,m_rctCtrl.right-m_rctCtrl.left,m_rctCtrl.bottom-m_rctCtrl.top);
    /*if (m_psh.dwFlags & PSH_WIZARD)
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
    }*/

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



//---------------------------------

// Define our offsets for drawing
#define TCEX_SELECTED_XOFFSET	7
#define TCEX_SELECTED_YOFFSET	0
#define TCEX_UNSELECTED_XOFFSET	4
#define TCEX_UNSELECTED_YOFFSET	2
#define CXBUTTONMARGIN			2
#define CYBUTTONMARGIN			3

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx2
namespace TA_Base_Bus
{
    CTabCtrlEx2::CTabCtrlEx2()
    {
    }

    CTabCtrlEx2::~CTabCtrlEx2()
    {
    }


    BEGIN_MESSAGE_MAP(CTabCtrlEx2, CTabCtrl)
	    //{{AFX_MSG_MAP(CTabCtrlEx2)
	    ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	    ON_WM_DESTROY()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTabCtrlEx2 message handlers
    /////////////////////////////////////////////////////////////////////////////
    /*
     * DrawItem
     *
     * Purpose:
     *   This is the function that draws the tab items (bitmaps and text) for
     *   this owner-drawn control
     *
     * Inputs:
     *   LPDRAWITEMSTRUCT lpd: Contains vital drawing information
     *
     * Returns:
     *   Nothing
     *
     * Note:
     *   This function is the heart of the extended tab control. It draws the
     *   items enabled or disabled, with or without images. There are bound to
     *   be flaws here, especially in the drawing code but for simple tabs it
     *   seems to work fairly well.
     */
    void CTabCtrlEx2::DrawItem(LPDRAWITEMSTRUCT lpd)
    {
	    // Make sure it's a tab control
	    if ( lpd->CtlType != ODT_TAB ) {
		    TRACE0( "CTabCtrlEx2::DrawItem() - Ignoring non-tab control\r\n" );
		    return;
	    }

	    // Get whether or not this item is enabled
	    BOOL bIsEnabled = IsItemEnabled( lpd->itemID );

	    // Get this item's text
	    TC_ITEM tci;
	    memset( &tci, 0, sizeof( TC_ITEM ) );
	    tci.mask = TCIF_TEXT | TCIF_IMAGE;
	    char buf[255];
	    tci.pszText = buf;
	    tci.cchTextMax = 255;
	    if ( !GetItem( lpd->itemID, &tci ) ) {
		    TRACE1( "Failed to get item %d\r\n", lpd->itemID );
		    return;
	    }

	    // Create a CRect
	    CRect rect( lpd->rcItem );
	    // Draw the background
	    FillRect( lpd->hDC, rect, (HBRUSH)(CTLCOLOR_DLG + 1) );
	    // Adjust the rectangle
	    if ( lpd->itemState & ODS_SELECTED ) {
		    // Push it right
		    rect.left += TCEX_SELECTED_XOFFSET;
		    // Push it down
		    rect.top += TCEX_SELECTED_YOFFSET;
	    }
	    else {
		    // Push it right
		    rect.left += TCEX_UNSELECTED_XOFFSET;
		    // Push it down
		    rect.top += TCEX_UNSELECTED_YOFFSET;
	    }

	    // Set the background mode (paint transparent)
	    SetBkMode( lpd->hDC, TRANSPARENT );

	    // If this item has an image, draw it
	    if ( tci.iImage >= 0 ) {
		    // Get the image list
		    CImageList *il = GetImageList();
		    ASSERT( il != NULL );
		    // Figure out where exactly we want to put the bitmap
		    CPoint pt( rect.TopLeft() );
		    // Adjust the rectangle
		    if ( lpd->itemState & ODS_SELECTED ) {
			    // Push it down
			    pt.y += CYBUTTONMARGIN;
		    }
		    // Draw it
		    CDC hDC;
		    hDC.Attach( lpd->hDC );
		    if ( bIsEnabled ) {
			    il->Draw( &hDC, tci.iImage, pt, ILD_NORMAL );
		    }
		    else {
			    // use DrawState to draw disabled button: must convert to icon
			    HICON hIcon = il->ExtractIcon( tci.iImage );
			    ASSERT(hIcon);
			    hDC.DrawState( pt, CSize(0,0), hIcon, DSS_DISABLED, (HBRUSH)NULL );
			    DestroyIcon(hIcon);
		    }
		    hDC.Detach();
		    // Push the text over
		    IMAGEINFO ii;
		    il->GetImageInfo( tci.iImage, &ii );
		    rect.left += (ii.rcImage.right - ii.rcImage.left); // Add the width of the bitmap
		    rect.left += CXBUTTONMARGIN; // Plus some separation
	    }

	    // We will need to remember the current color
	    COLORREF origCol;
	    // Draw disabled
	    if ( bIsEnabled ) {
		    // Set the text color for our hilight (remember the original color)
		    origCol = SetTextColor( lpd->hDC, GetSysColor( COLOR_BTNTEXT ) );
		    // Draw the text
		    DrawText( lpd->hDC, buf, -1, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
	    }
	    // Draw enabled
	    else {
		    // Set the text color for our hilight (remember the original color)
		    origCol = SetTextColor( lpd->hDC, GetSysColor( COLOR_3DHILIGHT ) );
		    // Draw the text
		    DrawText( lpd->hDC, buf, -1, rect + CPoint(1,1), DT_SINGLELINE | DT_LEFT | DT_VCENTER );
		    // Set the text color for our gray text
		    SetTextColor( lpd->hDC, GetSysColor( COLOR_GRAYTEXT ) );
		    // Draw the text
		    DrawText( lpd->hDC, buf, -1, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
	    }

	    // Restore the original text color
	    SetTextColor( lpd->hDC, origCol );
    }

    LRESULT CTabCtrlEx2::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
    {
	    // TODO: Add your specialized code here and/or call the base class
	    if ( message == TCM_SETCURSEL ) {
		    if ( !IsItemEnabled( wParam ) ) {
			    // We only calculate a different selection if the
			    // user used CTRL+Tab or CTRL+Shift+Tab. We determine
			    // this by checking if the difference between the
			    // current and the new selection is 1 (or -1)
			    int curSel = GetCurSel(), nCount = GetItemCount();
			    int diff = wParam - curSel, adiff = abs( diff );
			    if ( adiff == 1 || adiff == (int)wParam || adiff == curSel ) {
				    // OK, let's find the next one
				    BOOL bNext = ( diff == 1 ) || ( diff < -1 );

				    // Find the next enabled item
				    int sel = FindNextEnabled( wParam, bNext );
				    if ( sel >= 0 ) {
					    // Set the current selection to the next enabled
					    CTabCtrl::SetCurSel( sel );
					    return (LRESULT)0;
				    }
			    }
			    // Return indicating not to select wParam
			    return (LRESULT)-1;
		    }
	    }

	    // We could have checked for these keys in a TCN_KEYDOWN handler,
	    // but it's just as easy to do it here...

	    // Check for and process arrow keys
	    if ( message == WM_KEYDOWN ) {
		    if ( wParam == VK_RIGHT || wParam == VK_DOWN ) {
			    // Get the current selection
			    int curSel = GetCurSel();
			    // Set the current tab
			    SetCurrentTab( curSel < GetItemCount() - 1 ? curSel + 1 : 0 );
			    // Return indicating that we processed the message
			    return (LRESULT)0;
		    }
		    if ( wParam == VK_LEFT || wParam == VK_UP ) {
			    // Get the current selection
			    int curSel = GetCurSel();
			    // Set the current tab
			    SetCurrentTab( curSel > 0 ? curSel - 1 : GetItemCount() - 1 );
			    // Return indicating that we processed the message
			    return (LRESULT)0;
		    }
	    }

	    return CTabCtrl::WindowProc(message, wParam, lParam);
    }


    /////////////////////////////////////////////////////////////////////////////
    // Message Map Functions
    /////////////////////////////////////////////////////////////////////////////
    void CTabCtrlEx2::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    // TODO: Add your control notification handler code here
	    // Get the position of the last message
	    DWORD lastPos = GetMessagePos();
	    // Convert it to a point
	    CPoint point( LOWORD( lastPos ), HIWORD( lastPos ) );
	    // Convert the point to client coordinates
	    ScreenToClient( &point );
	    // Create a hit test
	    TC_HITTESTINFO hti;
	    hti.pt = point;
	    hti.flags = TCHT_ONITEM;
	    // See which (if any) tab was hit
	    int nItem = HitTest( &hti );
	    if ( nItem == -1 ) {
		    // Fail the selection change
		    *pResult = 1;
		    return;
	    }
	    // Set the result based on whether or not the item is enabled
	    BOOL bResult = IsItemEnabled( nItem );

	    // If the page is active, then send the kill message to check
	    // whether or not the page wants to change
	    if ( bResult ) {
		    // Send the kill active page message
		    bResult = SendKillActive();
	    }

	    // Return our result (TRUE to stop, FALSE to proceed)
	    *pResult = !bResult;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Protected Functions
    /////////////////////////////////////////////////////////////////////////////

    /*
     * FindNextEnabled
     *
     * Purpose:
     *   Finds the next enabled tab on the control.
     *
     * Inputs:
     *   int nStart: The index of then item to start the search
     *   BOOL bNext: TRUE to search next, FALSE to search previous
     *
     * Returns:
     *   Index of next enabled item found, -1 if no item was found
     */
    int CTabCtrlEx2::FindNextEnabled( int nStart, BOOL bNext )
    {
	    // Get the number of item we have
	    int i, nCount = GetItemCount();
	    // Check if we should go next or back
	    if ( bNext ) {
		    // Search all the next items
		    for ( i = nStart; i < nCount; i++ ) {
			    if ( IsItemEnabled( i ) ) {
				    // Found one, return it
				    return i;
			    }
		    }
		    // We didn't find it, loop and start from the beginning
		    for ( i = 0; i < nStart; i++ ) {
			    if ( IsItemEnabled( i ) ) {
				    // Found one, return it
				    return i;
			    }
		    }
	    }
	    else {
		    // Search all the previous items
		    for ( i = nStart; i >= 0; --i ) {
			    if ( IsItemEnabled( i ) ) {
				    // Found one, return it
				    return i;
			    }
		    }
		    // We didn't find it, loop and start from the end
		    for ( i = (nCount - 1); i > nStart; --i ) {
			    if ( IsItemEnabled( i ) ) {
				    // Found one, return it
				    return i;
			    }
		    }
	    }
	    // Didn't find one
	    return -1;
    }

    /*
     * SetCurrentTab
     *
     * Purpose:
     *   Sets the current tab selection and sends the TCM_SELCHANGE
     *   notification so that the dialog is properly updated
     *
     * Inputs:
     *   int nItem: The item that is to be the current tab
     *
     * Returns:
     *   Nothing
     */
    void CTabCtrlEx2::SetCurrentTab( int nItem ) 
    {
	    // Make sure the item is in our range
	    if ( nItem < 0 || nItem >= GetItemCount() ) {
		    return;
	    }

	    // Send the kill active page message
	    if ( !SendKillActive() ) {
		    // The kill message returned that the page should not
		    // be changed, so we don't do it
		    return;
	    }

	    // Set the current selection
	    SetCurSel( nItem );

	    // SetCurSel does not send the TCN_SELCHANGE notification
	    m_nmhdr.hwndFrom = GetSafeHwnd(); 
	    m_nmhdr.idFrom = GetDlgCtrlID();
	    m_nmhdr.code = TCN_SELCHANGE;  
	    // Send the message
	    SendMessage( WM_NOTIFY, m_nmhdr.idFrom, (LPARAM)&m_nmhdr );
    }

    /*
     * SendKillActive
     *
     * Purpose:
     *   Sends a kill active message to a property page of a parent
     *   property sheet
     *
     * Inputs:
     *   int nItem: The index of the page to send the kill message to
     *
     * Returns:
     *   TRUE if the kill message succeeds, FALSE otherwise
     */
    BOOL CTabCtrlEx2::SendKillActive( int nItem )
    {
	    // Send a message to our parent to kill the current active window
	    CWnd *pParent = GetParent();
	    if ( pParent->IsKindOf( RUNTIME_CLASS( CPropertySheet ) ) ) {
		    // Check if the user wants to use the active page
		    if ( nItem == -1 ) {
			    nItem = ((CPropertySheet *)pParent)->GetActiveIndex();
		    }
		    // Get the property page
		    CPropertyPage *page = ((CPropertySheet *)pParent)->GetPage( nItem );
    #ifdef _DEBUG
		    CString txt;
		    page->GetWindowText( txt );
		    TRACE2( "Sending kill active message to property page #%d (%s)\r\n", nItem, (LPCSTR)txt );
    #endif // _DEBUG
		    m_nmhdr.hwndFrom = pParent->GetSafeHwnd();
		    m_nmhdr.idFrom = pParent->GetDlgCtrlID();
		    m_nmhdr.code = PSN_KILLACTIVE;
		    // PSN_KILLACTIVE will return TRUE to prevent the page from losing activation
		    // and FALSE to to allow it
		    return !page->SendMessage( WM_NOTIFY, m_nmhdr.idFrom, (LPARAM)&m_nmhdr );
	    }
	    // Return success - Send kill active to non-propertysheet does nothing
	    return TRUE;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Public Functions
    /////////////////////////////////////////////////////////////////////////////
    /*
     * Install
     *
     * Purpose:
     *   Installs the extended tab control
     *
     * Inputs:
     *   CTabCtrl *pTabCtrl: The tab control that we will subclass
     *
     * Returns:
     *   TRUE if install succeeds, FALSE otherwise
     *
     * Note:
     *   Install() MUST be called, even if the extended tab control is not
     *   subclassing a property sheet tab control. In that case, create
     *   the tab control using its Create() function, and then call install
     *   with a pointer to the control itself. Install() recognizes when
     *   this occurs and does not attempt to subclass itself. The style
     *   bit and image list are still set as normal. In the case where the
     *   tab control is used in a dialog as part of DDX and Create() is not
     *   called directly, Install() must still be called with a pointer to
     *   the extended tab control. Neither of these two cases has actually
     *   been tested by me at this point, but it sounds right. If you try
     *   this and it doesn't work, let me know...
     */
    BOOL CTabCtrlEx2::Install( CTabCtrl *pTabCtrl)
    {
	    ASSERT( pTabCtrl );

	    TRACE0( "Installing extended tab control\r\n" );
	    // Subclass the window, but only if it's not us
	    if ( pTabCtrl != this ) {
		    // Subclass the tab control
		    if ( !SubclassWindow( pTabCtrl->GetSafeHwnd() ) ) {
			    TRACE0( "Failed to subclass tab control\r\n" );
			    // Return failure
			    return FALSE;
		    }
	    }
	    // Get our current style
	    DWORD dStyle = pTabCtrl->GetStyle();
	
	    // Add the owner-drawn bit
	    //dStyle |= TCS_OWNERDRAWFIXED;
	    // Set the style
	    SetWindowLong( GetSafeHwnd(), GWL_STYLE, dStyle );

	    // Return success
	    return TRUE;
    }

    /*
     * Install
     *
     * Purpose:
     *   Installs the extended tab control
     *
     * Inputs:
     *   CPropertySheet *pPropSheet: The property sheet that contains
     *     the tab control that we will subclass
     *
     * Returns:
     *   TRUE if install succeeds, FALSE otherwise
     */
    BOOL CTabCtrlEx2::Install( CPropertySheet *pPropSheet )
    {
	    ASSERT( pPropSheet );

	    // Call the install function for the tab control
	    return( Install( pPropSheet->GetTabControl() ) );
    }

    /*
     * EnableItem
     *
     * Purpose:
     *   Enabled or disables an item in the tab control
     *
     * Inputs:
     *   int nItem: The item to enable or disable
     *   BOOL bEnable: TRUE to enable the item, FALSE to disable it
     *
     * Returns:
     *   TRUE if function succeeds, FALSE otherwise
     */
    BOOL CTabCtrlEx2::EnableItem( int nItem, BOOL bEnable )
    {
	    // Make sure the item is in our range
	    if ( nItem < 0 || nItem >= GetItemCount() ) {
		    // Return failure
		    return FALSE;
	    }

	    // See if this item is already in our disabled list
	    int idx = -1;
	    BOOL bIsEnabled = IsItemEnabled( nItem, &idx );

	    // Does the caller want to enable or disable this item?
	    // If our state is the same as what the user wants to do,
	    // then we're done
	    if ( bEnable == bIsEnabled ) {
		    // Return success
		    return TRUE;
	    }

	    // We come here if we have to add or remove from our list
	    if ( bEnable ) {
		    // Mark this item as enabled
		    ASSERT( idx >= 0 );
		    m_disabled.RemoveAt( idx );
	    }
	    else {
		    // Check if this item is the current item. If it is,
		    // then we select a new current item (if we can)
		    int curSel = GetCurSel();
		    if ( nItem == curSel ) {
			    // Find the next enabled item
			    int sel = FindNextEnabled( nItem < GetItemCount() - 1 ? nItem + 1 : 0, TRUE );
			    // If there isn't one or the next one is the
			    // one we're trying to disable, then we can't
			    // disable this item
			    if ( sel < 0 || sel == nItem) {
				    // We didn't find an enabled item, return failure
				    return FALSE;
			    }
			    // Set the current selection
    //			SetCurSel( sel );
			    SetCurrentTab( sel );
		    }
		    // Mark this item as disabled
		    m_disabled.Add( nItem );
	    }

	    // Invalidate the item
	    RECT rect;
	    GetItemRect( nItem, &rect );
	    InvalidateRect( &rect );
	    
	    // Return success
	    return TRUE;
    }

    /*
     * IsItemEnabled
     *
     * Purpose:
     *   Specifies whether an item is enabled or disabled
     *
     * Inputs:
     *   int nItem: The item whose enable state we will get
     *
     * Outputs:
     *   int *disabledIdx: The index of the item in the disabled list
     *
     * Returns:
     *   TRUE if the item is enabled, FALSE if it is not
     */
    BOOL CTabCtrlEx2::IsItemEnabled( int nItem, int *disabledIdx )
    {
	    // Make sure the item is in our range
	    if ( nItem < 0 || nItem >= GetItemCount() ) {
		    // Return failure
		    return FALSE;
	    }

	    // Find the item in our list
	    int dCount = m_disabled.GetSize();
	    for ( int idx = 0; idx < dCount; idx++ ) {
		    if ( m_disabled[idx] == (DWORD)nItem ) {
			    // Found it, return failure (indicates disabled)
			    if ( disabledIdx != NULL ) {
				    // Set the disabled index
				    *disabledIdx = idx;
			    }
			    // Return failure
			    return FALSE;
		    }
	    }
	    // Didn't find it, return success (indicates enabled)
	    return TRUE;
    }

    /*
     * SetItemImage
     *
     * Purpose:
     *   Adds an image into the tab control's image list and specifies
     *   the item that will use this image
     *
     * Inputs:
     *   int nItem: The item that will be associated with the image
     *   int nResBmp: The resource ID of the image bitmap
     *   int nResMsk: The resource ID of the image mask (also a bitmap resource)
     *
     * Returns:
     *   The index into the image list if successful, -1 otherwise
     */
    int CTabCtrlEx2::SetItemImage( int nItem, int nResBmp, int nResMsk )
    {
	    // Make sure the item is in our range
	    if ( nItem < 0 || nItem >= GetItemCount() ) {
		    // Return failure
		    return -1;
	    }

	    // Get the image list
	    CImageList *il = GetImageList();
	    if ( il == NULL ) {
		    TRACE0( "No image list defined\r\n" );
		    return -1;
	    }
	    // If the user only specified the bitmap and not the mask,
	    // we use the bitmap as the mask
	    if ( nResMsk == -1 ) {
		    TRACE0( "CTabCtrlEx2::SetItemImage - No mask specified so using bitmap as mask\r\n" );
		    nResMsk = nResBmp;
	    }

	    // Load the bitmap resource
	    CBitmap bitmap;
	    if ( !bitmap.LoadBitmap( nResBmp ) ) {
		    TRACE0( "Failed to load bitmap\r\n" );
		    // Return failure
		    return -1;
	    }
	    // Load the mask resource
	    CBitmap mask;
	    if ( !mask.LoadBitmap( nResMsk ) ) {
		    TRACE0( "Failed to load mask\r\n" );
		    // Delete the bitmap object
		    bitmap.DeleteObject();
		    // Return failure
		    return -1;
	    }

	    // Add this bitmap to our list
	    int result = il->Add( &bitmap, &mask );
	    
	    // Delete the bitmap object
	    bitmap.DeleteObject();
	    // Delete the mask object
	    mask.DeleteObject();

	    // Check to see if the image was added
	    if ( result == -1 ) {
		    TRACE1( "Failed to add bitmap and mask to image list for item %d\r\n", nItem );
		    // Return failure
		    return -1;
	    }
	    // Set the item's image index
	    TC_ITEM tci;
	    memset( &tci, 0, sizeof( TC_ITEM ) );
	    tci.mask = TCIF_IMAGE;
	    tci.iImage = result;
	    if ( !SetItem( nItem, &tci ) ) {
		    TRACE1( "Failed to set image for item %d\r\n", nItem );
		    // Return failure
		    return -1;
	    }
	    // Return the image index
	    return result;
    }

    /*
     * SetItemImage
     *
     * Purpose:
     *   Adds an image into the tab control's image list and specifies
     *   the item that will use this image
     *
     * Inputs:
     *   int nItem: The item that will be associated with the image
     *   HICON hIcon: An HICON to use as the image
     *
     * Returns:
     *   The index into the image list if successful, -1 otherwise
     */
    int CTabCtrlEx2::SetItemImage( int nItem, HICON hIcon )
    {
	    // Make sure the item is in our range
	    if ( nItem < 0 || nItem >= GetItemCount() ) {
		    // Return failure
		    return -1;
	    }

	    // Get the image list
	    CImageList *il = GetImageList();
	    if ( il == NULL ) {
		    TRACE0( "No image list defined\r\n" );
		    return -1;
	    }

	    // Add this bitmap to our list
	    int result = il->Add( hIcon );
	    // Check to see if the image was added
	    if ( result == -1 ) {
		    TRACE1( "Failed to add icon to image list for item %d\r\n", nItem );
		    // Return failure
		    return -1;
	    }
	    // Set the item's image index
	    TC_ITEM tci;
	    memset( &tci, 0, sizeof( TC_ITEM ) );
	    tci.mask = TCIF_IMAGE;
	    tci.iImage = result;
	    if ( !SetItem( nItem, &tci ) ) {
		    TRACE1( "Failed to set image for item %d\r\n", nItem );
		    // Return failure
		    return -1;
	    }
	    // Return the image index
	    return result;
    }

    void CTabCtrlEx2::OnDestroy() 
    {
	    CTabCtrl::OnDestroy();
    }
};


//---------------------------------
