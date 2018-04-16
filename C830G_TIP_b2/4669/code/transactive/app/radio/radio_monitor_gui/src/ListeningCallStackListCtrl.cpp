/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/ListeningCallStackListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides list control for displaying status of radio calls that
  * are currently being listened in on
  *
  */

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "app/radio/radio_monitor_gui/src/ListeningCallStackListCtrl.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/RadioComms_MessageTypes.h"

using namespace TA_IRS_App::RadioMonitor;

namespace TA_IRS_App
{
    static const char COL_TITLE_TIME[] = "Time";
    static const char COL_TITLE_CALL[] = "Source";        // Source caller name (or CONFERENCE)
    static const char COL_TITLE_NUMBER[] = "Destination";
    // Changed to "Loc", TES #570
    static const char COL_TITLE_LOCATION[] = "Loc";    // Destination location
    
    static enum
    {
        COL_TIME_INDEX      = 0,
        COL_CALL_INDEX      = 1,
        COL_NUMBER_INDEX    = 2,
        COL_LOCATION_INDEX  = 3
    };

    ListeningCallStackListCtrl::ListeningCallStackListCtrl()
    :
    m_messageSubscriber(NULL)
    {
    }

	ListeningCallStackListCtrl::~ListeningCallStackListCtrl()
	{
	}

    BEGIN_MESSAGE_MAP(ListeningCallStackListCtrl, CTreeListCtrl)
	    //{{AFX_MSG_MAP(ListCtrlSelNoFocus)
        ON_MESSAGE(WM_RM_LISTEN_CALL_STATUS_UPDATE, OnCallStatusUpdate)
        ON_MESSAGE(WM_RM_LISTEN_CALL_UPDATE, OnListenCallUpdate)
        ON_WM_DESTROY()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

	void ListeningCallStackListCtrl::Init()
	{
	    m_messageSubscriber = new TA_IRS_Bus::SimpleMessageSubscriber<CorbaListenCallUpdate>(TA_Base_Core::RadioComms::RadioMonitorNotification, 
                        WM_RM_LISTEN_CALL_UPDATE,
                        CachedConfig::getInstance().getEntityLocationKey());
        try
        {
            m_messageSubscriber->addListenerWindow(m_hWnd);
			TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage
				( TA_Base_Core::RadioComms::RadioCallStatus, this, 
					CachedConfig::getInstance().getLocalRadioAgentEntityKey(),0, 
					CachedConfig::getInstance().getEntityLocationKey(), NULL );
        }
        DEFAULT_RM_CATCH_HANDLER("ListeningCallStackListCtrl subscription");

	}

    void ListeningCallStackListCtrl::PreSubclassWindow()
    {
        CTreeListCtrl::PreSubclassWindow();

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT);
        // Turn icons off (NOTE: Class doesn't work properly with icons atm..)
        setupIndentWithoutIcons();

        InsertColumn(COL_TIME_INDEX, COL_TITLE_TIME);
        InsertColumn(COL_CALL_INDEX, COL_TITLE_CALL);
        InsertColumn(COL_NUMBER_INDEX, COL_TITLE_NUMBER);
        InsertColumn(COL_LOCATION_INDEX, COL_TITLE_LOCATION);
        
        SetColumnWidth(COL_TIME_INDEX,      109);
        SetColumnWidth(COL_CALL_INDEX,      145);
        SetColumnWidth(COL_NUMBER_INDEX,    145);
        SetColumnWidth(COL_LOCATION_INDEX,  LVSCW_AUTOSIZE_USEHEADER);
    }


    void ListeningCallStackListCtrl::OnDestroy()
    {
        if(m_messageSubscriber !=  NULL)
		{
			try
			{
				m_messageSubscriber->removeListenerWindow(m_hWnd);
				TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
			}
			DEFAULT_RM_CATCH_HANDLER("ListeningCallStackListCtrl unsubscription");        
		}
    }


    LRESULT ListeningCallStackListCtrl::OnListenCallUpdate(UINT wParam, LONG lParam)
    {
        if (!m_messageSubscriber->isConnectedListenerWindow(m_hWnd))
        {
            // Must be closing down, ignore message as subscriber won't have it
            // any longer if we're no longer connected
            return 0;
        }

        try
        {           
            const CorbaListenCallUpdate update = m_messageSubscriber->getMessageData(wParam, m_hWnd);            
            processUpdate(RadioMonitorUtils::convertCorbaStruct(update));        
        }
        DEFAULT_RM_CATCH_HANDLER("MonitoredCallStack OnMonitorUpdate");        

        return 0;
    }


	LRESULT ListeningCallStackListCtrl::OnCallStatusUpdate(UINT wParam, LONG lParam)
    {	
		TA_Base_Core::ThreadGuard guard(m_activeCallLock);

		long callID = lParam;
		
		logActiveCallDetails();
		
		CallSequence::iterator iter = m_activeCalls.begin();
		for(; iter!=m_activeCalls.end(); ++iter)
		{
			if(iter->callID == callID)
			{
				m_activeCalls.erase(iter);
                performFullListRefresh();
				break;
			}
		}

        return 0;
    }

    long ListeningCallStackListCtrl::getCallIdFromIndex(int visualIndex)
    {
		TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        const ListenCallUpdate* lcu = reinterpret_cast<ListenCallUpdate*>(GetVItemData(visualIndex));
        
        return lcu->callID;
    }
    

    void ListeningCallStackListCtrl::processUpdate(const ListenCallUpdate& updateData)
    {
		TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        CallSequence::iterator existingCallItr = findExisting(updateData);
        
        saveSelection();

        // As updates come in, we perform partial update of list control
        // (don't need to do full refresh)                
        switch (updateData.callStatus)
        {
        case TA_Base_Bus::IRadioCorbaDef::Listening:
        case TA_Base_Bus::IRadioCorbaDef::Included:
        case TA_Base_Bus::IRadioCorbaDef::Joined:
            // Need to add to or update list (depending on if already there or not)
            if (m_activeCalls.end() == existingCallItr)
            {
                // Item(s) needs to be added
                m_activeCalls.push_back(updateData);

                performFullListRefresh();
            }
            else
            {
                // Refresh required
                attemptPartialRefresh(existingCallItr, updateData);
            }
            
            break;
        case TA_Base_Bus::IRadioCorbaDef::Monitoring:        // FALLTHROUGH
        case TA_Base_Bus::IRadioCorbaDef::Stopped:                        
            // Need to remove from list (if not in there, just ignore request)                
            if (m_activeCalls.end() != existingCallItr)
            {
                m_activeCalls.erase(existingCallItr);

                performFullListRefresh();
            }
            break;
        default:
            LOG( SourceInfo, 
                    TA_Base_Core::DebugUtil::GenericLog, 
                    TA_Base_Core::DebugUtil::DebugError, 
                    "MonitoredCallStackListCtrl::processUpdate: Invalid callStatus");

            RadioMonitorErrorHandler::getInstance().displayError(
                RadioMonitorErrorHandler::INVALID_MONITOR_DATA_RECEIVED);

            // Ignore this update
            break;
        }

        restoreSelection();
    }


    void ListeningCallStackListCtrl::resetContents()
    {
		if(this->m_hWnd != NULL)
		{
			DeleteAllItems();
		}
    }


    // Can't use TA_Base_Bus::ListCtrlSelNoFocus for this functionality
    // (because already inheriting from CTreeListCtrl)
    // NOTE: Now TreeListCtrl inherits from ListCtrlSelNoFocus
    /*
    void ListeningCallStackListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
    {
	    *pResult = CDRF_DODEFAULT;
        NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

        // used to keep track of item state between prepaint and postpaint
        static bool itemIsHighlighted = false;

        switch (pLVCD->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:

            // We want item prepaint notifications, so...
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;

        case CDDS_ITEMPREPAINT:
            {
                // let windows do the drawing
                *pResult = CDRF_DODEFAULT;

                // get the current item being drawn
                int row = pLVCD->nmcd.dwItemSpec;

                // store the state of this row for the post paint
                itemIsHighlighted = ( LVIS_SELECTED == GetItemState(row, LVIS_SELECTED) );

                // if the item is selected
                if (itemIsHighlighted)
                {
                    // set the colours to the highlight colours
                    pLVCD->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
                    pLVCD->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);

                    // Turn off listview highlight otherwise it uses the system colors
                    // out of focus this is black on grey
                    SetItemState(row, ~LVIS_SELECTED, LVIS_SELECTED);

                    // We want item post-paint notifications, so...
                    *pResult |= CDRF_NOTIFYPOSTPAINT;
                }               
            }
            break;

        case CDDS_ITEMPOSTPAINT:
            {
                // if we deselected the item in prepaint
                if (itemIsHighlighted)
                {
                    int row = pLVCD->nmcd.dwItemSpec;

                    // reselect it again
                    SetItemState(row, LVIS_SELECTED, LVIS_SELECTED);
                }

                *pResult = CDRF_DODEFAULT;
            }
            break;

        default:
            *pResult = CDRF_DODEFAULT;
            break;
        }

        return;
    }
    */
            
    void ListeningCallStackListCtrl::attemptPartialRefresh(CallSequence::iterator callItr, const ListenCallUpdate& updateData)
    {
        // If we simply need to update rows (no insertion / deletion), then do just that
        // Otherwise need to completely reset (and refresh) the list control
        // Check if full refresh required..
        if (updateData.participants.size() != callItr->participants.size())
        {
            // If there is any change in the number of participants, must perform full refresh
            // (because it requires insertions and deletions)
            (*callItr) = updateData;
            
            performFullListRefresh();
        }
        else
        {
            // Otherwise we're right to perform a partial update (of this group only)
            int nItem = findItemIndex(*callItr);

            if (nItem >= 0)
            {            
                (*callItr) = updateData;
                                
                refreshListItem(nItem, *callItr);                
            }
            else
            {
                LOG( SourceInfo, 
                    DebugUtil::GenericLog, 
                     DebugUtil::DebugError, 
                    "ListeningCallStackListCtrl::attemptPartialRefresh: Find error #1");   

                // Internal error within this class (programming error)
                RadioMonitorErrorHandler::getInstance().displayError(
                    RadioMonitorErrorHandler::INTERNAL_FAILURE);
            }
        }
    }


    void ListeningCallStackListCtrl::performFullListRefresh()
    {
		//already guard in invocate method
		//TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        // This is called when the user may have an active selection, so where possible
        // endeavour to remember (and restore) the current selection when doing the refresh
        // - Additionally, store (and restore) the expanded states for each node..
        CallDataBase* existingSelection = getSelectedItem();
        
        // For all parent nodes, store their expanded state
        saveExpandedStates();

        int vScrollPos = GetScrollPos(SB_VERT);
        
        TA_Base_Bus::LockControlRedraw redrawLock(*this);
         
        // Must clear all itemdata components as call list has changed
		if(this->m_hWnd != NULL)
		{
			DeleteAllItems();
		}
        
        for (CallSequence::iterator itr = m_activeCalls.begin(); itr != m_activeCalls.end(); ++itr )
        {
            int parentVIndex = AddItem("", 0, 0);
            
            refreshListItem(parentVIndex, (*itr));

            for (std::list<RmCallParticipant>::iterator participantItr = itr->participants.begin();
                        participantItr != itr->participants.end();
                        participantItr ++)
            {
                int childVIndex = AddItem("", 0, 1);
                refreshParticipantListItem(childVIndex, (*participantItr));
            }

            // Check if we need to expand this item
            ListenCallUpdate* pItem = &(*itr);
            ExpandedStateMap::iterator itr = m_expandedStates.find(pItem);

            // there should be a corresponding entry for each parent node that
            // was in the list prior to the refresh - if item not found - 
            // it means this item was just added (and had no previous state)..
            if (itr != m_expandedStates.end() && itr->second)
            {
                // Parent item has expanded state..
                int aIndex = GetActualIndexFromVirtual(parentVIndex);
                if (aIndex < 0)
                {
                    LOG( SourceInfo, 
                        DebugUtil::GenericLog, 
                        DebugUtil::DebugError, 
                        "ListeningCallStackListCtrl::performFullListRefresh: Find error #2");   

                    RadioMonitorErrorHandler::getInstance().displayError(
                            RadioMonitorErrorHandler::INTERNAL_FAILURE);                                
                }

                Expand(aIndex);
            }
        }

        int vScrollMax = GetScrollLimit(SB_VERT);
        
        ///////////////////
        // Restore vertical scroll position
        if (GetItemCount() > 0 && vScrollMax > 0)
        {            
            
            // This code, for some reason, doesn't work  - if you call Scroll(5, 0) it works
            // scrolling horizontally.. but call with (0, 5) and there is no vertical scrolling
            /*
            float vCap = 1.0f;
            // Cap at 1 (in case scroll range was reduced)
            float proportion = __min(vCap, float(vScrollPos) / float(vScrollMax));
            CRect rc;
            GetClientRect(rc);
            
            RECT item;
            GetItemRect(0, &item, LVIR_LABEL);
            // Determine point we should scroll up to
            // It will be capped at 'vScrollMax' - when we scroll to 'vScrollMax' full right side in view
            int moveDistancePix = (int)((item.bottom - item.top) * (proportion * vScrollMax));
            CSize scrollAmount(0, moveDistancePix);
            //SetIconSpacing(5, 5);
            Scroll(scrollAmount);
            */

            // To make scrolling work, use EnsureVisible.
            // The parameter vSize defines number of items visible in one page
            // The scroll position vScrollPos defines the offset from
            // vSize, ie vScrollPos + vSize - which is our target
            int vSize = GetCountPerPage();
            int lowerTarget = vSize - 1 + vScrollPos;
            if (lowerTarget >= GetItemCount() - 1)
            {
                lowerTarget = GetItemCount() - 1;
            }
            // lowerTarget should define the (visually) lower bound of the scroll range
            // - since we know we're starting from above, at index 0
            EnsureVisible(lowerTarget, false);
        }
    }

    

    void ListeningCallStackListCtrl::refreshListItem(int vItem, const ListenCallUpdate& updateData)
    {
        SetVItemText(vItem, COL_TIME_INDEX, updateData.callTime.c_str());

        std::ostringstream sourceString;
        std::ostringstream destString;
        std::string destLocation;

        if (TA_Base_Bus::IRadioCorbaDef::HalfDuplex == updateData.callCategory)
        {
            // If a conference call, display "CONFERERENCE"
            sourceString << RadioMonitorUtils::getCallCategoryString(updateData.callCategory);
            // and no destination/location details
            // destString
            // destLocation
        }
        else
        {
            // Otherwise show the identity of the callers
            sourceString << updateData.callerName << " (" << updateData.callerTSI << ")";
            destString << updateData.calledName << " (" << updateData.calledTSI << ")";
            destLocation = updateData.calledLocation;
        }
        
        SetVItemText(vItem, COL_CALL_INDEX, sourceString.str().c_str());        
        SetVItemText(vItem, COL_NUMBER_INDEX, destString.str().c_str());        
        SetVItemText(vItem, COL_LOCATION_INDEX, destLocation.c_str());    
        
        SetVItemData(vItem, reinterpret_cast<DWORD>(&updateData));
    }


    void ListeningCallStackListCtrl::refreshParticipantListItem(int vItem, const RmCallParticipant& updateData)
    {
        // Leave time column blank..
        std::ostringstream ostr;
        ostr << updateData.name << " (" << updateData.TSI << ")";
        SetVItemText(vItem, COL_CALL_INDEX, ostr.str().c_str());

        SetVItemText(vItem, COL_LOCATION_INDEX, updateData.location.c_str());   
        
        SetVItemData(vItem, reinterpret_cast<DWORD>(&updateData));           
    }    


    int ListeningCallStackListCtrl::findItemIndex(const ListenCallUpdate& updateData)
    {
		//already guard in invocate method
		//TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        for (int i = 0; i < GetVItemCount(); i ++)
        {
            const ListenCallUpdate* id = reinterpret_cast<ListenCallUpdate*>(GetVItemData(i));
            if (&updateData == id)
            {
                return i;
            }
        }

        return -1;
    }


    ListeningCallStackListCtrl::CallSequence::iterator 
        ListeningCallStackListCtrl::findExisting(const ListenCallUpdate& updateData)
    { 
		//already guard in processUpdate()
		//TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        // Inspect each item element in the list, and see if it's a match with updateData
        for (CallSequence::iterator itr = m_activeCalls.begin(); itr != m_activeCalls.end(); itr ++)
        {
            const ListenCallUpdate& check = (*itr);

            // There are three elements uniquely identifying each entry
            // callID, callerName, calledName
            // callID is unique for calls, but in a group call, can be duplicate entries
            // (different parties of the same call)
            if (check.keyMatch(updateData))
            {
                if (-1 == findItemIndex(*itr))
                {
                    LOG( SourceInfo, 
                        DebugUtil::GenericLog, 
                         DebugUtil::DebugError, 
                        "ListeningCallStackListCtrl::findExisting: Find error #1");   

                    RadioMonitorErrorHandler::getInstance().displayError(
                                    RadioMonitorErrorHandler::INTERNAL_FAILURE);                                
                }

                // Match found
                return itr;
            }
        }
    
        return m_activeCalls.end();
    }     


    CallDataBase* ListeningCallStackListCtrl::getSelectedItem()
    {
        if (1 != GetSelectedCount())
        {
            return 0;
        }

        int nItem = GetNextItem(-1, LVNI_SELECTED);

        if (nItem >= 0)
        {
            // Must obtain virtual index from actual (visual) index
            int vIndex = GetVirtualIndexFromActual(nItem);

            if (vIndex >= 0)
            {
                CallDataBase* result = reinterpret_cast<CallDataBase*>(GetVItemData(vIndex));
                return result;
            }
        }
        
        return 0;        
    }


    void ListeningCallStackListCtrl::saveExpandedStates()
    {
		TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        m_expandedStates.clear();

        // Go through all parent nodes (the active calls), and store in a map for rapid access
        for (CallSequence::iterator itr = m_activeCalls.begin(); itr != m_activeCalls.end(); ++ itr)
        {
            // Start off with no expanded states
            m_expandedStates.insert(ExpandedStateMap::value_type(&(*itr), false));
        }

        for (int vItem = 0; vItem < GetVItemCount(); ++vItem )
        {
            ListenCallUpdate* id = reinterpret_cast<ListenCallUpdate*>(GetVItemData(vItem));
            
            ExpandedStateMap::iterator itr = m_expandedStates.find(id);
            
            // If the entry isn't found in map, means this item was just removed from
            // the call stack, so don't need to save state - OR it's not a parent
            // node (a child RmCallParticipant object)
            if (itr == m_expandedStates.end())
            {
                continue;                   
            }
            
            if (!isVItemCollapsed(vItem))
            {
                itr->second = true;
            }
        }

    }


    bool ListeningCallStackListCtrl::isParentNodeSelected()
    {
		TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        CallDataBase* pSel = getSelectedItem();

        if (0 != pSel)
        {
            // Confirm the currently selected item is a paret node
            ListenCallUpdate* parent = dynamic_cast<ListenCallUpdate*>(pSel);

            if (0 != parent)
            {
                return true;
            }
            // Otherwise it's of type RmCallParticipant, a child node..
        }

        return false;
    } 


    void ListeningCallStackListCtrl::saveSelection()
    {
		//already guard in processUpdate()
		//TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        CallDataBase* pSel = getSelectedItem();

        if (0 != pSel)
        {
            ListenCallUpdate* parent = dynamic_cast<ListenCallUpdate*>(pSel);
            RadioMonitor::RmCallParticipant* child = dynamic_cast<RadioMonitor::RmCallParticipant*>(pSel);
            
            if (0 != parent)
            {
                m_selectedCall.setValue(*parent);
            }
            if (0 != child)
            {
                m_selectedParticipant.setValue(*child);
            }
        }
    }


    void ListeningCallStackListCtrl::restoreSelection()
    {   
		//already guard in processUpdate()
		//TA_Base_Core::ThreadGuard guard(m_activeCallLock);

        int selCount = 0;
        
        if (GetSelectedCount() > 0)
        {
            TA_Base_Bus::SelectionHelper::deselectAll(*this);
        }

        for (int nItem = 0; nItem < GetItemCount(); ++nItem ) 
        {
            int vIndex = GetVirtualIndexFromActual(nItem);
    
            CallDataBase* itemData = reinterpret_cast<CallDataBase*>(GetVItemData(vIndex));
            
            ListenCallUpdate* parent = dynamic_cast<ListenCallUpdate*>(itemData);
            RadioMonitor::RmCallParticipant* child = dynamic_cast<RadioMonitor::RmCallParticipant*>(itemData);
            
            if (0 != parent && m_selectedCall.hasBeenSet())
            {
                // This is parent node, check for match
                if (m_selectedCall.getValue().keyMatch(*parent))
                {
                    SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
                    selCount ++;
                }
            }
            else if (0 != child && m_selectedParticipant.hasBeenSet())
            {
                // This is a child node, check for match
                if (m_selectedParticipant.getValue().keyMatch(*child))
                {
                    SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
                    selCount ++;                    
                }
            }
        }
    
        // selCount should be 0 (selected item was removed) or 1 (re-selected OK)
        if (selCount > 1)
        {
            // To be safe, in the event of ambiguity..
            TA_Base_Bus::SelectionHelper::deselectAll(*this);

            LOG( SourceInfo, 
                DebugUtil::GenericLog, 
                 DebugUtil::DebugError, 
                "ListeningCallStackListCtrl::performFullListRefresh: Re-selection failure");   
    
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::INTERNAL_FAILURE);
        }
        
        // Reset save points
        m_selectedCall.resetValue();
        m_selectedParticipant.resetValue();
    }   


	    void ListeningCallStackListCtrl::receiveSpecialisedMessage(
        const CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
      
        TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusNotification;


		if( (std::string(message.messageTypeKey) == TA_Base_Core::RadioComms::RadioCallStatus.getTypeKey()) 
					&& ((message.messageState >>= callStatusNotification) != 0) )
		{
			//
			// If the radio notification is of type Ringing, add a new radio
			// call entry to the call banner.
			// If it is of any other type, there should be an existing call entry,
			// which is now deleted.
			//
			switch(callStatusNotification->callStatus)
			{
				// Ringing
				case TA_Base_Bus::IRadioCorbaDef::Disconnected:
				case TA_Base_Bus::IRadioCorbaDef::Deleted:
					processCallStatusUpdate(callStatusNotification->callID);
					break;

				// Other
				default:
					break;
			}// switch(callStatusNotification->callStatus)

		}

        FUNCTION_EXIT;
    }

	void ListeningCallStackListCtrl::processCallStatusUpdate(long callID)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Call status update, call id = %d", callID);

		SendMessage(WM_RM_LISTEN_CALL_STATUS_UPDATE, 0, callID);
//		TA_Base_Core::ThreadGuard guard(m_activeCallLock);
//
//		logActiveCallDetails();
//
//		CallSequence::iterator iter = m_activeCalls.begin();
//		for(; iter!=m_activeCalls.end(); ++iter)
//		{
//			if(iter->callID == callID)
//			{
//				m_activeCalls.erase(iter);
//                performFullListRefresh();
//				break;
//			}
//		}
	}

	void ListeningCallStackListCtrl::logActiveCallDetails()
	{
		std::stringstream str;
		str << "there are " << m_activeCalls.size() << "entry in the call.";
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, str.str().c_str());

		int i = 1;
		for(CallSequence::iterator iter = m_activeCalls.begin(); iter!=m_activeCalls.end(); ++iter)
		{
			std::stringstream item;
			str << "num " << i++ << ":"
				<< " call id (" << iter->callID << ") "
				<< " sessionRef (" <<iter->sessionRef << ") "
				<< " monitorID (" << iter->monitorID << ") "
				<< " callTime (" << iter->callTime << ") "
				<< " callerName (" << iter->callerName << ") "
				<< " callerTSI (" << iter->callerTSI << ") "
				<< " callerLocation (" << iter->callerLocation << ") "
				<< " calledName (" << iter->calledName << ") "
				<< " calledTSI (" << iter->calledTSI << ") "
				<< " calledLocation (" << iter->calledLocation << ") ";

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " %s ." , item.str().c_str());

		}	
	}

} // end namespace TA_
