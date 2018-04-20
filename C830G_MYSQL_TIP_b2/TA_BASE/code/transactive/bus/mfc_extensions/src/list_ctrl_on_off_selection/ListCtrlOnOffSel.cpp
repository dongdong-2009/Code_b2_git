/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/list_ctrl_on_off_selection/ListCtrlOnOffSel.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ListCtrlOnOffSel inherits from ListCtrlSelNoFocus.
  * it allows selection to be click on, click off style
  * as if the user was holding ctrl constantly.
  *
  */


#include "../stdafx.h"

#include <yvals.h>              // warning numbers get enabled in yvals.h 
#pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

#include "ListCtrlOnOffSel.h"


namespace TA_Base_Bus
{

    ListCtrlOnOffSel::ListCtrlOnOffSel()
    {
    }

    ListCtrlOnOffSel::~ListCtrlOnOffSel()
    {
    }


    BEGIN_MESSAGE_MAP(ListCtrlOnOffSel, ListCtrlSelNoFocus)
	    //{{AFX_MSG_MAP(ListCtrlOnOffSel)
        ON_WM_LBUTTONDOWN()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void ListCtrlOnOffSel::OnLButtonDown(UINT nFlags, CPoint point) 
    {
        UINT flags = LVHT_ONITEM;
        int item = HitTest(point, &flags);

        // toggle the item
        if (item > -1)
        {
            UINT itemState = GetItemState(item, LVIS_SELECTED);
            SetItemState(item, ~itemState, LVIS_SELECTED);
        }

        // this causes the selection to be undone
        // a side effect prevents the list getting focus
        // but that shouldnt worry most applications.
        //CListCtrl::OnLButtonDown(nFlags, point);
    }

} // TA_Base_Bus

