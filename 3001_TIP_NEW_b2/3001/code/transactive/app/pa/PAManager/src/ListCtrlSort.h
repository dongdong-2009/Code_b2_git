/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/ListCtrlSort.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#if !defined(AFX_LISTCTRLSORT_H__444FB7AF_DF0D_47D3_B8F6_0F25DAC0A757__INCLUDED_)
#define AFX_LISTCTRLSORT_H__444FB7AF_DF0D_47D3_B8F6_0F25DAC0A757__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/pa_4669//pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"

class CListCtrlSort : public TA_Base_Bus::ListCtrlSelNoFocus, public TA_Base_Bus::ISortingCallback

{
public:
//  Note: This class always does sorting.
	CListCtrlSort();

    /**
     * DeleteAllItems
     *
     * Overriden to clear out local map on deletion
     *
     */
    BOOL DeleteAllItems();
 
	virtual ~CListCtrlSort();

private:

    TA_Base_Bus::FixedHeaderCtrl			m_fixedHeaderCtrl;
    TA_Base_Bus::AutoSortListCtrl::SortInfo m_colSortingInfo;

    std::string getItemColumnLabel( const TA_IRS_Bus::PaZoneRecord* pZoneRecord, int iColumn );
    std::string getStatusStr( const TA_IRS_Bus::PaZoneRecord* pZoneRecord );

    /**
     * sortCallback
     *
     * User defined sorting function
     *
     * @return positive value if item lParam2 before lParam1, negative if lParam1 should
     *      come first
     *
     * @param lParam1, lParam2 user defined values passed in from sort operation (app specific)
     * @param columnIndex the index of the column being sorted
     * @param currentSort defines the type of sorting specified (ie ascending or descending)
     */
    virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort);

//protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
//	virtual void PreSubclassWindow();
};

#endif // !defined(AFX_LISTCTRLSORT_H__444FB7AF_DF0D_47D3_B8F6_0F25DAC0A757__INCLUDED_)
