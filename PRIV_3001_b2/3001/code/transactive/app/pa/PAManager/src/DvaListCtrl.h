/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DvaListCtrl.h $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Maintains a list control for DVA messages
  *
  */

#if !defined(AFX_DVALISTCTRL_H__AA87B8BD_6BE9_484A_B86B_BD8B058B88F8__INCLUDED_)
#define AFX_DVALISTCTRL_H__AA87B8BD_6BE9_484A_B86B_BD8B058B88F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DvaListCtrl.h : header file
//

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// DvaListCtrl window

namespace TA_IRS_Bus
{
    struct StationDvaMessageRecord;
}

class DvaListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
{
// Construction
public:

    enum EColumnIds
    {
        DVA_COL_LABEL,
        DVA_COL_EMERGENCY,
        DVA_COL_UDERLINE,
        DVA_COL_MAX
    };

    /**
     * DvaListCtrl
     *
     * @param sortingEnabled true if this list control is able to be sorted
     *          by clicking on the column header
     */
	DvaListCtrl(bool sortingEnabled);

    /**
     * ~DvaListCtrl
     *
     */
    virtual ~DvaListCtrl();

    /**
     * processDvaMessageUpdates
     *
     * Refreshes contents of this list *if* the input update is relevant to any items in the list 
     *  if the specified data doesn't match any existing records, the function does nothing
     *
     * @param dvaUpdate the message type update info structure (set of updates)
     *
     */
    void processDvaMessageUpdates(const DvaMessageUpdateData& dvaUpdate);

    /**
     * doesMessageKeyExist
     *
     * Checks if the specified message key is in use by any records present in this DVA list
     *
     * @param messageKey the message key that will be searched for amongst the DVA message keys
     *
     * @return true if any item in this list has a key that matches the supplied key
     *
     */
    bool doesMessageKeyExist(unsigned long messageKey);

    /**
     * doesMessageKeyExist
     *
     * Checks if any of the specified message keys are in use by any records present in this DVA list
     *
     * @param messageKeys the message keys that will be searched for amongst the DVA message keys
     *
     * @return true if any item in this list has a key that matches any of the supplied keys
     *
     */
    bool doesMessageKeyExist(const DvaMessageUpdateData& messageKeys);

    /**
     * doAllMessageKeysExist
     *
     * Checks if all of the specified message keys are in use by records present in this DVA list
     *
     * @param messageKeys the message keys that will be searched for amongst the DVA message keys
     *
     * @return true if all inputs keys matched an item in this DVA message list
     *
     */
    bool doAllMessageKeysExist(const DvaMessageUpdateData& messageKeys);

    /**
     * getSelectedMessageKey
     *
     * @return the key of the currently selected message, or TA_Base_Core::DatabaseKey::getInvalidKey()
     *      if there is no selection current.  If there happens to be
     *      multiple selections, the first is returned
     *
     */
    unsigned long getSelectedMessageKey();

    /**
     * getRecordFromIndex
     *
     * @return pointer to the record at the specified index, null if not found
     *
     * @param index the index to return
     *
     * @pre index >= 0 && index < GetItemCount()
     *
     */
    unsigned long getMessageKeyFromIndex(int index) const;

    /**
     * collectMatchingKeys
     *
     * Returns data on which of the input keys relate to this control, the
     *  union of modifiedKeys and deletedKeys will be all inputKeys that
     *  are found in the control, and these output sets contain no matching keys
     * 
     * @return true if any of the input keys belong to an item in this list
     *              (that is, one or more item(s) added to either of output sets)
     *
     * @param inputKeys the input primary keys of DVA Messages
     *
     * @param modifiedKeys [OUT] the subset of all keys from inputKeys that are
     *                          part of this DvaList (excluding now deleted keys)
     *
     * @param deletedKeys [OUT] the subset of all keys from inputKeys that are
     *                      part of this DvaList, but also have a deleted flag
     *
     * @param typeChanges [OUT] set to true if any of the detected matches
     *          have had their type changed (ie internal doesn't match cached maps)
     *
     * @param labelChanges [OUT] set to true if any of the detected matches
     *          have had their label changed (ie internal doesn't match cached maps)
     *
     */
    bool collectMatchingKeys(const DvaMessageUpdateData& inputKeys, 
        DvaMessageUpdateData& modifiedKeys, DvaMessageUpdateData& deletedKeys );

// Operations
public:
    /**
     * findItemIndices
     * 
     * @pre one or more items exist with a key matching supplied key
     *
     * @param key the search key
     *
     * @return an array of item indices where keys match supplied key
     *          the resulting array will be ordered from smallest to largest
     *          so if used for delete operations, traverse in reverse order
     *
     */
    std::vector<int> findItemIndices(const unsigned long key);


    /**
     * SetItemData
     *
     * Adds the specified item to the list (even if one with same key already exists)
     * If the parameter existingKey is valid (non null), the record is used to update
     * an existing item in the list (the old record will be destroyed)
     * Will take ownership of the supplied record and destroy it when done
     *
     * @pre if existingKey is a valid pointer, then there exists at least
     *      one record in the list with a matching key (that we can replace)
     *
     * @param record the data used to display in this control
     *
     * @param existingKey set to value of an existing key, if applicable
     *     
     * @param sortNewItemsById true if any new items added should be
     *              inserted in order, by ascending record ID
     *              @see findSortedIdInsertionPoint
     *
     */
	void updateFileds( int nItem, const TA_IRS_Bus::StationDvaMessageRecord& record );
 

    /**
     * updateCheckedState
     *
     * Updates the checked state of the specified item in the list control
     * by making the relevant call to SetCheck
     * 
     * @param nItem the index of the item to have its stage updated
     * @param record pointer to the record
     */
    void updateCheckedState(int nItem, const unsigned short usType);

	
	/** CL-15349
	 * selectItemInList
	 *
	 * This is to instruct the DVA list to select an item in the list
	 * 
	 * @param int: The index number of the item in list
	 * 
	 * @return: bool: True if successful, otherwise False.
	 *
	 */
	bool selectItemInList(int nItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DvaListCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:

    virtual void PreSubclassWindow();

    /// Override (CListCtrlSelNoFocus)
	//{{AFX_MSG(DvaListCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

    virtual DWORD getForegroundColourForItem(int nItem);

    /** 
     * Draws an underline for the item at the specified row
     *
     * @param rowIndex defines the item to draw underline for
     * @param lplvcd defines the drawing context
     */
    void drawUnderline(int rowIndex, LPNMLVCUSTOMDRAW lplvcd);

    /**
     * refreshItemsFromCachedMaps
     *
     * Refreshes item already existing in this list that can be identified with the specified key
     *
     * @param itemsForUpdate structure containing set of keys that require update
     *
     * @exception CachedMappingNotFoundException if item not found - or invalid
     *
     * 
     */
    void refreshItemsFromCachedMaps(const DvaMessageUpdateData& updateKeys);

protected:
    bool m_sortingEnabled;
    TA_Base_Bus::AutoSortListCtrl::SortInfo m_colSortingInfo;
    TA_Base_Bus::FixedHeaderCtrl  m_fixedHeaderCtrl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVALISTCTRL_H__AA87B8BD_6BE9_484A_B86B_BD8B058B88F8__INCLUDED_)
