/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TimeScheduleEntryListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  *
  * TimeScheduleEntryListCtrl defines a list control that displays
  * train time schedule entries.
  */

#if !defined(TimeScheduleEntryListCtrl_H)
#define TimeScheduleEntryListCtrl_H

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"

using TA_IRS_Core::ITrainTimeSchedule;
using TA_IRS_Core::ITrainTimeScheduleEntry;

using TA_Base_Bus::AutoSortListCtrl;
typedef TA_Base_Bus::AutoSortListCtrl::SortInfo SortInfo;

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
}

namespace TA_IRS_App
{
    class TimeScheduleEntryListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:

        // the columns for the list control
        enum TimeScheduleEntryColumns
        {
            START_TIME = 0,
            END_TIME = 1,
            MESSAGE = 2,
            REPEAT_INTERVAL = 3,
            PRIORITY = 4
        };

        TimeScheduleEntryListCtrl();
        virtual ~TimeScheduleEntryListCtrl();


        /**
          * addTimeScheduleEntry
          *
          * Given a time schedule entry, display it.
          *
          * @param scheduleEntry    The schedule entry to display
          *                         set as item data, but is not owned by this object
          *                         It is just used to return in removeSelectedTimeScheduleEntry()
          *                         so the caller knows what entry to delete
          * @param owningSchedule   The time schedule that owns the entry being added.
          *                         not stored, just used to get the free text title
          *
          * @exception DataException        If the entry is broken
          * @exception DatabaseException    If there is something wrong with the db connection
          */
        void addTimeScheduleEntry(ITrainTimeScheduleEntry* scheduleEntry,
                                  ITrainTimeSchedule* owningSchedule);


        /**
          * removeSelectedTimeScheduleEntry
          *
          * Simply removes the selected time schedule entry.
          *
          * @return a pointer to the selected entry, so the caller knows what to actually delete.
          */
        ITrainTimeScheduleEntry* removeSelectedTimeScheduleEntry();


        /**
          * removeAllEntries
          *
          * clears the list. Pretty much the same as CListCtrl::DeleteAllItems
          */
        void removeAllEntries();


        /**
          * setEditMode
          *
          * sets whether to allow editing or not.
          *
          * @param editMode true if the schedule entries should be selectable
          */
        void setEditMode(bool editMode);


        /**
          * freeTextDescriptionUpdated
          *
          * When the operator updates a free text title, it needs to be refreshed in the list.
          *
          * @param message  The message number
          * @param newTitle The new display text
          *
          * @exception DataException        If the entry is broken
          * @exception DatabaseException    If there is something wrong with the db connection
          */
        void freeTextDescriptionUpdated(ITrainTimeSchedule::AdhocMessageNumber message,
                                        std::string newTitle);


        /**
          * predefinedMessagesChanged
          *
          * Called when the pre-defined message library has changed.
          * Causes the list to update its message descriptions.
          *
          * @exception DataException        If the entry is broken
          * @exception DatabaseException    If there is something wrong with the db connection
          */
        void predefinedMessagesChanged();

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(TimeScheduleEntryListCtrl)
        protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:

        // {{AFX_MSG(TimeScheduleEntryListCtrl)
        afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

        virtual bool isSelectable(int rowNumber);

    private:


        // These convert the data into displayable strings
        CString getTimeString(const time_t& time);
        CString getMessageName(ITrainTimeScheduleEntry* scheduleEntry,
                               ITrainTimeSchedule* owningSchedule);
        CString getRepeatIntervalString(int repeatInterval);
        CString getPriorityString(unsigned short priority);

        // custom header control
        TA_Base_Bus::FixedHeaderCtrl    m_fixedHeaderCtrl;

        // whether edit mode is active or not
        bool m_editEnabled;

        // current sort information
        SortInfo m_sortInfo;

        TTISPredefinedMessages* m_ttisPredefinedMessages;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(TimeScheduleEntryListCtrl_H)
