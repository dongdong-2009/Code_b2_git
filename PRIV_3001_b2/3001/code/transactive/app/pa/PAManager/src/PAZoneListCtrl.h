/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAZoneListCtrl.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Maintains a list control for PA Zones
  *  Because there is only one PA Zone list in the PA Manager application
  *  this control is mostly self contained (listens for update data updates
  *  independently, updating view as required)
  *
  */

#if !defined(AFX_PAZONELISTCTRL_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
#define AFX_PAZONELISTCTRL_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <map>

#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"

class PaZoneListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus, public TA_Base_Bus::ISortingCallback
{
// Construction
public:
	PaZoneListCtrl();

// Attributes
public:

// Operations
public:
    /**
     * initialise
     *
     * Calls once off on startup to set up the control
     * Also initialises Priority Scheme udpate listener, performs
     * initial collection of priority scheme data
     *
     * @param agent the agent to use to collect initial priority scheme data
     *          
     * @pre the m_hWnd member must be valid (ie control window already constructed)
     *
     */
    void initialise();

    /**
     * SetItemData
     *
     * Interprets the input record and adds new row
     * to this control
     *
     * @pre a record with a matching pKey has not been added prior
     *      (unless DeleteAllItems was called after previous addition)
     *
     * @param record the data used to fill out text fields in control
     *
     */
    BOOL SetItemData( const TA_IRS_Bus::PaZoneRecord& record );

    /**
     * updateZoneList
     *
     * Performs a complete refresh of this list control using the input data
     * if the input parameter keys matches the data (exactly) that was last passed
     *  to this function, there is no change (database not queried) - should
     *  call invalidateZoneData to force a refresh
     *
     * @param keys the new set of keys used to define what zones should be displayed
     *
     */
    void updateZoneList(const std::vector<unsigned long>& keys);


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

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PaZoneListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PaZoneListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(PaZoneListCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    /**
     * Intercept all messages to allow sorting
     *
     */
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:

    /**
     * addNullEntry
     *
     * Called to add in an entry showing zone not found
     * (when a PA Zone no longer exists)
     *
     * @param the key of the zone that no longer exists
     *
     */
    void addNullEntry(unsigned long key);

    /// The currently displayed zone data in this list
    bool m_bIsAtOCC;        // true if runs at OCC
    int m_nMaxCol;
    int m_nColLocationIndex;
    int m_nColLabelIndex;
    int m_nColOrderIndex;

    TA_Base_Bus::AutoSortListCtrl::SortInfo m_colSortingInfo;
    TA_Base_Bus::FixedHeaderCtrl     m_fixedHeaderCtrl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAZONELISTCTRL_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
