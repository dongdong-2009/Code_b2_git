/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/SelectedDvaListCtrl.cpp $
  * @author Li Xiaoxia
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/09/24 17:39:15 $
  * Last modified by: $Author: huangjian $
  * 
  * Maintains a list control for Selected DVA messages
  *
  */

#include "stdafx.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/SelectedDvaListCtrl.h"
#include "app/pa/PAManager/src/StationBroadcastPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int SCROLL_TIMER_ID		= 1;

/////////////////////////////////////////////////////////////////////////////
// SelectedDvaListCtrl

SelectedDvaListCtrl::SelectedDvaListCtrl() :
DvaListCtrl( false ),
m_bSingleDva( false ),
m_iDropIndex(-1),
m_pDragImage(NULL),
m_iPrevDropIndex(-1),
m_uiPrevDropState(NULL),
m_uScrollTimer(0),
m_ScrollDirection(scrollNull),
m_dwStyle(NULL),
m_iDragIndex(-1)
{
}

SelectedDvaListCtrl::~SelectedDvaListCtrl()
{
	// Fail safe clean up.
	delete m_pDragImage;
	m_pDragImage = NULL;

	killScrollTimer();
}


BEGIN_MESSAGE_MAP(SelectedDvaListCtrl, DvaListCtrl)
	//{{AFX_MSG_MAP(SelectedDvaListCtrl)
	ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemchanging)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragDropListCtrl message handlers

void SelectedDvaListCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView)
	{
		m_iPrevDropIndex	= -1;
		m_uiPrevDropState	= NULL;
		
		// Items being dragged - can be one or more.
		//m_anDragIndexes.RemoveAll();
		POSITION pos = GetFirstSelectedItemPosition();

		m_iDragIndex = GetNextSelectedItem(pos);

		DWORD dwStyle = GetStyle();
		if ((dwStyle & LVS_SINGLESEL) == LVS_SINGLESEL)
		{
			// List control is single select; we need it to be multi-select so
			// we can show both the drag item and potential drag target as selected.
			m_dwStyle = dwStyle;
			ModifyStyle(LVS_SINGLESEL, NULL);
		}

		//if (m_anDragIndexes.GetSize() > 0)
		if(m_iDragIndex > -1)
		{
			// Create a drag image from the centre point of the item image.
			// Clean up any existing drag image first.
            if ( NULL != m_pDragImage )
            {
                delete m_pDragImage;
                m_pDragImage = NULL;
            }

			CPoint ptDragItem;
			m_pDragImage = createDragImageEx( ptDragItem );
			if ( NULL != m_pDragImage )
			{
				m_pDragImage->BeginDrag(0, ptDragItem);
				m_pDragImage->DragEnter(CWnd::GetDesktopWindow(), pNMListView->ptAction);

				// Capture all mouse messages in case the user drags outside the control.
				SetCapture();
			}
		}
	}
	
	*pResult = 0;
}

// Based on code by Frank Kobs.
CImageList* SelectedDvaListCtrl::createDragImageEx( CPoint& refPoint )
{
	CRect rectSingle(0, 0, 0, 0);	
	CRect rectComplete(0, 0, 0, 0);
	int	nIndex	= -1;
	BOOL bFirst	= TRUE;

	// Determine the size of the drag image.
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		nIndex = GetNextSelectedItem(pos);
		GetItemRect(nIndex, rectSingle, LVIR_BOUNDS);
		if (bFirst)
		{
			// Initialize the CompleteRect
			GetItemRect(nIndex, rectComplete, LVIR_BOUNDS);
			bFirst = FALSE;
		}
		rectComplete.UnionRect(rectComplete, rectSingle);
	}

	// Create bitmap in memory DC
	CClientDC dcClient(this);	
	CDC dcMem;
	CBitmap bmBackground;

	if (!dcMem.CreateCompatibleDC(&dcClient))
	{
		return NULL;
	}

	if (!bmBackground.CreateCompatibleBitmap(&dcClient, rectComplete.Width(), rectComplete.Height()))
	{
		return NULL;
	}

	CBitmap* pOldMemDCBitmap = dcMem.SelectObject(&bmBackground);
	// Here green is used as mask color.
	dcMem.FillSolidRect(0, 0, rectComplete.Width(), rectComplete.Height(), RGB(0, 255, 0)); 

	// Paint each DragImage in the DC.
	CImageList* pSingleImageList = NULL;
	CPoint pt;

	pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		nIndex = GetNextSelectedItem(pos);
		GetItemRect(nIndex, rectSingle, LVIR_BOUNDS);

		pSingleImageList = CreateDragImage(nIndex, &pt);
		if (pSingleImageList)
		{
			// Make sure width takes into account not using LVS_EX_FULLROWSELECT style.
			IMAGEINFO ImageInfo;
			pSingleImageList->GetImageInfo(0, &ImageInfo);
			rectSingle.right = rectSingle.left + (ImageInfo.rcImage.right - ImageInfo.rcImage.left);

			pSingleImageList->DrawIndirect(
				&dcMem, 
				0, 
				CPoint(rectSingle.left - rectComplete.left, 
				rectSingle.top - rectComplete.top),
				rectSingle.Size(), 
				CPoint(0,0));

			delete pSingleImageList;
		}
	}

	dcMem.SelectObject(pOldMemDCBitmap);

	// Create the imagelist	with the merged drag images.
	CImageList* pCompleteImageList = new CImageList();

	pCompleteImageList->Create(rectComplete.Width(), rectComplete.Height(), ILC_COLOR | ILC_MASK, 0, 1);
	// Here green is used as mask color.
	pCompleteImageList->Add(&bmBackground, RGB(0, 255, 0)); 

    bmBackground.DeleteObject();
    dcMem.DeleteDC();

	// As an optional service:
	// Find the offset of the current mouse cursor to the imagelist
	// this we can use in BeginDrag().
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);
	refPoint.x = ptCursor.x - rectComplete.left;
	refPoint.y = ptCursor.y - rectComplete.top;

    return pCompleteImageList;
}

// disables the selection of non-selectable items
void SelectedDvaListCtrl::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
    DvaListCtrl::OnItemchanging( pNMHDR, pResult );

    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

    *pResult = 0;

    // Only interested in state changes
    if (pNMListView->uChanged == LVIF_STATE) 
    {        
        // if it is being selected
        if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
            ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
        {
            // if the item is not selectable
            if ( !isSelectable( pNMListView->iItem ) )
            {
                // stop the change of state
                *pResult = -1;
                pNMListView->uNewState &= ~LVIS_SELECTED;
            }
        }
    }
}

void SelectedDvaListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( NULL != m_pDragImage )
	{
		// Must be dragging, as there is a drag image.

		// Move the drag image.
		CPoint ptDragImage(point);
		ClientToScreen(&ptDragImage);
		m_pDragImage->DragMove(ptDragImage);

		// Leave dragging so we can update potential drop target selection.
		m_pDragImage->DragLeave(CWnd::GetDesktopWindow());

		// Force x coordinate to always be in list control - only interested in y coordinate.
		// In effect the list control has captured all horizontal mouse movement.
		static const int nXOffset = 8;
		CRect rect;
		GetWindowRect(rect);
		CWnd* pDropWnd = CWnd::WindowFromPoint(CPoint(rect.left + nXOffset, ptDragImage.y));

		// Get the window under the drop point.
		if (pDropWnd == this)
		{
			// Still in list control so select item under mouse as potential drop target.
			point.x = nXOffset;	// Ensures x coordinate is always valid.
			updateSelection(HitTest(point));
		}

		CRect rectClient;
		GetClientRect(rectClient);
		CPoint ptClientDragImage(ptDragImage);
		ScreenToClient(&ptClientDragImage);

		// Client rect includes header height, so ignore it, i.e.,
		// moving the mouse over the header (and higher) will result in a scroll up.
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		if (pHeader)
		{
			CRect rectHeader;
			pHeader->GetClientRect(rectHeader);
			rectClient.top += rectHeader.Height();
		}

		if (ptClientDragImage.y < rectClient.top)
		{
			// Mouse is above the list control - scroll up.
			setScrollTimer(scrollUp);
		}
		else if (ptClientDragImage.y > rectClient.bottom)
		{
			// Mouse is below the list control - scroll down.
			setScrollTimer(scrollDown);
		}
		else
		{
			killScrollTimer();
		}

		// Resume dragging.
		m_pDragImage->DragEnter(CWnd::GetDesktopWindow(), ptDragImage);
	}
	else
	{
		killScrollTimer();
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void SelectedDvaListCtrl::updateSelection(int nDropIndex)
{
	if (nDropIndex > -1 && nDropIndex < GetItemCount())
	{
		// Drop index is valid and has changed since last mouse movement.
		restorePrevDropItemState();

		// Save information about current potential drop target for restoring next time round.
		m_iPrevDropIndex = nDropIndex;
		m_uiPrevDropState = GetItemState(nDropIndex, LVIS_SELECTED);

		// Select current potential drop target.
		SetItemState(nDropIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_iDropIndex = nDropIndex;		// Used by DropItem().

		UpdateWindow();
	}
}

void SelectedDvaListCtrl::restorePrevDropItemState()
{
	if (m_iPrevDropIndex > -1 && m_iPrevDropIndex < GetItemCount())
	{
		// Restore state of previous potential drop target.
		SetItemState(m_iPrevDropIndex, m_uiPrevDropState, LVIS_SELECTED);
	}
}

void SelectedDvaListCtrl::setScrollTimer(EScrollDirection ScrollDirection)
{
	if ( 0 == m_uScrollTimer )
	{
		m_uScrollTimer = SetTimer(SCROLL_TIMER_ID, 100, NULL);
	}
	m_ScrollDirection = ScrollDirection;
}

void SelectedDvaListCtrl::killScrollTimer()
{
	if ( 0 != m_uScrollTimer )
	{
		KillTimer( SCROLL_TIMER_ID );
		m_uScrollTimer = 0;
		m_ScrollDirection = scrollNull;
	}
}

void SelectedDvaListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( NULL != m_pDragImage )
	{
		killScrollTimer();

		// Release the mouse capture and end the dragging.
		::ReleaseCapture();
		m_pDragImage->DragLeave(CWnd::GetDesktopWindow());
		m_pDragImage->EndDrag();

		delete m_pDragImage;
		m_pDragImage = NULL;

		// Drop the item on the list.
		dropItem();
	}
	
	CListCtrl::OnLButtonUp(nFlags, point);
}

void SelectedDvaListCtrl::dropItem()
{
    try
    {
	    TA_IRS_Bus::StationDvaMessageRecord dragRecord = 
            TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(static_cast<unsigned long>(GetItemData(m_iDragIndex)));
        TA_IRS_Bus::StationDvaMessageRecord dropRecord = 
            TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(static_cast<unsigned long>(GetItemData(m_iDropIndex)));

	    updateFileds( m_iDropIndex, dragRecord );
        updateFileds( m_iDragIndex, dropRecord );
    }
    catch (...)
    {
        return;
    }

    if ( NULL != GetParent() )
    {
        GetParent()->PostMessage( WM_PA_SWITCH_SELECTED_DVA_MESSAGE, 
            static_cast<WPARAM>(m_iDragIndex), static_cast<LPARAM>(m_iDropIndex) );
    }

	if ( 0u != m_dwStyle )
	{
		// Style was modified, so return it back to original style.
		ModifyStyle( 0u, m_dwStyle );
		m_dwStyle = 0u;
	}
}

void SelectedDvaListCtrl::OnTimer(UINT nIDEvent) 
{
	if (( SCROLL_TIMER_ID == nIDEvent ) && ( NULL != m_pDragImage ))
	{
		WPARAM wParam	= NULL;
		int nDropIndex	= -1;
		if (m_ScrollDirection == scrollUp)
		{
			wParam = MAKEWPARAM(SB_LINEUP, 0);
			nDropIndex = m_iDropIndex - 1;
		}
		else if (m_ScrollDirection == scrollDown)
		{
			wParam = MAKEWPARAM(SB_LINEDOWN, 0);
			nDropIndex = m_iDropIndex + 1;
		}
		m_pDragImage->DragShowNolock(FALSE);
		SendMessage(WM_VSCROLL, wParam, NULL);
		updateSelection(nDropIndex);
		m_pDragImage->DragShowNolock(TRUE);
	}
	else
	{
		CListCtrl::OnTimer(nIDEvent);
	}
}

bool SelectedDvaListCtrl::isSelectable(int rowNumber)
{
    if( m_bSingleDva && ( rowNumber > 0 ))
    {
        return false;
    }
	else
	{
		return true;
	}
}

void SelectedDvaListCtrl::setSingleDva( bool bSingle )
{
    m_bSingleDva = bSingle;
}
