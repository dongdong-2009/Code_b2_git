/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/call_page/src/CallBannerListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * List control for displaying call banner items
  * 
  */

#pragma once

#include <map>
#include "bus\banner_pages\call_page_library\src\CallEntryManager.h"
#include <bus\mfc_extensions\src\mfc_extensions.h>

#define ID_REMOVE_CALL                 40001
#define ID_MAKE_CALL                   40002 
#define ID_RESET_ALL_PEC			   40003
#define ID_CLEAR_INVALID			   40004

namespace TA_IRS_App
{

    class CallBannerListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:
        
        /**
         * CallBannerListCtrl
         *
         * @param maxColumnCount defines max# columns that will be inserted
         */
        CallBannerListCtrl(int maxColumnCount);

		DECLARE_MESSAGE_MAP()
	public:
        /**
         * initialise
         *
         * Initialises the list control, providing the user defined callback
         *  required for sorting the elements
         *
         * @param columnIndex defines the time column
         * @param callback user defined sorting callback function
         */
        void initialise(int columnIndex, TA_Base_Bus::ISortingCallback* callback);

        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		afx_msg void OnRButtonUp(NMHDR* pNMHDR, LRESULT* pResult);

		int getLastRightClickItem();

		/**
		  * Remove call entry with content of Radio Call
		  * @param item index of the item
		  */
		void onRemoveRadioCall();

		/**
		  * Make a call to the radio call and remove the call
		  * @param item index of the item
		  */
		void onMakeRadioCall();

		void setCallEntryManager(TA_IRS_Bus::CallEntryManager* manager);

		// handles the reseting of all PEC call with the same train id
		afx_msg void OnResetAll();
		
		// handles the reseting of invalid train, this call is one by one
		afx_msg void OnClearInvalid();
		afx_msg void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
    private:

        TA_Base_Bus::AutoSortListCtrl::SortInfo m_calllistSortingInfo;

		TA_IRS_Bus::CallEntryManager* m_callEntryManager;

		int m_lastRightClickItem;

		static const int COL_STATUS;

		// function helper to call context menu
		void showContextMenu ();

		// function helper to get the call id base on the list selection
		int getIdFromSelectionList ();
		// function helper to validate the right to show the context menu
		bool isValidToShowContextMenu ();
    };
}