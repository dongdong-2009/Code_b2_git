/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/DisplayLogListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  *
  * This is used to display Tis display request logs.
  * The request data is stored as part of this list.
  */

#if !defined(AFX_DisplayLogListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
#define AFX_DisplayLogListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_

#include "stdafx.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"

#include "IRequestSelectionListener.h"
#include "ILoadProgressDisplay.h"
#include "DisplayRequestLoadingThread.h"

using TA_IRS_Core::IDisplayRequest;
using TA_Base_Bus::ListCtrlSelNoFocus;
using TA_Base_Bus::AutoSortListCtrl;
typedef /*using*/ TA_Base_Bus::AutoSortListCtrl::SortInfo SortInfo;


namespace TA_IRS_App
{
    class DisplayLogListCtrl : public ListCtrlSelNoFocus,
                               public TA_Base_Bus::ISortingCallback
    {
    public:


        DisplayLogListCtrl();
        virtual ~DisplayLogListCtrl();


        /**
          * setSelectionListener
          *
          * Sets the object to be notified of selection changes
          */
        void setSelectionListener(IRequestSelectionListener* listener);


        /**
          * setProgressDisplay
          *
          * Sets the object to be notified of progress updates
          */
        void setProgressDisplay(ILoadProgressDisplay* progressDisplay);

        /**
          * reload
          *
          * Clears the current list and reload the items from database.
          *
          * @return void
          * @param : const bool displayTTISFreeText
          * @param : const bool displayTTISPredefined
          * @param : const bool displaySTISFreeText
          * @param : const bool displaySTISPredefined
          */
        void reload(const bool displayTTISFreeText,
                    const bool displayTTISPredefined,
                    const bool displaySTISFreeText,
                    const bool displaySTISPredefined);

        /**
          * getSelectedRequest
          *
          * Return a pointer to the database object associated with the
          * selected request.
          *
          * @return a pointer to the selected request object.
          *         NULL if nothing is selected.
          *         do not store or delete the pointer, it belongs to the list.
          */
        IDisplayRequest* getSelectedRequest();


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
        virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort);

        IDisplayRequest* getLastSelectItem();

        std::vector<IDisplayRequest*> getAllSelectedItems();


    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(DisplayLogListCtrl)
        protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:

        // {{AFX_MSG(DisplayLogListCtrl)
        afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg BOOL onClick(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT onDisplayRequestLoadStart(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onAddDisplayRequest(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onDisplayRequestLoadEnd(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()


        enum LogColumns
        {
            TIMESTAMP = 0,
            SYSTEM = 1,
            SUBMITTED_BY = 2,
            TYPE = 3,
            MESSAGE_CONTENT = 4,
            STATE = 5,
            START_TIME = 6,
            END_TIME = 7,
            PRIORITY = 8,
            RATIS_TAG = 9
        };


        /**
          * isSelectable
          *
          * Given the row number, return true if this item
          * should be selectable.
          *
          * @param rowNumber the row number.
          *
          * @return true if the item can be selected
          */
        virtual bool isSelectable(int rowNumber);


    private:

        /**
          * loadAllDisplayRequests
          *
          * loads all display requests from the database and populates the list
          *
          */
        void loadAllDisplayRequests();


        /**
          * cleanupDisplayRequests
          *
          * clears the display request list and deletes database objects
          *
          */
        void cleanupDisplayRequests();

        /**
          * addDisplayRequest
          *
          * Add a display request to the list. The request object should not be deleted
          * because the pointer is added as the item data.
          *
          * @param request
          *
          */
        void addDisplayRequest(IDisplayRequest* request);


        /**
          * isTtisMessageReinstatable
          *
          * Returns true if a TTIS message with the given end time is reinstatable. This
          * is the case if the current system time is before the end time.
          *
          * @param endTime  The end time of the message.
          *
          * @return  True if the message is reinstatable, and false otherwise.
          */
        bool isTtisMessageReinstatable(const std::string& endTime);


        /**
          * isStisMessageReinstatable
          *
          * Returns true if an STIS message with the given end date/time is reinstatable. This
          * is the case if the current system date/time is before the end date/time.
          *
          * @param endTime  The end date/time of the message.
          *
          * @return  True if the message is reinstatable, and false otherwise.
          */
        bool isStisMessageReinstatable(const std::string& endDateTime);

        // custom header control
        TA_Base_Bus::FixedHeaderCtrl    m_fixedHeaderCtrl;

        // selection listener
        IRequestSelectionListener* m_selectionListener;

        // background loading of the display logs
        DisplayRequestLoadingThread* m_loadingThread;

        // used to display load progress
        ILoadProgressDisplay* m_loadProgressDisplay;

        // true if the list is currently loading
        bool m_loading;

        // current sort information
        SortInfo m_sortInfo;

        int m_latestIndex;

        // TES891 ~ added - used for filtering messages to be displayed
        bool m_canDisplayTTISFreeText,
             m_canDisplayTTISPredefined,
             m_canDisplaySTISFreeText,
             m_canDisplaySTISPredefined;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_App

#endif // !defined(AFX_DisplayLogListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
