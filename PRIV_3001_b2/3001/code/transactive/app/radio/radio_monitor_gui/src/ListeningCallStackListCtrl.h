/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/ListeningCallStackListCtrl.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Provides list control for displaying status of radio calls that
  * are currently being listened in on
  *
  */

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/tree_list_control/TreeListCtrl.h"
#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorUtils.h"

namespace TA_IRS_App
{
    class ListeningCallStackListCtrl 
    : 
    public CTreeListCtrl, public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    public:

        typedef RadioMonitor::RmMonitorUpdate ListenCallUpdate;
        typedef RadioMonitorUtils::CorbaMonitorUpdate CorbaListenCallUpdate;
        typedef std::list<ListenCallUpdate> CallSequence;

        /**
         * ListeningCallStackListCtrl
         */
        ListeningCallStackListCtrl();
		~ListeningCallStackListCtrl();

		void Init();
        /**
         * resetContents
         *
         * Completely resets this list control (making it empty)
         *
         */
        void resetContents();
        
        /**
         * getCallIdFromIndex
         *
         * Fetches the ID of the call for the logical (visual) index defined by 'index'
         *  (needs to map to virtual index, accounting for collapsed items etc)
         *
         * @exception ValueNotSetException if there is no Call ID set (or found) for the 
         *              specified index
         */
        long getCallIdFromIndex(int index);
        
        /**
         * getSubscriber
         *
         * @return the subscriber used for this list control
         */
        TA_IRS_Bus::SimpleMessageSubscriber<CorbaListenCallUpdate>& getSubscriber() { if(m_messageSubscriber) return *m_messageSubscriber; }

        /**
         * hasSelectedParentNode
         *
         * @return true if there is a single selection in this call stack, and
         *          it corresponds to a parent node (doesn't need to have children)
         */
        bool isParentNodeSelected();


        /**
         *  Called when a Radio notification is received from the
         *  Message subsystem.
         */
        virtual void receiveSpecialisedMessage(
            const TA_Base_Core::CommsMessageCorbaDef& message);

	    // Generated message map functions
    protected:

	    //{{AFX_MSG(ListeningCallStackListCtrl)
        virtual void PreSubclassWindow();        

        afx_msg void OnDestroy();
        afx_msg LRESULT OnListenCallUpdate(UINT wParam, LONG lParam);
        afx_msg LRESULT OnCallStatusUpdate(UINT wParam, LONG lParam);        
	    //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        /**
         * getSelectedItem
         *
         * @return pointer to the currently selected item, or null if no current selection (or multiple
         *          items selected)
         */
        RadioMonitor::CallDataBase* getSelectedItem();


        /**
         * processUpdate
         *
         * Processes a set of update data received from our subscription
         *
         * @param updateData the up to date information that we should refresh with
         *
         */
        void processUpdate(const ListenCallUpdate& updateData);

        /**
         * refreshListItem
         *
         * Called to perform a visual refresh of the data displayed at row defined
         *      by vItem - where vItem defines a parent node
         *
         * @param vItem defines the vritual index of the item to be refreshed
         * 
         * @param updateData defines the updated information
         *
         */
        void refreshListItem(int vItem, const ListenCallUpdate& updateData);

        /**
         * refreshParticipantListItem
         *
         * Called to perform a visual refresh of the data displayed at the row
         *  defined by vItem
         *
         * @param vItem defines the virtual index of the item to be refreshed
         * 
         * @param updateData defines the updated information
         *
         */
        void refreshParticipantListItem(int vItem, const RadioMonitor::RmCallParticipant& updateData);
    
        /**
         * attemptPartialRefresh
         *
         * Attempts to perform a partial refresh of the list control (without having
         *  to refresh entire tree list)
         *
         * @param callItr
         */
        void attemptPartialRefresh(CallSequence::iterator callItr, const ListenCallUpdate& updateData);
    

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
        CallSequence::iterator findExisting(const ListenCallUpdate& updateData);

        /**
         * findItemIndex
         *
         * @return the list control index of the call which corresponds to the input item
         *       (by comparing memory points against item data), -1 if not found
         * @param updateData reference to the item we're searching for
         *
         * @see findExisting (can use return value from this as input to this function)
         * 
         */
        int findItemIndex(const ListenCallUpdate& updateData);

        /**
         * performFullListRefresh
         * 
         * Completely refreshes the contents of this tree list control
         *  (required since tree list control doesn't handle insertions / deletions)
         *
         */
        void performFullListRefresh();

        /**
         * saveExpandedStates
         *
         * Examines the current state of the list control, and stores the expanded
         *  state of each parent node in the m_expandedStates map
         * m_expandedStates is cleared, then refreshed with state information
         */
        void saveExpandedStates();

        /**
         * saveSelection
         *
         * Saves the currently selected item, if any, for later selection with restoreSelection
         */
        void saveSelection();

        /**
         * restoreSelection
         *
         * Restores previous selection that was saved with saveSelection
         */
        void restoreSelection();

		void processCallStatusUpdate(long callID);
		void logActiveCallDetails();
		
        /// Subscriber for Radio Agent updates
        TA_IRS_Bus::SimpleMessageSubscriber<CorbaListenCallUpdate>* m_messageSubscriber;

        // Calls being actively listened to - this list MUST be synchronised with
        // the itemdata contents of the list (because GetItemData will point to an
        // element contained in this list).  Therefore upon any insertions or deletion
        // we must discard the list and refresh it completely (or at least invalidate
        // the itemdata component that was pointing to the element removed)
        CallSequence m_activeCalls;
		TA_Base_Core::ReEntrantThreadLockable m_activeCallLock;

        typedef std::map<ListenCallUpdate*, bool> ExpandedStateMap;
        ExpandedStateMap m_expandedStates;

        TA_Base_Core::PrimitiveWrapper<ListenCallUpdate>   m_selectedCall;
        TA_Base_Core::PrimitiveWrapper<RadioMonitor::RmCallParticipant>    m_selectedParticipant;
    };

} // end namespace TA_IRS_App
