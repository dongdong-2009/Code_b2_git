/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/ListCtrlSort.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/09/11 15:41:59 $
  * Last modified by:  $Author: huangjian $
  *
  */

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "app/pa/PAManager/src/ListCtrlSort.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const int COL_ZONE = 0;
const int COL_STATUS = 1;

CListCtrlSort::CListCtrlSort() :
m_colSortingInfo(2)
{
    // We want LVN_ITEMCHANGING notifications to be passed back to parent window
    // (and give it the opportunity to handle the notifications)
    setItemChangeRequestForwarding(TRUE);

    m_colSortingInfo.setCallback(this);

    // define sorting semantic for all columns

    m_colSortingInfo.defineSortingSemantic( COL_ZONE, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);
    m_colSortingInfo.defineSortingSemantic( COL_STATUS, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);

    // Make 1st column the active sorting one
    m_colSortingInfo.setCurrentSort( COL_ZONE, TA_Base_Bus::AutoSortListCtrl::ASCENDING );
    m_colSortingInfo.activateSort( COL_ZONE );
}

CListCtrlSort::~CListCtrlSort()
{
}

LRESULT CListCtrlSort::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_colSortingInfo);
    return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
}

//void CListCtrlSort::PreSubclassWindow()
//{
//    ListCtrlSelNoFocus::PreSubclassWindow();
//
//    m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());
//}

BOOL CListCtrlSort::DeleteAllItems()
{
    int i = 0;

    while ( GetItemCount() > 0)
    {
        // get the item data
        TA_IRS_Bus::PaZoneRecord* itemData = reinterpret_cast<TA_IRS_Bus::PaZoneRecord*>(GetItemData(0));
        delete itemData;
        // delete the item
        DeleteItem(0);
    }

    return CListCtrl::DeleteAllItems();
}

std::string CListCtrlSort::getItemColumnLabel( const TA_IRS_Bus::PaZoneRecord* pZoneRecord, int iColumn )
{
    LVFINDINFO lvFind;
    lvFind.flags = LVFI_PARAM;
    lvFind.lParam = (LPARAM) pZoneRecord;
    int iItem = FindItem( &lvFind );

    const int MAX_LENGTH = 128;
    char szStatus[MAX_LENGTH];
    
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.iSubItem = iColumn;
    item.pszText = szStatus;
    item.cchTextMax = MAX_LENGTH;
    GetItem(&item);

    return std::string(szStatus);
}

std::string CListCtrlSort::getStatusStr( const TA_IRS_Bus::PaZoneRecord* pZoneRecord )
{
    return getItemColumnLabel( pZoneRecord, COL_STATUS );
}

int CListCtrlSort::sortCallback(LPARAM lParam1, 
                                LPARAM lParam2, 
                                int columnIndex, 
                                TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort)
{
    TA_IRS_Bus::PaZoneRecord* first = reinterpret_cast<TA_IRS_Bus::PaZoneRecord*>(lParam1);
    TA_IRS_Bus::PaZoneRecord* second = reinterpret_cast<TA_IRS_Bus::PaZoneRecord*>(lParam2);

    if (currentSort != TA_Base_Bus::AutoSortListCtrl::ASCENDING)
    {
        // descending
        std::swap( first, second );
    }

    if ( columnIndex == COL_ZONE )
    {
        // geographical order
        int ret = first->m_orderId - second->m_orderId;
        if (ret == 0)
        {
            // same location -> order by zone name
            ret = first->m_label.compare(second->m_label);
        }
        return ret;
    }
    else if ( columnIndex == COL_STATUS )
    {
        return ( getStatusStr(first).compare(getStatusStr(second)) );
    }
    else
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsupported column given to sort by callback" );
    }
    
    return 0;
}

