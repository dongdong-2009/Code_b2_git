/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/MonitoredCallStackListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Provides list control for displaying status of radio calls that
  * are currently being monitored
  *
  */

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/MonitoredCallStackListCtrl.h"
#include "core/message/types/RadioComms_MessageTypes.h"

namespace TA_IRS_App
{
    static const char COL_TITLE_SOURCE[]    = "Source";
    static const char COL_TITLE_DESTINATION[] = "Destination";
    static const char COL_TITLE_TYPE[]      = "Type";

    static enum 
    {
        COL_SOURCE_INDEX = 0,
        COL_DESTINATION_INDEX = 1,
        COL_TYPE_INDEX = 2,
        COL_MAX
    };

    MonitoredCallStackListCtrl::MonitoredCallStackListCtrl()
    :
    m_messageSubscriber(NULL),
    m_colSortingInfo(COL_MAX)
    {        
    }

	MonitoredCallStackListCtrl::~MonitoredCallStackListCtrl()
	{
	}

    BEGIN_MESSAGE_MAP(MonitoredCallStackListCtrl, ListCtrlSelNoFocus)
	    //{{AFX_MSG_MAP(MonitoredCallStackListCtrl)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_RM_MONITOR_CALL_UPDATE, OnMonitorUpdate)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()
    

	void MonitoredCallStackListCtrl::Init()
	{
		m_messageSubscriber = new TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate> (TA_Base_Core::RadioComms::RadioMonitorNotification, 
                        WM_RM_MONITOR_CALL_UPDATE,
                        CachedConfig::getInstance().getEntityLocationKey());
        try
        {
            m_messageSubscriber->addListenerWindow(m_hWnd);
        }
        DEFAULT_RM_CATCH_HANDLER("MonitoredCallStack subscription");        

	}

    void MonitoredCallStackListCtrl::PreSubclassWindow()
    {
        ListCtrlSelNoFocus::PreSubclassWindow();

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT);

        InsertColumn(COL_SOURCE_INDEX, COL_TITLE_SOURCE);
        InsertColumn(COL_DESTINATION_INDEX, COL_TITLE_DESTINATION);
        InsertColumn(COL_TYPE_INDEX, COL_TITLE_TYPE);

        SetColumnWidth(COL_SOURCE_INDEX, 180);
        SetColumnWidth(COL_DESTINATION_INDEX, 180);
        // Expand to take up full length of right side
        SetColumnWidth(COL_TYPE_INDEX, LVSCW_AUTOSIZE_USEHEADER);
    }


    void MonitoredCallStackListCtrl::OnDestroy()
    {
        if(m_messageSubscriber != NULL)
		{
		    try
			{
				m_messageSubscriber->removeListenerWindow(m_hWnd);
				delete m_messageSubscriber;
			}
			DEFAULT_RM_CATCH_HANDLER("MonitoredCallStack unsubscription");     	
		}
    }


    LRESULT MonitoredCallStackListCtrl::OnMonitorUpdate(UINT wParam, LONG lParam)
    {
        if (!m_messageSubscriber->isConnectedListenerWindow(m_hWnd))
        {
            // Must be closing down, ignore message as subscriber won't have it
            // any longer if we're no longer connected
            return 0;
        }

        try
        {  
            const CorbaMonitorUpdate update = m_messageSubscriber->getMessageData(wParam, m_hWnd);            
            processUpdate(RadioMonitorUtils::convertCorbaStruct(update));        
        }
        DEFAULT_RM_CATCH_HANDLER("MonitoredCallStack OnMonitorUpdate");

        return 0;
    }
    

    void MonitoredCallStackListCtrl::processUpdate(const MonitorUpdate& updateData)
    {
        int nItem = findExisting(updateData);

        // As updates come in, we perform partial update of list control
        // (don't need to do full refresh)                
        switch (updateData.callStatus)
        {
        case TA_Base_Bus::IRadioCorbaDef::Monitoring:
            // Need to add to or update list (depending on if already there or not)
            if (nItem < 0)
            {
                // Item needs to be added, add at end
                nItem = InsertItemPtr(GetItemCount(), "", updateData);
            }
            else
            {
                RefreshItemDataPtr(nItem, updateData);
            }
            
            refreshListItem(nItem);
            break;
        case TA_Base_Bus::IRadioCorbaDef::Listening: // FALLTHROUGH
        case TA_Base_Bus::IRadioCorbaDef::Included:
        case TA_Base_Bus::IRadioCorbaDef::Joined:
        case TA_Base_Bus::IRadioCorbaDef::Stopped:
            {
                // Need to remove from list (if not in there, just ignore request)                
                if (nItem >= 0)
                {
                    DeleteItem(nItem);
                }
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
    }


    void MonitoredCallStackListCtrl::refreshListItem(int nItem)
    {
        const MonitorUpdate& updateData = GetItemDataRef(nItem);
        
        std::ostringstream ostr;
        ostr << updateData.callerName << " (" << updateData.callerTSI << ")";
        SetItemText(nItem, COL_SOURCE_INDEX, ostr.str().c_str());

        ostr.str("");
        ostr << updateData.calledName << " (" << updateData.calledTSI << ")";
        SetItemText(nItem, COL_DESTINATION_INDEX, ostr.str().c_str());

        std::string callCatStr = RadioMonitorUtils::getCallCategoryString(updateData.callCategory);
        
        SetItemText(nItem, COL_TYPE_INDEX, callCatStr.c_str());
    }


    int MonitoredCallStackListCtrl::findExisting(const MonitorUpdate& updateData) const
    {
        // Inspect each item element in the list, and see if it's a match with updateData
        for (int i = 0; i < GetItemCount(); i ++)
        {
            const MonitorUpdate& check = GetItemDataRef(i);

            // There are three elements uniquely identifying each entry
            // callID, callerName, calledName
            // callID is unique for calls, but in a group call, can be duplicate entries
            // (different parties of the same call)
            if (check.callID == updateData.callID && 
                check.callerTSI == updateData.callerTSI &&
                check.calledTSI == updateData.calledTSI)
            {
                // Match found
                return i;
            }
        }

        return -1;
    }      


    void MonitoredCallStackListCtrl::resetContents()
    {
		if(this->m_hWnd != NULL)
        {
			DeleteAllItems();
		}
    }

    
    LRESULT MonitoredCallStackListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_colSortingInfo);    

        return ManagedListCtrl<RadioMonitor::RmMonitorUpdate, TA_Base_Bus::ListCtrlSelNoFocus>::WindowProc(message, wParam, lParam);
    }

} // end namespace TA_
