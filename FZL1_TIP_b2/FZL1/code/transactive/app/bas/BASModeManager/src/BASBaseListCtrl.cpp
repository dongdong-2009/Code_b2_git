// src\BASBaseListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/BASBaseListCtrl.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
Namespace_TA_IRS_APP_Begin
// BASBaseListCtrl
IMPLEMENT_DYNAMIC(BASBaseListCtrl, CListCtrl)
BASBaseListCtrl::BASBaseListCtrl()
	:m_rowFullSelectFlag(false)
{
}

BASBaseListCtrl::~BASBaseListCtrl()
{
}


BEGIN_MESSAGE_MAP(BASBaseListCtrl, CListCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void BASBaseListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if(IsWindow(GetSafeHwnd()))
	{
		if (false == m_rowFullSelectFlag)
		{
			setRowFullSelect();
			setDisplayGrid();
			m_rowFullSelectFlag = true;
		}

        //CRect listCtrlRect;
        //GetWindowRect(&listCtrlRect);
        //int listctrlWidth = listCtrlRect.Width();
        //if (0 == listctrlWidth) // for minisize state, no need to resize
        //{
        //    return;
        //}

		POINT pt;
		if (GetCursorPos(&pt))
		{
			CRect rectLst;
			GetWindowRect(&rectLst);
			if (rectLst.PtInRect(pt))
			{
				return;
			}			
		}
		resizeColumn();
	}	
}

DWORD BASBaseListCtrl::setRowFullSelect()
{
	LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
	style = SetWindowLong(this->m_hWnd, GWL_STYLE, style | LVS_SINGLESEL);
	return SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
}

DWORD BASBaseListCtrl::setDisplayGrid(bool bShow/* = true*/)
{
	if (false == bShow)
	{
		return SetExtendedStyle(GetExtendedStyle() & ~LVS_EX_GRIDLINES);
	}
	else
	{
		return SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES );
	}	
}

void BASBaseListCtrl::resizeColumn()
{
	SetRedraw(FALSE);
	CRect listCtrlRect;
	GetWindowRect(&listCtrlRect);
	int listctrlWidth = listCtrlRect.Width();

	bool displayAllColumn; //auto display the all column header
	int listctrlHeaderWidth = 0;
	int headerColumnCount = GetHeaderCtrl()->GetItemCount();
  	if (headerColumnCount > 0)
  	{
  		for (int index = 0; index < headerColumnCount; index++)
  		{
  			CRect headerItemRect;
  			GetHeaderCtrl()->GetItemRect(index, &headerItemRect);
  			listctrlHeaderWidth += headerItemRect.Width();
  		}
  	}

    listctrlHeaderWidth += 20;

	// change the column width
	int tempWidth = 0;
	for (int index = 0; index < headerColumnCount; index++)
	{
		CRect headerItemRect;
		GetHeaderCtrl()->GetItemRect(index, &headerItemRect);
		//tempWidth = headerItemRect.Width() + ((listctrlWidth - listctrlHeaderWidth) * ((double)headerItemRect.Width() / listctrlHeaderWidth));
		tempWidth = listctrlWidth * ((double)headerItemRect.Width() / listctrlHeaderWidth);
		
		if (tempWidth < 70)
		{
			tempWidth = 70;			
		}
		SetColumnWidth(index, tempWidth);
	}
	SetRedraw(TRUE);
}

void BASBaseListCtrl::resizeLastColumnWidth()
{
	int colCount = GetHeaderCtrl()->GetItemCount();
	SetColumnWidth(colCount - 1, LVSCW_AUTOSIZE_USEHEADER);
}

void BASBaseListCtrl::StartTimers()
{
}

void BASBaseListCtrl::StopTimers()
{
}
Namespace_TA_IRS_APP_End