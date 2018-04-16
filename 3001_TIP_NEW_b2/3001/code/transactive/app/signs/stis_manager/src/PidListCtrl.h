/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/PidListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/03/13 10:29:41 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * PidListCtrl defines a list control that displays PIDs.
  */

#if !defined(PidListCtrl_H)
#define PidListCtrl_H

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

using TA_Base_Bus::AutoSortListCtrl;
typedef TA_Base_Bus::AutoSortListCtrl::SortInfo SortInfo;

namespace TA_IRS_App
{
    class PidSelectionManager;
}

namespace TA_IRS_App
{
    class PidListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:

        // the columns for the list control
        enum PidColumns
        {
			LOCATION = 0,
            PID_NAME = 1
        };

	    PidListCtrl();
	    virtual ~PidListCtrl();

        /** 
          * setPidSelectionManager
          *
          * Sets the object to tell when selection changes
          *
          * @param pidSelectionManager
          */
        void setPidSelectionManager(PidSelectionManager* pidSelectionManager);

    public:

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(PidListCtrl)
	    protected:
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL


	    // Generated message map functions
    protected:

	    //{{AFX_MSG(PidListCtrl)
        afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        afx_msg BOOL onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    
	    DECLARE_MESSAGE_MAP()

         /** 
          * isSelectable
          *
          * Given the row number, return true if this item
          * should be selectable.
          *
          * TES 880 - individual PIDS are no longer selected for clear requests
          *
          * @param rowNumber the row number.
          *
          * @return true if the item can be selected
          */
        virtual bool isSelectable(int rowNumber);

    private:

        // custom header control
        TA_Base_Bus::FixedHeaderCtrl	m_fixedHeaderCtrl;

        // current sort information
        SortInfo m_sortInfo;

        PidSelectionManager* m_pidSelectionManager;

        bool m_pidSelected;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(PidListCtrl_H)
