/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/CallBannerListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * List control for displaying call banner items
  * 
  */

#pragma once
#include "stdafx.h"
#include <bus\mfc_extensions\src\mfc_extensions.h>
#include "bus\mfc_extensions\src\auto_sort_list_ctrl\ISortingCallback.h"

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

		void HideScrollBars();  //TD16152

        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    private:

        TA_Base_Bus::AutoSortListCtrl::SortInfo m_calllistSortingInfo;
			
	protected:
		//{{AFX_MSG(CallBannerListCtrl)  //TD16152
		afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()   //TD16152
    };
}