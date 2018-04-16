/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSOverallTimetableListCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/ECSOverallTimetableListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
	/////////////////////////////////////////////////////////////////////////////
	COverallECSTimetableListCtrl::COverallECSTimetableListCtrl()
	{
		m_SelectionFlag = FALSE;
		m_PrevItem=-1;
		m_iCurRow =-1;
		m_iCurCol =-1;
		m_ArialFont.CreateFont(16,0,0,0, FW_BOLD, FALSE,FALSE, FALSE, 0, 0, 0, 0, 0, "Arial");
	}

	COverallECSTimetableListCtrl::~COverallECSTimetableListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(COverallECSTimetableListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(COverallECSTimetableListCtrl)
		ON_WM_LBUTTONDOWN()		
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// COverallECSTimetableListCtrl message handlers
	
	void COverallECSTimetableListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
		TCHAR  lpBuffer[256];

		LV_ITEM lvi;

		lvi.mask = LVIF_TEXT | LVIF_PARAM ;
		lvi.iItem = lpDrawItemStruct->itemID ; 	
		lvi.iSubItem = 0;
		lvi.pszText = lpBuffer ;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
    
		LV_COLUMN lvc, lvcprev ;
		::ZeroMemory(&lvc, sizeof(lvc));
		::ZeroMemory(&lvcprev, sizeof(lvcprev));
		lvc.mask = LVCF_WIDTH | LVCF_FMT;
		lvc.fmt = LVCFMT_CENTER;
		lvcprev.mask = LVCF_WIDTH | LVCF_FMT;
		lvcprev.fmt = LVCFMT_CENTER;
		
		CDC* pDC;
		pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		int nCol;
		CPen whitePen;
		whitePen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		RowHighlight *pLight = (RowHighlight *)GetItemData(lpDrawItemStruct->itemID);
		for(nCol=0;GetColumn(nCol,&lvc);nCol++)
		{
			if(nCol>0)
			{
				GetSubItemRect(lpDrawItemStruct->itemID, nCol,LVIR_BOUNDS, m_SelectionRect);
			}
			else
			{
				GetItemRect(lpDrawItemStruct->itemID, m_SelectionRect,LVIR_BOUNDS);
				if(m_SelectionRect.PtInRect(m_Point))
				{
					pDC->DrawFocusRect(&m_SelectionRect);					
					m_SelectionRect.DeflateRect(1,1,1,1);
					
					//zn++ CL-17265 ITEM 2
					//pDC->FillSolidRect(&m_SelectionRect, RGB(0,0,255));	//TD17265 - The highlighted colour should be lighter.
					CPen newPen(PS_SOLID, 3, RGB(0,0,255));
					CPen* oldPen = pDC->SelectObject(&newPen);
					
					pDC->SelectStockObject(NULL_BRUSH);
					pDC->Rectangle(&m_SelectionRect);
					
					pDC->SelectObject(oldPen);
					
					// Turn selecting back on if necessary
					SetItemState(lpDrawItemStruct->itemID, LVIS_SELECTED, LVIS_SELECTED);

					ValidateRect(&m_SelectionRect);
					//++zn 
					
				}
			
				m_SelectionRect.right = GetColumnWidth(0);
				m_SelectionRect.left = 0;
			}
			
			CPoint center = m_SelectionRect.CenterPoint();
			center.x      -= 8;
			center.y      -= 8;
			
			
			if(pLight && nCol >0)
			{
				if(pLight->iCurrentSelection == nCol)
				{
					m_StationSelectionImageList.Draw (pDC, 0, center,
								ILD_TRANSPARENT );	
				}
				
				if(pLight->iClickedSelection == nCol && 
				   pLight->iCurrentSelection != pLight->iClickedSelection)
				{
					m_StationSelectionImageList.Draw ( pDC, 1, center,
								ILD_TRANSPARENT );	
				}
			}		
					
		}

			
		GetItemRect(lpDrawItemStruct->itemID, m_SelectionRect,LVIR_BOUNDS);
		m_SelectionRect.right = GetColumnWidth(0);
		m_SelectionRect.left = 0;
		CFont *pOldFont = pDC->SelectObject(&m_ArialFont);		
		CPen  *pOldPen1  = pDC->SelectObject(&whitePen);
		UINT		uFormat    = DT_CENTER ;		
		::DrawText(lpDrawItemStruct->hDC, lpBuffer, strlen(lpBuffer), 
			m_SelectionRect, uFormat) ;		
		pDC->SelectObject(pOldFont) ;	
		
			
	}
	

	void COverallECSTimetableListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		m_Point = point;
		m_SelectionFlag = FALSE;	
		GetRowColumn();
		Invalidate();	
		CListCtrl::OnLButtonDown(nFlags, point);
	}
	
	void COverallECSTimetableListCtrl::GetRowColumn()
	{	
		m_iCurRow = m_iCurCol = -1;
		BOOL bFound = FALSE;
		LV_COLUMN lvc;
		::ZeroMemory(&lvc, sizeof(lvc));	
		lvc.mask = LVCF_WIDTH | LVCF_FMT;	

		for(int nRow = 0; nRow < GetItemCount(); ++ nRow)
		{
			for (int nCol=0; GetColumn(nCol, &lvc); nCol++)
			{
			
				if(nCol>0)
				{
					GetSubItemRect(nRow, nCol,LVIR_BOUNDS, m_SelectionRect);
				}
				else
				{
					GetItemRect(nRow, m_SelectionRect,LVIR_BOUNDS);
					m_SelectionRect.right = GetColumnWidth(0);
					m_SelectionRect.left = 0;
				}
			
				if(m_SelectionRect.PtInRect(m_Point))
				{
					RowHighlight *pLight = (RowHighlight *)GetItemData(nRow);
					if(pLight && nCol >0)
					{
						if(pLight->iCurrentSelection != nCol)
						{
							if(pLight->iClickedSelection == nCol)
								pLight->iClickedSelection = -1;
							else
								pLight->iClickedSelection = nCol;
						}
					}
					m_iCurRow = nRow;
					m_iCurCol = nCol;
					bFound = TRUE;
					break;
				}
			}
			if(bFound == TRUE)
				break;
		}
		
		
	}

	void COverallECSTimetableListCtrl::SelectColumn(int iColumn)
	{
		for(int nRow=0; nRow < GetItemCount(); nRow++)
		{
			RowHighlight *pLight = (RowHighlight *)GetItemData(nRow);
			if(pLight && iColumn >0 && pLight->iCurrentSelection != iColumn)
			{
				pLight->iClickedSelection = iColumn;
			}
			//zn++ CL-17265 ITEM 1
			else if (pLight && iColumn > 0 
				&& pLight->iCurrentSelection == iColumn 
				&& pLight->iClickedSelection != iColumn)
			{
				pLight->iClickedSelection = -1;
			}
			//++zn
		}

		Invalidate();
	}


	void COverallECSTimetableListCtrl::ClearClickedItems()
	{
		for(int nRow=0; nRow < GetItemCount(); nRow++)
		{
			RowHighlight *pLight = (RowHighlight *)GetItemData(nRow);
			if(pLight)
			{
				pLight->iClickedSelection = -1;
			}
		}

		Invalidate();
	}
			
	
}
