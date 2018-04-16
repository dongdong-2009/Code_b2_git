#include "stdafx.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>   // MFC support for Windows Common Controls
#include "TisDialogCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTisDialogCtrl
namespace TA_IRS_App
{

    CTisDialogCtrl::CTisDialogCtrl()
    {
        // Initialise the list of dialog pages
        m_activeDialog = NULL;

    }

    CTisDialogCtrl::~CTisDialogCtrl()
    {
    }


    void CTisDialogCtrl::AddPage( CDialog&  dlg,UINT templateID,CString& label )
    {
        ASSERT ( dlg.m_hWnd==NULL );  // The dialog window should not be created yet
        ASSERT ( this->m_hWnd==NULL );  // These must be added before the control is drawn

        // Add to the list of dialogs
        DialogInfo   dlgInfo;
        dlgInfo.pDlg = &dlg;
        dlgInfo.nTemplateID = templateID;
        dlgInfo.label = label;
        m_dlgInfo.push_back( dlgInfo );
    }


    int CTisDialogCtrl::SetCurSel( int nItem )
    {
        int  iRet;

        // Do normal stuff
        iRet = CTabCtrl::SetCurSel( nItem );

        // Display associated dialog
        DisplayDialog( nItem );

        return iRet;
    }

    BEGIN_MESSAGE_MAP( CTisDialogCtrl, CTabCtrl )
        //{{AFX_MSG_MAP(CTisDialogCtrl)
        ON_NOTIFY_REFLECT_EX( TCN_SELCHANGE, OnSelchange )
        ON_WM_CREATE()
        ON_WM_DESTROY()
        ON_WM_PAINT()
        ON_WM_MOVE()
        ON_WM_SHOWWINDOW()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTisDialogCtrl message handlers

    void CTisDialogCtrl::OnPaint()
    {
        // Update the active dialog
        CTabCtrl::OnPaint();
        //if ( m_activeDialog!=NULL ) m_activeDialog->RedrawWindow();
    }

    BOOL CTisDialogCtrl::OnSelchange( NMHDR* pNMHDR, LRESULT* pResult )
    {
        // Display the matching dialog
        UINT newPage = this->GetCurSel();
        DisplayDialog( newPage );
        *pResult = 0;

        return TRUE;
    }



    int CTisDialogCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
    {
        if ( CTabCtrl::OnCreate( lpCreateStruct ) == -1 )
            return -1;

        // Create the associated modeless dialog windows
        CreateDialogs();

        return 0;
    }

    void CTisDialogCtrl::OnDestroy()
    {
        // Dispose of the modeless dialogs
        m_activeDialog = NULL;
        std::vector<DialogInfo>::iterator   it;
        for ( it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++ )
        {
            ( *it ).pDlg->DestroyWindow();
        }
        m_dlgInfo.clear();

        CTabCtrl::OnDestroy();
    }

    void CTisDialogCtrl::PreSubclassWindow()
    {
        // Perform normal stuff
        CTabCtrl::PreSubclassWindow();
        ModifyStyle(0, TCS_OWNERDRAWFIXED);
        // The parent will be the dialog which hosts the tab control
        CreateDialogs();
    }

	void CTisDialogCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{

        DRAWITEMSTRUCT& ds = *lpDrawItemStruct;
		
        int iItem = ds.itemID;
		
        // Get tab item info
        char text[128];
        TCITEM tci;
        tci.mask = TCIF_TEXT;
        tci.pszText = text;
        tci.cchTextMax = sizeof(text);
        GetItem(iItem, &tci);
		
        // use draw item DC
        CDC dc;
        dc.Attach(ds.hDC);
		
        // draw text background
        CRect bkRc = ds.rcItem;
        bkRc.top += ::GetSystemMetrics(SM_CYEDGE);
		bkRc.left += 1;
		bkRc.right -= 1;
        dc.SetBkMode(TRANSPARENT);
        dc.FillSolidRect(bkRc, ::GetSysColor(COLOR_BTNFACE));
		
        // calculate text rectangle
        CRect rc = ds.rcItem;
        rc += CPoint(0,3);

		// draw test
        dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
        dc.DrawText(text, &rc, DT_CENTER | DT_VCENTER);

        dc.Detach();
	}

    void CTisDialogCtrl::CreateDialogs()
    {
        int iTab = 0;

        // Create the associated modeless dialog windows
        CString       title;
        std::vector<DialogInfo>::iterator   it;
        for ( it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++ )
        {
            // Setup the dialog
            ( *it ).pDlg->Create( ( *it ).nTemplateID, this);
            if ( it==m_dlgInfo.begin() )
            {
                ( *it ).pDlg->ShowWindow( SW_NORMAL );
            }
            else
            {
                ( *it ).pDlg->ShowWindow( SW_HIDE );
            }

            // Add a tab to the control for the dialog
            InsertItem( iTab,( *it ).label );
            iTab++;
            ShowWindow( SW_NORMAL );

            // Save first page for current
            if ( it==m_dlgInfo.begin() )
            {
                m_activeDialog = ( *it ).pDlg;
            }
        }

        // Reset to first tab
        SetCurSel( 0 );

        ResizeDialogs();
    }

    void CTisDialogCtrl::ResizeDialogs()
    {
        std::vector<DialogInfo>::iterator   it;
        CRect tabRect, itemRect;
        int nX, nY, nXc, nYc;

        GetClientRect( &tabRect );
		GetItemRect( 0, &itemRect );
        nX=itemRect.left;
        nY=itemRect.bottom+1;
        nXc=tabRect.right-itemRect.left-1;
        nYc=tabRect.bottom-nY-1;

        for ( it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++ )
        {
			if(it == m_dlgInfo.begin())
			{
                ( *it ).pDlg->SetWindowPos( &wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
			}
			else
			{
                ( *it ).pDlg->SetWindowPos( &wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
			}
        }
    }


    void CTisDialogCtrl::OnMove( int x, int y )
    {
        CTabCtrl::OnMove( x, y );

        // Refresh the active dialogs as it may be exposed and not yet redrawn
        //if ( m_activeDialog!=NULL ) m_activeDialog->RedrawWindow();
    }

    /*
     * DisplayDialog
     *
     * This function will display the dialog associated with the specified tab,
     * hiding any currently displayed one.
     *
     */
    void CTisDialogCtrl::DisplayDialog( UINT tabPage )
    {
        std::vector<DialogInfo>::iterator   it;

        ASSERT ( tabPage<m_dlgInfo.size() );

        // Hide the last dialog (if any)
        if ( m_activeDialog!=NULL ) m_activeDialog->ShowWindow( SW_HIDE );

        // Activate the new dialog
        it = m_dlgInfo.begin();
        it = it + tabPage;
        ASSERT ( it<=m_dlgInfo.end() );
        ( *it ).pDlg->ShowWindow( SW_NORMAL );

        m_activeDialog = ( *it ).pDlg;
    }

    void CTisDialogCtrl::OnShowWindow( BOOL bShow, UINT nStatus )
    {
        CTabCtrl::OnShowWindow( bShow, nStatus );

        if ( !bShow )
        {
            // If this is a hide command, then hide all child dialogs

            std::vector<DialogInfo>::iterator   it;
            for ( it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++ )
            {
                ( *it ).pDlg->ShowWindow( SW_HIDE );
            }
        }
        else
        {
            // Only show the active dialog
            if ( m_activeDialog!=NULL ) m_activeDialog->ShowWindow( SW_NORMAL );
        }

    }
}