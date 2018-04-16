/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/PidListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/12/18 14:24:15 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * PidListCtrl defines a list control that displays PIDs.
  */

#include "stdafx.h"
#include "resource.h"
#include "PidListCtrl.h"
#include "PidSelectionManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    PidListCtrl::PidListCtrl()
     : m_sortInfo(2),
       m_pidSelectionManager(NULL)
    {
    }


    PidListCtrl::~PidListCtrl()
    {
    }


    BEGIN_MESSAGE_MAP(PidListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
	    //{{AFX_MSG_MAP(PidListCtrl)
	    ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, onItemchanged)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void PidListCtrl::PreSubclassWindow() 
    {
	    CListCtrl::PreSubclassWindow();

        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

        RECT rect;
        GetClientRect(&rect);
	    InsertColumn(LOCATION, "LOC"); //TD17909 AVteam
	    SetColumnWidth(LOCATION, 67);
        m_sortInfo.defineSortingSemantic(LOCATION, AutoSortListCtrl::BY_STRING);

        InsertColumn(PID_NAME, "PID");
        SetColumnWidth(PID_NAME, rect.right - 67);
        m_sortInfo.defineSortingSemantic(PID_NAME, AutoSortListCtrl::BY_STRING);

        // Sort columns based on label
        m_sortInfo.setCurrentSort(LOCATION, AutoSortListCtrl::ASCENDING);
        // Make label column the active sorting column
        m_sortInfo.activateSort(LOCATION);

        setScrollBarVisibility(true, false);

        m_pidSelected = false;

        setNonSelectableGray(false);
    }


    void PidListCtrl::setPidSelectionManager(PidSelectionManager* pidSelectionManager)
    {
        m_pidSelectionManager = pidSelectionManager;
    }


    bool PidListCtrl::isSelectable(int rowNumber)
    {
        return false;
    }


    BOOL PidListCtrl::onItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

        // call the parent class handler first
        // if it returns true - this has been blocked.
        if (ListCtrlSelNoFocus::OnItemchanged(pNMHDR, pResult) == TRUE)
        {
            return TRUE;
        }

        *pResult = 0;

        // Only interested in state changes (ie selection)
	    if (pNMListView->uChanged != LVIF_STATE) 
	    {        
		    return FALSE;
	    }


        // if there is something managing events
	    if (m_pidSelectionManager != NULL)
	    {
            // if the status has changed
            if ( m_pidSelected != (GetFirstSelectedItemPosition() != NULL) )
            {
                m_pidSelected = (GetFirstSelectedItemPosition() != NULL);

                // update the selection manager
                m_pidSelectionManager->pidSelectionChanged();
            }
	    }

        // this allows the owning window to handle this as well
        return FALSE;
    }


    // for sorting
    LRESULT PidListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {     
        AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);

        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }

} // TA_IRS_App

