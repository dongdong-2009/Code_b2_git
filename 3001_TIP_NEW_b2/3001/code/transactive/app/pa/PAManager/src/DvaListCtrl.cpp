/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/DvaListCtrl.cpp $
  * @author Sean Liew
  * @version $Revision: #7 $
  * Last modification: $DateTime: 2012/12/07 11:52:06 $
  * Last modified by: $Author: qi.huang $
  * 
  * Maintains a list control for DVA messages
  *
  */


#include "app/pa/PAManager/src/stdafx.h"

#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/DvaListCtrl.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/TisCachedMaps.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "app/pa/PAManager/src/LockControlRedraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long EMERGENCYFGCOLOR = RGB(255, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// DvaListCtrl

DvaListCtrl::DvaListCtrl(bool sortingEnabled) :
m_sortingEnabled(sortingEnabled),
m_colSortingInfo(DVA_COL_MAX)
{
    // We want LVN_ITEMCHANGING notifications to be passed back to parent window
    // (and give it the opportunity to handle the notifications)
    setItemChangeRequestForwarding(TRUE);

    if (m_sortingEnabled)
    {
        // Sort columns based on label
        m_colSortingInfo.setCurrentSort(DVA_COL_LABEL, TA_Base_Bus::AutoSortListCtrl::ASCENDING);
        // Make label column the active sorting column
        m_colSortingInfo.activateSort(DVA_COL_LABEL);
    }
}


DvaListCtrl::~DvaListCtrl()
{
}


void DvaListCtrl::drawUnderline(int rowIndex, LPNMLVCUSTOMDRAW lplvcd)
{
    RECT rc;
	CString itemMsg = GetItemText(rowIndex, 0);   //TD17646
	CSize size;
    if (GetItemRect(lplvcd->nmcd.dwItemSpec, &rc, LVIR_LABEL))
    {
        DWORD lineColour = lplvcd->clrText;
        
        CDC dc;
        dc.Attach(lplvcd->nmcd.hdc); 
		size = dc.GetTextExtent(itemMsg);    //TD17646
        CPen pen(PS_SOLID, 1, lineColour);
        CPen *oldPen = dc.SelectObject(&pen); 
        
        int horzClearance = 2;
        int vertClearance = 3;

        dc.MoveTo(rc.left + horzClearance,  rc.bottom - vertClearance);
		dc.LineTo(rc.left + horzClearance + size.cx,  rc.bottom - vertClearance); //TD17646
    
        dc.SelectObject(oldPen);
        dc.Detach();
    }
}


void DvaListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

    if (CDDS_ITEMPOSTPAINT == lplvcd->nmcd.dwDrawStage)
    {
        // If this is a TIS enabled message, it needs to be underlined
        // It is not sufficient to simply provide an underlined font because
        // as defined in the MSDN documentation, selected items cannot
        // have a custom font defined by NM_CUSTOMDRAW
        CString strUnderline = GetItemText( lplvcd->nmcd.dwItemSpec, DVA_COL_UDERLINE );
        if ( !strUnderline.IsEmpty() )
        {
        	// Underline the message
            drawUnderline(lplvcd->nmcd.dwItemSpec, lplvcd);
        }
    }

    // To make sure the selected item draws properly when focus is lost..
    ListCtrlSelNoFocus::OnCustomDraw(pNMHDR, pResult);

    // The ListCtrlSelNoFocus ensures each draw stage is handled
    if (CDDS_ITEMPREPAINT == lplvcd->nmcd.dwDrawStage)
    {
        // Always request post-paint, to allow opportunity to draw underline
        *pResult |= CDRF_NOTIFYPOSTPAINT;
    }  
}


DWORD DvaListCtrl::getForegroundColourForItem(int nItem)
{
    CString strEmergency = GetItemText( nItem, DVA_COL_EMERGENCY );
    if ( !strEmergency.IsEmpty() )
    {
        return EMERGENCYFGCOLOR;
    }

    return ListCtrlSelNoFocus::getForegroundColourForItem(nItem);
}


unsigned long DvaListCtrl::getMessageKeyFromIndex(int index) const
{
    return static_cast<unsigned long>(GetItemData( index ));
}


unsigned long DvaListCtrl::getSelectedMessageKey()
{
    if (GetSelectedCount() > 0)
    {
        int itemIndex = GetNextItem(-1, LVNI_SELECTED);

        // Invalid - no items selected
        if (itemIndex < 0)
        {
            return TA_Base_Core::DatabaseKey::getInvalidKey();
        }

        return static_cast<unsigned long>(GetItemData(itemIndex));
    }
    else
    {
        // No selection made       
        return TA_Base_Core::DatabaseKey::getInvalidKey();
    }
}


void DvaListCtrl::updateFileds( int nItem, const TA_IRS_Bus::StationDvaMessageRecord& record )
{
    bool bUnderline = TA_IRS_Bus::TisCachedMaps::getInstance().isPADvaMsgMapped( record.ulKey );
    bool bEmergency = (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY == record.usMsgType ) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == record.usMsgType ));

    // Begin updating the item
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == record.usMsgType )
    {
        SetItemText(nItem, DVA_COL_LABEL, "");
    }
    else
    {
        SetItemText(nItem, DVA_COL_LABEL, record.strMsgName.c_str());
    }

    static const char* const FeaturedColumn = "1";
    static const char* const EmptyColumn = "";
    SetItemText(nItem, DVA_COL_EMERGENCY, bEmergency ? FeaturedColumn : EmptyColumn );
    SetItemText(nItem, DVA_COL_UDERLINE, bUnderline ? FeaturedColumn : EmptyColumn );

    updateCheckedState( nItem, record.usMsgType );

    ListCtrlSelNoFocus::SetItemData( nItem, static_cast<unsigned long>(record.ulKey) );
}


void DvaListCtrl::updateCheckedState(int nItem, const unsigned short usType)
{
    // We must update the state right at the end - because
    // the state change handler has no means of knowing where the
    // state change request originated from (user or internally) - 
    // so the way it currently does this is checking if the internal
    // record state (from GetItemData) corresponds to the target state
    // If not - it knows it's a user state change
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == usType )
	{
		SetCheck(nItem, TRUE);
   	}
	else
	{
		SetCheck(nItem, FALSE);
	}
}


std::vector<int> DvaListCtrl::findItemIndices(const unsigned long key)
{
    std::vector<int> itemArray;

    // Addresses of all records matching this key (using this we need to them find their list indices)
    for ( int nLoop = 0; nLoop < GetItemCount(); ++nLoop )
    {
        if ( key == GetItemData( nLoop ))
        {
            itemArray.push_back(nLoop);
        }
    }

    return itemArray;
}

// CL-15349
bool DvaListCtrl::selectItemInList(int nItem)
{
	if ( SetItemState( nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ))
	{
		return true;
	}
	else
	{
		return false;
	}
}
// CL-15349


BEGIN_MESSAGE_MAP(DvaListCtrl, ListCtrlSelNoFocus)
	//{{AFX_MSG_MAP(DvaListCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DvaListCtrl message handlers
void DvaListCtrl::OnDestroy() 
{
	ListCtrlSelNoFocus::OnDestroy();
}


bool DvaListCtrl::doesMessageKeyExist(unsigned long messageKey)
{
    bool bExist = false;

    for (int nItem = 0; nItem < GetItemCount(); ++nItem )
    {
        if ( messageKey == static_cast<unsigned long>(GetItemData( nItem )))
        {
            bExist = true;
            break;
        }
    }

    return bExist;
}


bool DvaListCtrl::doAllMessageKeysExist(const DvaMessageUpdateData& messageKeys)
{
    std::set<unsigned long> setExistKeys;
    for (int nItem = 0; nItem < GetItemCount(); ++nItem )
    {
        setExistKeys.insert( static_cast<unsigned long>(GetItemData( nItem )));
    }

    bool bExist = true;

    for ( DvaMessageUpdateData::const_iterator itLoop = messageKeys.begin(); messageKeys.end() != itLoop; ++itLoop )
    {
        if ( setExistKeys.end() == setExistKeys.find( *itLoop ))
        {
            bExist = false;
            break;
        }
    }

    // ALl keys must have been present
    return bExist;
}


bool DvaListCtrl::doesMessageKeyExist(const DvaMessageUpdateData& messageKeys)
{
    std::set<unsigned long> setExistKeys;
    for (int nItem = 0; nItem < GetItemCount(); ++nItem )
    {
        setExistKeys.insert( static_cast<unsigned long>(GetItemData( nItem )));
    }

    bool bExist = false;

    for ( DvaMessageUpdateData::const_iterator itLoop = messageKeys.begin(); messageKeys.end() != itLoop; ++itLoop )
    {
        if ( setExistKeys.end() != setExistKeys.find( *itLoop ))
        {
            bExist = true;
            break;
        }
    }

    return bExist;
}


bool DvaListCtrl::collectMatchingKeys( const DvaMessageUpdateData& inputKeys, 
    DvaMessageUpdateData& modifiedKeys, DvaMessageUpdateData& deletedKeys )
{
    // Clear the output variables straight off
    modifiedKeys.clear();
    deletedKeys.clear();

    bool matchFound = false;

    for (int i = 0; i < GetItemCount(); i ++)
    {
        unsigned long key = getMessageKeyFromIndex(i);

        if (inputKeys.find(key) == inputKeys.end())
        {
            // This particular record doesn't correspond to input key, may ignore
            continue;
        }

        // One of input keys matched a local key
        matchFound = true;

        // Obtain the latest record from cached maps
        TA_IRS_Bus::StationDvaMessageRecord record;
        try
        {
            record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(key);
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {   
			deletedKeys.insert(key);
            continue;
        }

        // Determine which group to place message in (based on deleted state)
        if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED == record.usMsgType ||
             TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == record.usMsgType )
        {
            deletedKeys.insert(key);            
        }                              
        else
        {
            // Otherwise this message exists, but not deleted
            modifiedKeys.insert(key);
        }
    }

    return matchFound;
}


void DvaListCtrl::processDvaMessageUpdates(const DvaMessageUpdateData& updates)
{
    if (!doesMessageKeyExist(updates))
    {
        // If false returned, none of the input set of items exist locally
        // So we just ignore it (normal control flow)
        return;
    }
    
    // We can simply refresh the data for this item, using item straight from cachedMaps
    // because we know cachedMaps always to be up to date
    try
    {
        refreshItemsFromCachedMaps(updates);
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
}


void DvaListCtrl::refreshItemsFromCachedMaps(const DvaMessageUpdateData& updateKeys)
{
    for ( DvaMessageUpdateData::const_iterator itKey = updateKeys.begin(); itKey != updateKeys.end(); ++itKey )
    {
        if (!doesMessageKeyExist(*itKey))
        {
            // This particular key not within this control
            continue;
        }

        std::vector<int> vecItems = findItemIndices( *itKey );

		try
		{
		    TA_IRS_Bus::StationDvaMessageRecord clonedRecord = 
                TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(*itKey);

            for ( std::vector<int>::iterator itItem = vecItems.begin(); itItem != vecItems.end(); ++itItem )
            {
			    updateFileds(*itItem, clonedRecord);
            }
		}
		catch (const TA_Base_Core::CachedMappingNotFoundException&)
		{
			// Item no longer exists in cached maps
            for ( std::vector<int>::iterator itItem = vecItems.begin(); itItem != vecItems.end(); ++itItem )
            {
                // Clean up the old record pointer memory, and set text to invalid
                if (0 != getMessageKeyFromIndex( *itItem ))
                {
                    SetItemText( *itItem, 0, "<Deleted Dva Message>" );
                }
            }
		}
    }
}


BOOL DvaListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// Do not erase background
    //return TRUE;
	return ListCtrlSelNoFocus::OnEraseBkgnd(pDC);
}


LRESULT DvaListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_sortingEnabled)
    {
		TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_colSortingInfo);
    }

    return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
}


void DvaListCtrl::PreSubclassWindow()
{
    ListCtrlSelNoFocus::PreSubclassWindow();

    SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT );

    m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());
}

