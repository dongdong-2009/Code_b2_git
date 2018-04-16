// WorkflowView.cpp : implementation file
//

#include "stdafx.h"

#include <cmath>

#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/DrawingContext.h"
#include "WorkflowEditor.h"
#include "WorkflowDiagram.h"
#include "WorkflowViewer.h"


// WorkflowView

WorkflowViewer::WorkflowViewer( WorkflowDiagram& refDiagram ) :
CScrollView(),
IDiagramViewer(),
m_refDiagram( refDiagram ),
m_fZoomScale( 1.0f ),
m_bDrawSelection( false )
{
    SetScrollSizes(MM_TEXT, CSize(0, 0));
    m_refDiagram.attachViewer( *this );
}

WorkflowViewer::~WorkflowViewer()
{
}


BEGIN_MESSAGE_MAP(WorkflowViewer, CScrollView)
    ON_WM_SIZE()
    ON_WM_MOUSEACTIVATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// WorkflowView drawing

void WorkflowViewer::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

    SetFont( GetParent()->GetFont() );

    resyncScrollSizes();
}

BOOL WorkflowViewer::OnEraseBkgnd(CDC* pDC) 
{
    return FALSE;
}

void WorkflowViewer::pagePtToTotalPt(CPoint& ptPage)
{
    CPoint ptScroll = GetScrollPosition();

    ptPage.x += ptScroll.x;
    ptPage.y += ptScroll.y;

    ptPage.x = static_cast<int>(static_cast<float>(ptPage.x) / m_fZoomScale);
    ptPage.y = static_cast<int>(static_cast<float>(ptPage.y) / m_fZoomScale);
}

void WorkflowViewer::OnDraw(CDC* pDC)
{
    // Update DC to support zoom
    int nOldMapMode = pDC->SetMapMode( MM_ISOTROPIC );
    CSize szOldWinExt = pDC->SetWindowExt( 100, 100 );
    CSize szOldViewExt = pDC->SetViewportExt( static_cast<int>(100.0f * m_fZoomScale), static_cast<int>(100.0f * m_fZoomScale) );

    // Create bitmap in memory DC
    CDC dcMem;
    CBitmap bmBackground;

    if (!dcMem.CreateCompatibleDC( pDC ))
    {
        return;
    }

    CRect rtDrawBound;
    pDC->GetClipBox(&rtDrawBound);
    pDC->LPtoDP(&rtDrawBound);
    if (!bmBackground.CreateCompatibleBitmap( pDC, rtDrawBound.Width(), rtDrawBound.Height() ))
    {
        return;
    }

    HGDIOBJ hOldBitmap = dcMem.SelectObject(bmBackground.GetSafeHandle());
    dcMem.SetMapMode(pDC->GetMapMode());
    dcMem.SetWindowExt(pDC->GetWindowExt());
    dcMem.SetViewportExt(pDC->GetViewportExt());

    pDC->DPtoLP(&rtDrawBound);
    dcMem.SetWindowOrg(rtDrawBound.left, rtDrawBound.top);
    dcMem.FillSolidRect(rtDrawBound, pDC->GetBkColor());

    DrawingContext dcDraw( dcMem );
    m_refDiagram.refresh( dcDraw );

    pDC->BitBlt( rtDrawBound.left, rtDrawBound.top, rtDrawBound.Width(), rtDrawBound.Height(), &dcMem, rtDrawBound.left, rtDrawBound.top, SRCCOPY );

    // Reset dc object and delete handle
    dcMem.SelectObject(hOldBitmap);
    dcMem.DeleteDC();
    bmBackground.DeleteObject();

    // Restore DC
    pDC->SetMapMode( nOldMapMode );
    pDC->SetWindowExt( szOldWinExt );
    pDC->SetViewportExt( szOldViewExt );
}

void WorkflowViewer::OnSize(UINT nType, int cx, int cy) 
{
    resyncScrollSizes();        // ensure that scroll info is up-to-date
    CScrollView::OnSize(nType, cx, cy);
}

void WorkflowViewer::resyncScrollSizes()
{
    CSize sizeTotal;

    sizeTotal.cx = m_refDiagram.Width;
    sizeTotal.cy = m_refDiagram.Height;

    if ( sizeTotal.cx < 1600)
    {
        sizeTotal.cx = 1600;
    }
    if ( sizeTotal.cy < 1600 )
    {
        sizeTotal.cy = 1600;
    }

    sizeTotal.cx = static_cast<int>(static_cast<float>(sizeTotal.cx) * m_fZoomScale);
    sizeTotal.cy = static_cast<int>(static_cast<float>(sizeTotal.cy) * m_fZoomScale);

    SetScrollSizes(MM_TEXT, sizeTotal);
}

void WorkflowViewer::OnLButtonDown( UINT nFlags, CPoint ptCursor )
{
    if ( m_refDiagram.ReadOnly )
    {
        return;
    }

    m_rtSelection.SetRect( ptCursor.x, ptCursor.y, ptCursor.x, ptCursor.y );

    pagePtToTotalPt( ptCursor );

    bool bEnableMultiple = (0 != (nFlags & (MK_SHIFT | MK_CONTROL)));
    m_bDrawSelection = m_refDiagram.buttonDown( bEnableMultiple, ptCursor.x, ptCursor.y );

    m_ptMouseDown = ptCursor;

    if ( m_refDiagram.elementSelected() || m_bDrawSelection )
    {
        SetCapture();
    }
}

void WorkflowViewer::OnLButtonUp( UINT nFlags, CPoint ptCursor )
{
    if ( m_refDiagram.ReadOnly )
    {
        return;
    }

    pagePtToTotalPt( ptCursor );

    bool bEnableMultiple = (0 != (nFlags & (MK_SHIFT | MK_CONTROL)));
    m_refDiagram.buttonUp(ptCursor.x, ptCursor.y);

    if ( m_refDiagram.elementSelected() || m_bDrawSelection )
    {
        ReleaseCapture();
    }

    if ( m_bDrawSelection )
    {
        int nTop = __min( m_rtSelection.top, m_rtSelection.bottom );
        int nLeft = __min( m_rtSelection.left, m_rtSelection.right );
        int nBottom = __max( m_rtSelection.top, m_rtSelection.bottom );
        int nRight = __max( m_rtSelection.left, m_rtSelection.right );

        CPoint ptTopLeft( nLeft, nTop );
        pagePtToTotalPt( ptTopLeft );

        nLeft = ptTopLeft.x;
        nTop = ptTopLeft.y;

        CPoint ptBottomRight( nRight, nBottom );
        pagePtToTotalPt( ptBottomRight );

        nRight = ptBottomRight.x;
        nBottom = ptBottomRight.y;

        m_refDiagram.selectArea( bEnableMultiple, nLeft, nTop, nRight, nBottom );
    }

    if ( m_bDrawSelection )
    {
        m_bDrawSelection = false;

        drawSelection();
    }
}

void WorkflowViewer::OnMouseLeave()
{
    if ( m_refDiagram.ReadOnly )
    {
        return;
    }

    if ( !m_refDiagram.elementSelected() || !m_bDrawSelection )
    {
        return;
    }

    ReleaseCapture();
}

void WorkflowViewer::OnMouseMove( UINT nFlag, CPoint ptCursor )
{
    if ( m_refDiagram.ReadOnly )
    {
        return;
    }

    if ( MK_LBUTTON != ( MK_LBUTTON & nFlag ))
    {
        return;
    }

    if ( m_bDrawSelection )
    {
        drawSelection();

        m_rtSelection.right = ptCursor.x;
        m_rtSelection.bottom = ptCursor.y;

        drawSelection();
    }
    else
    {
        pagePtToTotalPt( ptCursor );

        if ( m_refDiagram.moveElements( ptCursor.x - m_ptMouseDown.x, ptCursor.y - m_ptMouseDown.y ) )
        {
            m_ptMouseDown = ptCursor;
        }
    }
}

void WorkflowViewer::diagramUpdated()
{
    Invalidate( TRUE );
}

void WorkflowViewer::elementAdded( const unsigned int unId )
{
    Invalidate( TRUE );
}

void WorkflowViewer::elementDeleted( const unsigned int unId )
{
    Invalidate( TRUE );
}

void WorkflowViewer::elementUpdated( const unsigned int unId )
{
    Invalidate( TRUE );
}

void WorkflowViewer::elementSelected( const unsigned int unId, const std::vector<unsigned int>& vecDeselectedIds )
{
    Invalidate( TRUE );

    GetParent()->PostMessage(WM_ACTIVITY_CLICKED, static_cast<WPARAM>(unId), 0);
}

void WorkflowViewer::elementsSelected( const std::vector<unsigned int>& vecIds, const std::vector<unsigned int>& vecDeselectedIds )
{
    Invalidate( TRUE );
}

void WorkflowViewer::fitToSize( SIZE& szTotal )
{
    m_szOrgTotal = m_totalDev;

    // Note: It is possible to set sizeTotal members to negative values to
    //  effectively invert either the X or Y axis.

    m_nMapMode = -1;     // special internal value
    m_totalLog = szTotal;

    // reset and turn any scroll bars off
    if ( m_hWnd != NULL && (GetStyle() & (WS_HSCROLL | WS_VSCROLL)) )
    {
        SetScrollPos( SB_HORZ, 0 );
        SetScrollPos( SB_VERT, 0 );
        //EnableScrollBarCtrl( SB_BOTH, FALSE );
    }

    CRect rectT;
    GetClientRect( rectT );
    m_totalDev = rectT.Size();

    if ( m_hWnd != NULL )
    {
        // window has been created, invalidate
        //UpdateBars();
        Invalidate( TRUE );
    }
}

void WorkflowViewer::zoomIn()
{
    m_fZoomScale += 0.1f;

    if ( m_fZoomScale >= 1.3f )
    {
        m_fZoomScale = 1.3f;
    }

    resyncScrollSizes();

    Invalidate( TRUE );
}

void WorkflowViewer::zoomOut()
{
    m_fZoomScale -= 0.1f;

    if ( m_fZoomScale <= 0.4f )
    {
        m_fZoomScale = 0.4f;
    }

    resyncScrollSizes();

    Invalidate( TRUE );
}

void WorkflowViewer::actualZoom()
{
    m_fZoomScale = 1.0f;

    resyncScrollSizes();

    Invalidate( TRUE );
}

void WorkflowViewer::fitToWin()
{
    CRect rtClient;
    GetClientRect( &rtClient );

    CSize szScrollbar;
    GetScrollBarSizes( szScrollbar );

    int nDiagramWidth = 0;
    int nDiagramHeight = 0;

    m_refDiagram.size( nDiagramWidth, nDiagramHeight );

    float fWidthRatio = static_cast<float>(rtClient.Width() - szScrollbar.cy) / static_cast<float>(nDiagramWidth);
    float fHeightRatio = static_cast<float>(rtClient.Height() - szScrollbar.cy) / static_cast<float>(nDiagramHeight);

    m_fZoomScale = fWidthRatio < fHeightRatio ? fWidthRatio : fHeightRatio;
    m_fZoomScale = std::floor( m_fZoomScale * 10.0f ) / 10.0f;

    resyncScrollSizes();

    Invalidate( TRUE );
}

void WorkflowViewer::drawSelection()
{
    CClientDC dcDraw( this );
    int nOldRop = dcDraw.SetROP2( R2_NOT );
    dcDraw.MoveTo( m_rtSelection.left, m_rtSelection.top );
    dcDraw.LineTo( m_rtSelection.right, m_rtSelection.top );
    dcDraw.LineTo( m_rtSelection.right, m_rtSelection.bottom );
    dcDraw.LineTo( m_rtSelection.left, m_rtSelection.bottom );
    dcDraw.LineTo( m_rtSelection.left, m_rtSelection.top );
    dcDraw.SetROP2( nOldRop );
}

void WorkflowViewer::screenToDiagram( CPoint& ptInOut )
{
    ScreenToClient( &ptInOut );
    pagePtToTotalPt( ptInOut );
}

// Override CView method to avoid ASSERT
int WorkflowViewer::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

    if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
    {
        return nResult;   // frame does not want to activate
    }

    CFrameWnd* pParentFrame = GetParentFrame();
    if (pParentFrame != NULL)
    {
        // either re-activate the current view, or set this view to be active
        CView* pView = pParentFrame->GetActiveView();
        HWND hWndFocus = ::GetFocus();
        if (pView == this && m_hWnd != hWndFocus && !::IsChild(m_hWnd, hWndFocus))
        {
            // re-activate this view
            OnActivateView(TRUE, this, this);
        }
    }

    return nResult;
}
// WorkflowView message handlers
