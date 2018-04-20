/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/MonitoredCallStackListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides list control for displaying status of radio calls that
  * are currently being monitored
  *
  */

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorUtils.h"
#include "app/radio/radio_monitor_gui/src/ManagedListCtrl.h"


namespace TA_IRS_App
{
    class MonitoredCallStackListCtrl 
    : 
    public ManagedListCtrl<RadioMonitor::RmMonitorUpdate, TA_Base_Bus::ListCtrlSelNoFocus>
    {
    public:
        typedef RadioMonitor::RmMonitorUpdate MonitorUpdate;
        typedef RadioMonitorUtils::CorbaMonitorUpdate CorbaMonitorUpdate;
    
        /**
         * MonitoredCallStackListCtrl
         */
        MonitoredCallStackListCtrl();
        ~MonitoredCallStackListCtrl();
		
		void Init();
        /**
         * resetContents
         *
         * Completely resets this list control (making it empty)
         *
         */
        void resetContents();
        
        /**
         * getSubscriber
         *
         * @return the subscriber used for this list control
         */
        TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>& getSubscriber() { if(m_messageSubscriber) return *m_messageSubscriber; }

    protected:

    	// Generated message map functions
    	//{{AFX_MSG(MonitoredCallStackListCtrl)
    	virtual void PreSubclassWindow();
    	afx_msg void OnDestroy();
        afx_msg LRESULT OnMonitorUpdate(UINT wParam, LONG lParam);
        //}}AFX_MSG
    	DECLARE_MESSAGE_MAP()

        /**
         * Intercept all messages to allow sorting
         *
         */
    	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    private:
    
        /**
         * processUpdate
         *
         * Processes a set of update data received from our subscription
         *
         * @param updateData the up to date information that we should refresh with
         *
         */
        void processUpdate(const MonitorUpdate& updateData);
        
        /**
         * findExisting
         *
         * Finds the index of the item in the list corresponding to 'updateData'
         *
         * @return -1 if item not found, otherwise index into list
         *
         * @param updateData the data used to search through list
         *
         */
        int findExisting(const MonitorUpdate& updateData) const;
        
        /**
         * refreshListItem
         *
         * @param nItem defines the item index whose to have it's row contents updated (from item data)
         *
         */
        void refreshListItem(int nItem);

        /// Subscriber for Radio Agent updates
        TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* m_messageSubscriber;        

        TA_Base_Bus::AutoSortListCtrl::SortInfo m_colSortingInfo;
    };

} // end namespace TA_IRS_App
