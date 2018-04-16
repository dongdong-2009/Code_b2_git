/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/mfc_extensions/src/focus_management/XorListSelectionController.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Accepts a set of list controls as parameters
  * but only allows a single list control to have a selection
  * at any one time
  *
  */

#pragma once

#include <set>

namespace TA_Base_Bus
{

    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) XorListSelectionController
    #else
    class AFX_EXT_CLASS XorListSelectionController
    #endif
    {
    public:
        XorListSelectionController() {}
        virtual ~XorListSelectionController() {}
    
        /**
         * registerList
         *
         * Registers a new list for us to operate on
         * 
         * @pre pList hasn't already been registered
         *
         * @param pList the list control
         *
         */
        void registerList(CListCtrl* pList);
    
        /**
         * onListItemStateChange
         *
         * Called when a list selection state is being modified, all
         *  *other* lists contained in this controller will
         *  have their selections cleared if there is a new 
         *  selection made to the input list (new item selected)
         *
         * @pre pList exists in list (from registerList)
         *
         * @param pSelType details of the state change
         *
         * @param pList the list which is having the selection modified
         *
         */
        void onListItemStateChange(NM_LISTVIEW* pSelType, CListCtrl* pList);
    
    private:
    
        std::set<CListCtrl*> m_listControls;
    };
    
}   // namespace TA_Base_Bus
