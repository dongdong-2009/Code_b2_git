// MyTreeCtrl.cpp : implementation file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "DragTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl

DragTreeCtrl::DragTreeCtrl()
{
    m_bDragging = FALSE;
    m_pimagelist = NULL;
}

DragTreeCtrl::~DragTreeCtrl()
{
}


BEGIN_MESSAGE_MAP( DragTreeCtrl, CTreeCtrl )
    //{{AFX_MSG_MAP(CMyTreeCtrl)
    ON_NOTIFY_REFLECT( TVN_BEGINDRAG, OnBeginDrag )
    ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnCustomDraw )
    ON_WM_MOUSEMOVE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl message handlers
void DragTreeCtrl::OnDestroy()
{
    CTreeCtrl::OnDestroy();
}

void DragTreeCtrl::SetNewStyle( long lStyleMask, BOOL bSetBits )
{
    long lStyleOld = GetWindowLong( m_hWnd, GWL_STYLE );
    lStyleOld &= ~lStyleMask;
    if( bSetBits )
    {
        lStyleOld |= lStyleMask;
    }

    SetWindowLong( m_hWnd, GWL_STYLE, lStyleOld );
    SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
}

void DragTreeCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
    HTREEITEM hitem = NULL;
    UINT flags = 0;

    if( m_bDragging && ( m_pimagelist != NULL ))
    {
        m_pimagelist->DragMove( point );
        if( ( hitem = HitTest( point, &flags ) ) != NULL )
        {
            //m_pimagelist->DragLeave(CWnd::GetDesktopWindow());
            //m_pimagelist->DragEnter(CWnd::GetDesktopWindow(), point);
        }
    }

    CTreeCtrl::OnMouseMove( nFlags, point );
}

void DragTreeCtrl::OnButtonUp()
{
    if( m_bDragging )
    {
        if ( m_pimagelist != NULL )
        {
            m_pimagelist->DragLeave(this);
            m_pimagelist->EndDrag();

            delete m_pimagelist;
            m_pimagelist = NULL;
        }

        ::ReleaseCapture();
        m_bDragging = FALSE;
        //SelectDropTarget(NULL);
        GetParent()->PostMessage( WM_TREE_DRAG_RELEASE, static_cast<WPARAM>( GetItemData( m_hitemDrag ) ), 0 );
    }
}

void DragTreeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
    OnButtonUp();
    CTreeCtrl::OnLButtonUp( nFlags, point );
}

void DragTreeCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
    OnButtonUp();
    CTreeCtrl::OnRButtonUp( nFlags, point );
}

void DragTreeCtrl::OnBeginDrag( LPNMHDR pnmhdr, LRESULT* pLResult )
{
    if ( m_bDragging )
    {
        return;
    }

    CPoint ptAction;
    UINT nFlags = 0;

    ::GetCursorPos( &ptAction );
    ScreenToClient( &ptAction );

    m_hitemDrag = HitTest( ptAction, &nFlags );

    if (ItemHasChildren(m_hitemDrag))
    {
        m_hitemDrag = NULL;
        return;
    }

    m_bDragging = TRUE;

    if ( m_pimagelist != NULL )
    {
        delete m_pimagelist;
        m_pimagelist = NULL;
    }

    m_pimagelist = CreateDragImageEx(m_hitemDrag); // get the image list for dragging

    CFont* pOldFont = GetFont();
    SetFont( GetParent()->GetFont() );
    SetFont( pOldFont );
    if( NULL != m_pimagelist )
    {
        m_pimagelist->DragShowNolock( TRUE );
        //m_pimagelist->SetDragCursorImage(0, ptAction);
        m_pimagelist->BeginDrag( 0, CPoint( 0, 0 ) );
        //m_pimagelist->DragMove(ptAction);
        ::GetCursorPos( &ptAction );
        GetParent()->ScreenToClient( &ptAction );
        m_pimagelist->DragEnter( GetParent(), ptAction );
        SetCapture();
    }

    *pLResult = 0;
}

CImageList* DragTreeCtrl::CreateDragImageEx( const HTREEITEM hItem )
{
    CRect rectComplete( 0, 0, 0, 0 );

    // Initialize the CompleteRect
    GetItemRect( hItem, rectComplete, LVIR_BOUNDS );

    // Create bitmap in memory DC
    CClientDC dcClient( this );
    CDC dcMem;
    CBitmap bmBackground;

    if( !dcMem.CreateCompatibleDC( &dcClient ) )
    {
        return NULL;
    }

    if( !bmBackground.CreateCompatibleBitmap( &dcClient, rectComplete.Width(), rectComplete.Height() ) )
    {
        return NULL;
    }

    CBitmap* pOldMemDCBitmap = dcMem.SelectObject( &bmBackground );
    // Here green is used as mask color.
    dcMem.FillSolidRect( 0, 0, rectComplete.Width(), rectComplete.Height(), GetBkColor() );

    TVITEM tvi;
    memset(&tvi, 0, sizeof(TVITEM));
    tvi.mask = TVIF_IMAGE | TVIF_TEXT;
    tvi.hItem = hItem;
    TreeView_GetItem(m_hWnd, &tvi);

    HGDIOBJ bmp = reinterpret_cast<HGDIOBJ>(tvi.iImage);
    BITMAP bitmap;
    ::GetObject(bmp, sizeof(bitmap), &bitmap);

    CDC memDc;
    memDc.CreateCompatibleDC( &dcMem );
    memDc.SelectObject(bmp);
    int nOldBltMode = dcMem.SetStretchBltMode( HALFTONE );
    dcMem.StretchBlt(0, 0, rectComplete.Height(), rectComplete.Height(), &memDc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
    dcMem.SetStretchBltMode( nOldBltMode );

    CString strItem = GetItemText(hItem);
    // Paint DragImage in the DC.
    dcMem.TextOut(rectComplete.Height() + 2, 0, strItem);

    //dcMem.SelectObject(pOldMemDCBitmap);

    // Create the imagelist with the merged drag images.
    CImageList* pCompleteImageList = new CImageList();

    pCompleteImageList->Create( rectComplete.Width(), rectComplete.Height(), ILC_COLOR | ILC_MASK, 0, 1 );
    // Here green is used as mask color.
    pCompleteImageList->Add( &bmBackground, RGB( 0, 255, 0 ) );

    bmBackground.DeleteObject();
    dcMem.DeleteDC();
    memDc.DeleteDC();

    // As an optional service:
    // Find the offset of the current mouse cursor to the imagelist
    // this we can use in BeginDrag().

    return pCompleteImageList;
}

void DragTreeCtrl::DrawChlItem( CDC* pDC, HTREEITEM hTreeItem, CRect itemRect )
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_IMAGE;
    tvi.hItem = hTreeItem;
    TreeView_GetItem( m_hWnd, &tvi );

    HBITMAP hbmp = reinterpret_cast<HBITMAP>(tvi.iImage);
	BITMAP bitmap;
	::GetObject(hbmp, sizeof(bitmap), &bitmap);

	CDC memDc;
    memDc.CreateCompatibleDC( pDC );
	::SelectObject(memDc.m_hDC, hbmp);

	int strechedSz = itemRect.Height() - 2;
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(itemRect.left + 5, itemRect.top, strechedSz, strechedSz, &memDc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

    memDc.DeleteDC();
}

void DragTreeCtrl::DrawParItem( CDC* pDC, HTREEITEM hTreeItem, CRect itemRect )
{
    int midHeight = itemRect.top + itemRect.Height() / 2;
	CPoint ptArray[3];

	CPen pen(PS_SOLID, 1, RGB(169, 169, 169));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush brush(RGB(169, 169, 169));
	CBrush* pOldBrush = pDC->SelectObject(&brush);

    if( GetItemState( hTreeItem, TVIS_EXPANDED ) & TVIS_EXPANDED )
    {
        ptArray[0] = CPoint( itemRect.left + 2, midHeight - 3 );
        ptArray[1] = CPoint( itemRect.left + 8, midHeight + 3 );
        ptArray[2] = CPoint( itemRect.left + 14, midHeight - 3 );
    }
    else
    {
        ptArray[0] = CPoint( itemRect.left + 5, midHeight - 6 );
        ptArray[1] = CPoint( itemRect.left + 5, midHeight + 6 );
        ptArray[2] = CPoint( itemRect.left + 12, midHeight );
    }

	pDC->Polygon(ptArray, 3);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void DragTreeCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTVCUSTOMDRAW* ptvTreeCtrl = ( NMTVCUSTOMDRAW* )pNMHDR;
    if( ptvTreeCtrl->nmcd.dwDrawStage == CDDS_PREPAINT )
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;
    }
    if( ptvTreeCtrl->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
    {
        CDC dc;
        dc.Attach( ptvTreeCtrl->nmcd.hdc );

        CRect itemRect( ptvTreeCtrl->nmcd.rc );
        CPoint ptItem( itemRect.left + 1, itemRect.top + 1 );
        HTREEITEM hTreeItem = HitTest( ptItem );
        BOOL isSelected = ptvTreeCtrl->nmcd.uItemState & CDIS_SELECTED;
        CString strItem = GetItemText( hTreeItem );
        if( !isSelected )
        {
            dc.FillSolidRect( itemRect, RGB( 221, 221, 221 ) );
        }

        HTREEITEM parent = GetParentItem( hTreeItem );

        if( ItemHasChildren( hTreeItem ) || 0 == parent )
        {
            DrawParItem( &dc, hTreeItem, itemRect );
            itemRect.left += 20;
        }
        else
        {
            DrawChlItem( &dc, hTreeItem, itemRect );
            itemRect.left += 33;
        }
        dc.SetTextColor( RGB( 0, 0, 0 ) );
        dc.SetBkMode( TRANSPARENT );

        dc.DrawText( strItem, itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        dc.Detach();
        *pResult = CDRF_SKIPDEFAULT;

        return;
    }
}

BOOL DragTreeCtrl::OnEraseBkgnd( CDC* pDC )
{
    CRect cliRect;
    GetClientRect( &cliRect );
    pDC->FillSolidRect( &cliRect, RGB( 221, 221, 221 ) );
    return TRUE;
}
