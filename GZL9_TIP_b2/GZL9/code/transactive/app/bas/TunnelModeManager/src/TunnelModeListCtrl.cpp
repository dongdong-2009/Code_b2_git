// src\TunnelModeListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeListCtrl.h"

namespace TA_IRS_App
{
	// CTunnelModeListCtrl
	const std::string CTunnelModeListCtrl::ColumnName_ModeType = "类型";
	const std::string CTunnelModeListCtrl::ColumnName_ModeDesc = "模式描述";
	const std::string CTunnelModeListCtrl::ColumnName_LeftStation = "上行车站";
	const std::string CTunnelModeListCtrl::ColumnName_RightStation = "下行车站";
	const unsigned long CTunnelModeListCtrl::ColumnIndex_ModeType = 0;
	const unsigned long CTunnelModeListCtrl::ColumnIndex_ModeDesc = 1;
	const unsigned long CTunnelModeListCtrl::ColumnIndex_LeftStation = 2;
	const unsigned long CTunnelModeListCtrl::ColumnIndex_RightStation = 3;
	IMPLEMENT_DYNAMIC(CTunnelModeListCtrl, CListCtrl)
	CTunnelModeListCtrl::CTunnelModeListCtrl()
	{

	}

	CTunnelModeListCtrl::~CTunnelModeListCtrl()
	{
	}


	BEGIN_MESSAGE_MAP(CTunnelModeListCtrl, CListCtrl)
        /*ON_WM_SIZE()*/
	END_MESSAGE_MAP()



	// CTunnelModeListCtrl message handlers
	void CTunnelModeListCtrl::SetupListCtrl()
	{
		setRowFullSelect();
		InsertColumn(ColumnIndex_ModeType, ColumnName_ModeType.c_str(), LVCFMT_CENTER, 250);
		InsertColumn(ColumnIndex_ModeDesc, ColumnName_ModeDesc.c_str(), LVCFMT_LEFT, 500);
		InsertColumn(ColumnIndex_LeftStation, ColumnName_LeftStation.c_str(), LVCFMT_CENTER, 150);
		InsertColumn(ColumnIndex_RightStation, ColumnName_RightStation.c_str(), LVCFMT_CENTER, 150);
	}

	void CTunnelModeListCtrl::InsertItemTexts(const CString& modeType, const CString& modeDesc, const CString& leftBound, const CString rightBound, DWORD itemData)
	{
		int index = this->GetItemCount();
		index = InsertItem(index, modeType);
		SetItemText(index, ColumnIndex_ModeDesc, modeDesc);
		SetItemText(index, ColumnIndex_LeftStation, leftBound);
		SetItemText(index, ColumnIndex_RightStation, rightBound);
		SetItemData(index, itemData);
	}

	DWORD CTunnelModeListCtrl::setRowFullSelect()
	{
		LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
		style = SetWindowLong(this->m_hWnd, GWL_STYLE, style | LVS_SINGLESEL);
		return SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	}

    //void CTunnelModeListCtrl::OnSize( UINT nType, int cx, int cy )
    //{
    //    __super::OnSize(nType, cx, cy);


    //    // reset column width
    //    SetRedraw(FALSE);
    //    CRect listCtrlRect;
    //    GetWindowRect(&listCtrlRect);
    //    int listctrlWidth = listCtrlRect.Width();

    //    if ( 0 == listctrlWidth )
    //    {
    //        return;
    //    }

    //    bool displayAllColumn; //auto display the all column header
    //    int listctrlHeaderWidth = 0;
    //    int headerColumnCount = GetHeaderCtrl()->GetItemCount();
    //    if (headerColumnCount > 0)
    //    {
    //        for (int index = 0; index < headerColumnCount; index++)
    //        {
    //            CRect headerItemRect;
    //            GetHeaderCtrl()->GetItemRect(index, &headerItemRect);
    //            listctrlHeaderWidth += headerItemRect.Width();
    //        }
    //    }


    //    // change the column width
    //    int tempWidth = 0;
    //    for (int index = 0; index < headerColumnCount; index++)
    //    {
    //        CRect headerItemRect;
    //        GetHeaderCtrl()->GetItemRect(index, &headerItemRect);

    //        tempWidth = listctrlWidth * ((double)headerItemRect.Width() / listctrlHeaderWidth);

    //        SetColumnWidth(index, tempWidth);
    //    }
    //    SetRedraw(TRUE);
    //}

}
