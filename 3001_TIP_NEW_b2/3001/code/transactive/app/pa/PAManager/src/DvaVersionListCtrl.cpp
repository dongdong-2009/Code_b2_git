/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/DvaVersionListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #5 $
  * Last modification: $DateTime: 2015/12/23 16:11:53 $
  * Last modified by: $Author: qi.huang $
  * 
  * Helper class for displaying DVA Message data
  *
  */

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/DvaVersionListCtrl.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/LockControlRedraw.h"
#include "app/pa/PAManager/src/CachedConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;

DvaVersionListCtrl::DvaVersionListCtrl() : 
m_excessWidth(0)
{
}

DvaVersionListCtrl::~DvaVersionListCtrl()
{
}


BEGIN_MESSAGE_MAP(DvaVersionListCtrl, TA_Base_Bus::HighlightItemListCtrl)
	//{{AFX_MSG_MAP(DvaVersionListCtrl)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void DvaVersionListCtrl::setRowText(int nItem, const DvaVersionRecord& rowData)
{
    // Station name in first column
    SetItemText(nItem, 0, getStationName(rowData.ulLocationKey).c_str());

    SetItemText(nItem, 1, (DecodeVersionFromHexString(rowData.strVersionNumber)).c_str());
}


void DvaVersionListCtrl::addRowData(const DvaVersionRecord& rowData)
{
    // Item shouldn't already exist in list if adding
    int existingIndex = findVersionIndex(rowData.ulLocationKey);

    if (existingIndex >= 0)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Row with same locationkey [%d] already exists", rowData.ulLocationKey );
        return;
    }

    // Insert item at front (so last item added corresponds to top item)
    int nItem = InsertItem(0, getStationName(rowData.ulLocationKey).c_str());

    // Update entire row of data
    setRowText(nItem, rowData);

    SetItemData(nItem, rowData.ulLocationKey);
}


void DvaVersionListCtrl::updateRowData(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecord& rowData)
{
    DvaVersionRecord convertedRowData;
 
    convertedRowData.ulLocationKey = rowData.ulLocationKey;
    convertedRowData.strVersionNumber = convertVersionToString(rowData.strVersionNumber);

    updateRowData(convertedRowData);
}

void DvaVersionListCtrl::updateRowData(const DvaVersionRecord& rowData)
{
    // To update the item, it must exist in list already
    int existingIndex = findVersionIndex(rowData.ulLocationKey);

    if (existingIndex >= 0)
    {
        // Refresh the data in this row
        setRowText(existingIndex, rowData);
    }
}

int DvaVersionListCtrl::findVersionIndex(unsigned long locationKey) const
{
    LVFINDINFO searchParams;
    searchParams.flags = LVFI_PARAM;
    searchParams.lParam = locationKey;

    return FindItem(&searchParams);
}


std::string DvaVersionListCtrl::getStationName(unsigned long locationKey) const
{
    try
    {
        std::string name = TA_IRS_Bus::CachedMaps::getInstance()->getLocationNameFromKey(locationKey);

        return name;
    }
    catch (const TA_Base_Core::CachedMappingNotFoundException&)
    {
        return std::string("UNKNOWN");
    }
}


std::string DvaVersionListCtrl::convertVersionToString(const char* version)
{
    return std::string(version);
}


void DvaVersionListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

    int scrollPos = GetScrollPos(SB_HORZ);

    int max = GetScrollLimit(SB_HORZ);

    float proportion = float(scrollPos) / float(max);    
    CRect rc;
    GetClientRect(rc);

    // Determine point we should scroll up to
    // It will be capped at 'max' - when we scroll to 'max' full right side in view
    int absWidthDistance = (int) (proportion * max);

    for (unsigned int i = 0; i < m_hScrollChildren.size(); i ++)
    {
        CListCtrl& child = *(m_hScrollChildren[i].listControl);
        
        // absWidthDistance is our target
        int relativeOffset = absWidthDistance - m_hScrollChildren[i].wOffset;
        m_hScrollChildren[i].wOffset += relativeOffset;
        
        CSize scrollAmount(relativeOffset, 0);
        
        child.Scroll(scrollAmount);
    }
}


void DvaVersionListCtrl::addHorizontalScrollingChild(CListCtrl& child)
{
    ListContainer lc;
    lc.listControl = &child;
    lc.wOffset = 0;
    m_hScrollChildren.push_back(lc); 
}

std::string DvaVersionListCtrl::DecodeVersionFromHexString( const std::string& strSrc )
{
    std::string strDes = strSrc.c_str();
    const unsigned int unDelemiterPos = 8u;

    if ( unDelemiterPos < strDes.length() )
    {
        strDes.insert( unDelemiterPos, "." );
    }
    else
    {
        strDes = "<Unkonwn>";
    }

    return strDes.c_str();
}
