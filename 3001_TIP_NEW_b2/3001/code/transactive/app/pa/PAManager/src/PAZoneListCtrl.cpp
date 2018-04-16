/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/PAZoneListCtrl.cpp $
  * @author Sean Liew
  * @version $Revision: #7 $
  * Last modification: $DateTime: 2012/12/21 17:51:35 $
  * Last modified by: $Author: huangjian $
  * 
  * Maintains a list control for PA Zones
  *  Because there is only one PA Zone list in the PA Manager application
  *  this control is mostly self contained (listens for update data updates
  *  independently, updating view as required)
  *
  */

#include "app/pa/PAManager/src/stdafx.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/PAManager.h"
#include "app/pa/PAManager/src/PaZoneListCtrl.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaManagerModel.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "app/pa/PAManager/src/LockControlRedraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const char* const ZONE_NO_LONGER_EXISTS = "*Zone no longer exists*";

PaZoneListCtrl::PaZoneListCtrl() : ListCtrlSelNoFocus(),
m_bIsAtOCC( CachedConfig::getInstance()->isAtOcc() ), 
m_nMaxCol( m_bIsAtOCC ? 2 : 1 ),
m_nColLabelIndex( m_bIsAtOCC ? 1 : 0 ),
m_nColLocationIndex( m_bIsAtOCC ? 0 : -1 ),
m_nColOrderIndex( m_bIsAtOCC ? 2 : 1 ),
m_colSortingInfo( m_nMaxCol ),
m_fixedHeaderCtrl()
{
    // We want LVN_ITEMCHANGING notifications to be passed back to parent window
    // (and give it the opportunity to handle the notifications)
    setItemChangeRequestForwarding(TRUE);

    // Sort columns based on label
    m_colSortingInfo.setCallback(this);
    m_colSortingInfo.defineSortingSemantic(m_nColLabelIndex, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);
    if ( m_bIsAtOCC )
    {
        m_colSortingInfo.defineSortingSemantic(m_nColLocationIndex, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);
    }

    // Make label column the active sorting column
    m_colSortingInfo.setCurrentSort( m_nColLabelIndex, TA_Base_Bus::AutoSortListCtrl::ASCENDING );
    m_colSortingInfo.activateSort( m_nColLabelIndex );
}


PaZoneListCtrl::~PaZoneListCtrl()
{
}


BEGIN_MESSAGE_MAP(PaZoneListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(PaZoneListCtrl)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL PaZoneListCtrl::SetItemData( const TA_IRS_Bus::PaZoneRecord& record )
{
	int nextIndex = InsertItem(0, ""); 
    int colIndex = 0;

    try
    {
        // Previous codes are replaced by codes below which work for all stations
        // without changing meaning of columns's name
        SetItemText( nextIndex, m_nColLabelIndex, record.m_label.c_str() );
        CString strOrder = "";
        strOrder.Format( "%d", record.m_orderId );
        SetItemText( nextIndex, m_nColOrderIndex, strOrder.GetBuffer() );
        strOrder.ReleaseBuffer();

        if ( m_bIsAtOCC )
        {
            SetItemText( nextIndex, m_nColLocationIndex, record.m_strDispLocName.c_str() );
        }

        return CListCtrl::SetItemData(nextIndex, record.m_pKey);
    }
    catch (...)
    {
        // Don't allow SetItemData to fail (leaving undefined columns)
        return CListCtrl::SetItemData(nextIndex, TA_Base_Core::DatabaseKey::getInvalidKey());
    }
}


void PaZoneListCtrl::addNullEntry(unsigned long key)
{
    int nItem = InsertItem(0, "");

    SetItemText(nItem, m_nColLabelIndex, ZONE_NO_LONGER_EXISTS);
    SetItemText(nItem, m_nColOrderIndex, "");

    if ( m_bIsAtOCC )
    {
        SetItemText(nItem, m_nColLocationIndex, "");
    }

    // Ensure invalid key set to here
    CListCtrl::SetItemData(nItem, TA_Base_Core::DatabaseKey::getInvalidKey());
}


void PaZoneListCtrl::initialise()
{
    SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );

    int colIndex = 0;

    int headerTotalWidth = 335;		// extend to take up reminder space

    int STN_WIDTH = 0;				// change from "const int" to "int" for calculating correct 

    if (m_bIsAtOCC)
    {
		STN_WIDTH = 100;

        // Station column always present (wasn't in earlier versions)
        InsertColumn(colIndex++, "LOC", LVCFMT_LEFT, STN_WIDTH);    
    }

    const int ZONE_WIDTH = headerTotalWidth - STN_WIDTH;
 
    InsertColumn(colIndex++, "Zone", LVCFMT_LEFT, ZONE_WIDTH);
    InsertColumn(colIndex++, "Order", LVCFMT_LEFT, 0);

    m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());
}


void PaZoneListCtrl::updateZoneList(const Keys& keys)
{
    LockControlRedraw redrawLock(*this);        // Reduce flicker

    DeleteAllItems();

    std::set<std::string> setPaZoneRecordIds;
    std::string paZoneRecordId;

    for (Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
    {
        try
        {
            TA_IRS_Bus::PaZoneRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey(*it);
            paZoneRecordId = record.m_strZoneStnLocName + record.m_area + record.m_stnZoneShortName;
            if ( setPaZoneRecordIds.find(paZoneRecordId) == setPaZoneRecordIds.end() )
            {
                setPaZoneRecordIds.insert(paZoneRecordId);
                SetItemData(record);
            }
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            // This zone no longer exists in database
            addNullEntry(*it);
        }
    }
}


void PaZoneListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
}


LRESULT PaZoneListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_colSortingInfo);

    return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
}


int PaZoneListCtrl::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort)
{
    unsigned long ulFirstKey = 0u;
    unsigned long ulSecondKey = 0u;

    if (currentSort == TA_Base_Bus::AutoSortListCtrl::DESCENDING)
    {
        ulFirstKey = static_cast<unsigned long>(lParam2);
        ulSecondKey = static_cast<unsigned long>(lParam1);
    }
    else
    {
        // ascending or unknown - which will default to ascending
        ulFirstKey = static_cast<unsigned long>(lParam1);
        ulSecondKey = static_cast<unsigned long>(lParam2);
    }

    int nRet = 0;

    try
    {
        CString strFirstOrder = "";
        CString strSecondOrder = "";
        TA_IRS_Bus::PaZoneRecord oFirst = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey( ulFirstKey );
        TA_IRS_Bus::PaZoneRecord oSecond = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey( ulSecondKey );

        // Previous codes are replaced by codes below which work for all stations
        // without changing meaning of columns being used
        if ( m_bIsAtOCC && ( columnIndex == m_nColLocationIndex ))
        {
            // geographical order
            nRet = oFirst.m_orderId - oSecond.m_orderId;
            if ( 0 == nRet )
            {
                nRet = oFirst.m_label.compare(oSecond.m_label);
            }
        }
        else if ( columnIndex == m_nColLabelIndex )
        {
            nRet = oFirst.m_label.compare(oSecond.m_label);
            if ( 0 == nRet )
            {
                nRet = oFirst.m_orderId - oSecond.m_orderId;
            }
        }
    }
    catch (...)
    {
        // LOG
    }

    return nRet;
}
