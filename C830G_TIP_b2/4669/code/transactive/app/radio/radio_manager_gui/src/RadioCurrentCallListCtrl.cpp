/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioCurrentCallListCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 15:50:09 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioCurrentCallListCtrl.cpp : implementation file
//

#pragma warning (disable : 4786)

#include "stdafx.h"

#include <algorithm>

#include "radiomanager.h"
#include "radiomanagerDlg.h"
#include "RadioCurrentCallListCtrl.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "core\message\src\MessageSubscriptionManager.h"
#include "core\synchronisation\src\ThreadGuard.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"

#include "bus/radio/radiosearchpage/src/SearchPage.h"
#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"

#ifdef RUN_IN_DEBUG_MODE
    #include "RadioManagerTestData.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int CALL_NOTIFICATION_MSG = WM_USER + 100;
const int PTT_NOTIFICATION_MSG = WM_USER + 101;
const int INIT_NOTIFICATION_MSG = WM_USER + 102;


/////////////////////////////////////////////////////////////////////////////
// RadioCurrentCallListCtrl

RadioCurrentCallListCtrl::RadioCurrentCallListCtrl() 
                         : CTreeListCtrl(), 
						   m_sessionRef(0),
						   m_bSubscribed(false),
						   m_radioEntityKey(0),
                           m_radioEntityLocation(0),
						   m_isLoaded(false)

{
 	FUNCTION_ENTRY("Constructor");

	//HICON hIcon;
	// Setup image list
	m_ImageList.Create(16,16,ILC_COLOR8,4,4);

	FUNCTION_EXIT;
}

RadioCurrentCallListCtrl::~RadioCurrentCallListCtrl()
{
 	FUNCTION_ENTRY("Destructor");

    try
    {
        TA_ASSERT(!m_bSubscribed, "Subscription should have been cancelled in OnDestroy");

        // Delete all allocated memory present in call map following subscription cancellation
        for (CallMap::iterator it = m_callMap.begin(); it != m_callMap.end(); ++it )
        {
            delete (it->second.call);    
        }
        m_callMap.clear();
    }
    SILENT_RM_CATCH_HANDLER("~RadioCurrentCallListCtrl");

	FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(RadioCurrentCallListCtrl, CTreeListCtrl)
	//{{AFX_MSG_MAP(RadioCurrentCallListCtrl)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(CALL_NOTIFICATION_MSG, onCallNotification)
    ON_MESSAGE(PTT_NOTIFICATION_MSG, onPttNotification)
    ON_MESSAGE(INIT_NOTIFICATION_MSG, onInitNotification)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioCallStackListCtrl message handlers

void RadioCurrentCallListCtrl::setRadioName(std::string radioName)
{
 	FUNCTION_ENTRY("setRadioName");
	m_radio.setEntityName( radioName,false );

    try
    {
	    // Determine entity key for the local radio agent
	    // This is used to filter the incoming notifications from the radio agents
	    TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(radioName);
	    TA_ASSERT(entity!=NULL,"Radio entity could not be found.");
	    m_radioEntityKey = entity->getKey();
        m_radioEntityLocation = entity->getLocation();
	    delete entity;
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::setRadioName")

	FUNCTION_EXIT;
}

void RadioCurrentCallListCtrl::setSessionRef(long sessionRef)
{
 	FUNCTION_ENTRY("setSessionRef");
	m_sessionRef = sessionRef;
	FUNCTION_EXIT;
}


BOOL RadioCurrentCallListCtrl::setColumnName(int nCol, char*  columnName)
{
 	FUNCTION_ENTRY("setColumnName");

	LVCOLUMN  column;

	column.mask = LVCF_TEXT|LVCF_SUBITEM;
	column.iSubItem = nCol;
	column.pszText = columnName;

	FUNCTION_EXIT;
	return SetColumn(nCol,&column);
}


long RadioCurrentCallListCtrl::getSelectedCallID()
{
 	FUNCTION_ENTRY("getSelectedCallID");

	POSITION SelPos = GetFirstSelectedItemPosition();
	ASSERT (SelPos!=NULL);

	int nItem = GetNextSelectedItem(SelPos);
    
    int vIndex = GetVirtualIndexFromActual(nItem);

	FUNCTION_EXIT;
	return GetVItemData(vIndex);	
}


RadioCurrentCallListCtrl::SelectedCalls RadioCurrentCallListCtrl::getSelectedCallIDs()
{    
 	FUNCTION_ENTRY("getSelectedCallIDs");
    
    SelectedCalls result;
    std::vector<unsigned int> selection = TA_Base_Bus::SelectionHelper::getSelectedItemIndices(*this);
    for (int i = 0; i < (int)selection.size(); ++i )
    {
        int vIndex = GetVirtualIndexFromActual(selection[i]);
        result.insert(GetVItemData(vIndex));
    }

	FUNCTION_EXIT;
	return result;	
}


bool RadioCurrentCallListCtrl::isSelectedCallHalfDuplex()
{
 	FUNCTION_ENTRY("isSelectedCallHalfDuplex");

	long callID = getSelectedCallID();

    FUNCTION_EXIT;
    return isCallHalfDuplex(callID);
}


void RadioCurrentCallListCtrl::queueSubscriptions()
{
    // post this window a message
    // it needs to initialise its call list after all the other crap is done
	//PostMessage(INIT_NOTIFICATION_MSG,0,0);  PostMessage in the same thread will cause the message lost 
	//TD 19479
	SendMessage(INIT_NOTIFICATION_MSG,0,0);
}


bool RadioCurrentCallListCtrl::isCallHalfDuplex(long callID)
{
	CallMap::iterator itr = m_callMap.find(callID);
    TA_ASSERT(itr != m_callMap.end(), "Call details not defined");
    
	bool isHalfDuplex = (itr->second.call->callCategory == TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex);	
	return isHalfDuplex;
}


afx_msg LRESULT RadioCurrentCallListCtrl::onCallNotification(WPARAM wParam, LPARAM lParam)
{
 	FUNCTION_ENTRY("onCallNotification");

    try
    {
	    TA_Base_Bus::IRadioCorbaDef::CallStatusEvent*  callStatusEvent = reinterpret_cast<TA_Base_Bus::IRadioCorbaDef::CallStatusEvent*>(lParam);
	    TA_ASSERT(callStatusEvent!=NULL,"No call event specified on call.");
	    processUpdate(*callStatusEvent);
	    delete callStatusEvent;
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::onCallNotification");

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT RadioCurrentCallListCtrl::onPttNotification(WPARAM wParam, LPARAM lParam)
{
 	FUNCTION_ENTRY("onPttNotification");

	TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification*  pttEvent = reinterpret_cast<TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification*>(lParam);
	TA_ASSERT(pttEvent != NULL, "No call event specified on call.");

    try
    {
	    processUpdate(*pttEvent);
        delete pttEvent;
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::onPttNotification");

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT RadioCurrentCallListCtrl::onInitNotification(WPARAM wParam, LPARAM lParam)
{
    startSubscription();
    return 0;
}


void RadioCurrentCallListCtrl::processUpdate(const TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification& pttStatus)
{
    // PTT changes result in an update only of the list control (never items being inserted / removed)
    CallMap::iterator itr = m_callMap.find(pttStatus.callID);

    if (itr == m_callMap.end())
    {
        // unrecognized, may ignore
        return;
    }
    else
    {        
        // Otherwise update the talking TSI parameter
        itr->second.talkingTSI = pttStatus.tpiITSI.in();

        // Refresh relevant rows (must refresh virtual items, in case expanded later!)
        for (int vIndex = 0; vIndex < GetVItemCount(); ++ vIndex)
        {
            if (pttStatus.callID == (long) GetVItemData(vIndex))
            {   
                // Check if child or parent node
                int level = getVItemLevel(vIndex);

                TA_ASSERT(0 == level, "Unexpected internal condition");

                refreshConferenceParentRow(vIndex, itr->second);
                
                // Refresh participants, which should follow immediately after
                TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetails = *itr->second.call;

                for (int childIndex = 0; childIndex < (int) callDetails.participants.length(); ++childIndex  )
                {
                    TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant = callDetails.participants[childIndex];        

                    if (strcmp(participant.TSI.in(), pttStatus.tpiITSI.in()) == 0)
                    {
                        // Refresh the PTT state internally before updating row
                        participant.pttState = pttStatus.state;
                        // If we're receiving PTT notifications for this party, must have acknowledged
                        participant.isAcknowledged = true;                       
                    }

                    // Children will immediately follow the parent, and added in-order (no sub-level sorting)
                    int childVIndex = vIndex + childIndex + 1;

                    refreshChildRow(childVIndex, participant, itr->second);
                }

                // Call ID's are unique in list
                break;
            }
        }
    }
}


void RadioCurrentCallListCtrl::processUpdate(const TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatusEvent)
{
 	FUNCTION_ENTRY("processUpdate");

	// Check if call belongs to this call stack
	if (callStatusEvent.sessionRef == m_sessionRef)
	{
		// Add, Update or remove the call
		if (callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Connected)
		{
            if (m_callMap.end() != m_callMap.find(callStatusEvent.callID))
            {
                // Call already exists in list control
                updateCall(callStatusEvent.callID);
            }
            else if (!callStatusEvent.isInvisibleToGUIApps)
            {
			    addCall(callStatusEvent.callID);
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"addCall::PostMessage(ADD_CALL_TO_HISTORY, callID, 0)");
				AfxGetApp()->GetMainWnd()->PostMessage(ADD_CALL_TO_HISTORY, callStatusEvent.callID, 0);
            }
		}
		else
		{
			removeCall(callStatusEvent.callID);
		}
	}

	FUNCTION_EXIT;
}

bool RadioCurrentCallListCtrl::loadCalls()
{
 	FUNCTION_ENTRY("loadCalls");

	// Check if already loaded, no need to reload
	if (m_isLoaded)
	{
		return true;
	}

	try
	{
		// Clear current list
		if(this->m_hWnd != NULL)
		{
			DeleteAllItems();
		}

		// Get the list of calls
		TA_Base_Bus::IRadioCorbaDef::CallList* callList = 0;
		CORBA_CALL_RETURN( callList,
		                   m_radio,
		                   getCallList,
		                   ( ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );

		// Add the calls
		for (unsigned int iCall=0;iCall<callList->length();iCall++)
		{
			processUpdate((*callList)[iCall]);
		}

		delete callList;
		m_isLoaded = true;

        onListRefreshed();
	}
	catch(...)
	{
		int vPosition = AddItem("",0,0);
		SetVItemText(vPosition, COL_CALL, "Unable to get the call list from Radio Agent.");
	}

	FUNCTION_EXIT;
	return m_isLoaded;
}


void RadioCurrentCallListCtrl::addCall(long callID, bool isNew, std::string talkingTSI)
{
 	FUNCTION_ENTRY("addCall");

    try
    {    
        if (m_callMap.end() != m_callMap.find(callID))
        {
            // This shouldn't happen, same call being added twice
            TA_THROW(TA_Base_Core::TransactiveException("Adding the same call twice!"));
        }

		// Get the call details
		TA_Base_Bus::IRadioCorbaDef::CallDetailsType* callDetails = 0;
        
#ifdef RUN_IN_DEBUG_MODE
        callDetails = TA_IRS_App::RadioManager::RadioManagerTestData::getInstance().getCopyCurrentCallDetails(callID);
#else
		CORBA_CALL_RETURN( callDetails,
		                   m_radio,
		                   getCallDetails,
		                   ( callID, ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );
#endif

        if (0 == callDetails)
        {
            TA_THROW(TA_Base_Core::TransactiveException("Could not retrieve call details!"));
        }
	    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "hongzhi, callDetails: calledLocation=%s,callerLocation=%s",callDetails->calledLocation,callDetails->callerLocation);
        
        //TD 10234
        if (true == callDetails->isInvisibleToGUIApps)
        {
            FUNCTION_EXIT;
            return;
        }

        // Store in map for later usage
        m_callMap[callID] = CallData(callDetails, talkingTSI);
        
        // replace any (F) in fallback for trains.
        // yet another dodgy thing to please LTA
        static const std::string fallbackStr("(F)");
        if ( (fallbackStr.compare(callDetails->calledName.in()) == 0) ||
             (fallbackStr.compare(callDetails->calledLocation.in()) == 0) )
        {
            unsigned char trainId = 
            TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getTrainIdForTsi(callDetails->calledTSI.in());
            if ( trainId != 0 )
            {
                TA_IRS_Bus::CSearchPage::TrainUpdate trainDetails = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getDetailsForTrain(trainId);
                callDetails->calledName = CORBA::string_dup(trainDetails.name.GetBuffer(0));
                CString locationName = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getLocationNameFromKey(trainDetails.locationKey);
                callDetails->calledLocation = CORBA::string_dup(locationName.GetBuffer(0));
            }
        }
        

        performListRefresh();

        if (isNew)
        {
            // Automatically select all incoming calls
            for (int i = 0; i < GetVItemCount(); ++i )
            {
				//TD16959++: deselected the previous group call when new group call is made.
				SetItemState(i, 0, LVIS_SELECTED);

                if (0 == getVItemLevel(i) && callID == (long) GetVItemData(i))
                {
                    int actualIndex = GetActualIndexFromVirtual(i);
                    
                    // This is the new item
                    SetItemState(actualIndex, LVIS_SELECTED, LVIS_SELECTED);
                }
				//++TD16959
            }
        }
	}
	catch (...)
	{
		// Error getting call details
		int virtualIndex = AddItem("",0,0);
		SetVItemData(virtualIndex, callID);
		SetVItemText(virtualIndex, COL_CALL, "Unable to get the call details from Radio Agent.");
	}

    FUNCTION_EXIT;
}
 
 
void RadioCurrentCallListCtrl::performListRefresh()
{       
    FUNCTION_ENTRY("performListRefresh");

    // Save current states prior to refresh
    std::vector<DWORD> currentlyExpandedCallIDs = fetchExpandedNodeItemData();
    // Save selected call IDs
    RadioCurrentCallListCtrl::SelectedCalls selectedCallIDs = getSelectedCallIDs();

    int vScrollPos = GetScrollPos(SB_VERT);
    
    TA_Base_Bus::LockControlRedraw lockRedraw(*this);

    // Clear the list before adding in
	if(this->m_hWnd != NULL)
    {
		DeleteAllItems();
	}

    std::multimap<long, CallData> orderedCalls;
    // Create a map of calls with time as the key index, to give
    // us a sort based on increasing time (older calls at bottom of list)
    for (CallMap::iterator cmIt = m_callMap.begin(); cmIt != m_callMap.end(); ++cmIt )
    {
        orderedCalls.insert(std::multimap<long, CallData>::value_type(cmIt->second.call->time, cmIt->second) );
    }
    
    TA_ASSERT(orderedCalls.size() == m_callMap.size(), "Internal ordering failure!");

    // time stored in map in ascending order, want to add oldest time last, so traverse in reverse
    // (smallest time value represents oldest call time)
    for (std::multimap<long, CallData>::reverse_iterator it = orderedCalls.rbegin(); it != orderedCalls.rend(); ++it )
    {
        // Add the call to the end of the control
        int parentVIndex = addCallToControl(it->second);
        
        DWORD callID = (DWORD) it->second.call->callID;

        if (currentlyExpandedCallIDs.end() != 
            std::find(currentlyExpandedCallIDs.begin(), currentlyExpandedCallIDs.end(), callID))
        {
            // This is an expanded node, OK to expand it at this stage..
            int actualIndex = GetActualIndexFromVirtual(parentVIndex);

            if (actualIndex < 0)
            {
                LOG( SourceInfo, 
                    TA_Base_Core::DebugUtil::GenericLog, 
                    TA_Base_Core::DebugUtil::DebugError, 
                    "RadioCurrentCallListCtrl::performListRefresh: GetActualIndexFromVirtual");   
            }

            Expand(actualIndex);            
        }

//TD16959++: comment this
//        // Check if any newly added items should be selected
//        if (selectedCallIDs.end() != selectedCallIDs.find(callID))
//        {
//            int actualIndex = GetActualIndexFromVirtual(parentVIndex);
//
//            if (actualIndex < 0)
//            {
//                LOG( SourceInfo, 
//                    TA_Base_Core::DebugUtil::GenericLog, 
//                    TA_Base_Core::DebugUtil::DebugError, 
//                    "RadioCurrentCallListCtrl::performListRefresh: GetActualIndexFromVirtual");   
//            }
//
//            // This call was selected previously, re-select it
//            SetItemState(actualIndex, LVIS_SELECTED, LVIS_SELECTED);
//        }
//++TD16959
    }

    ////////////////
    // Restore scroll position
    // (this code taken from ListeningCallStackListCtrl)
    int vScrollMax = GetScrollLimit(SB_VERT);                
    if (GetItemCount() > 0 && vScrollMax > 0)
    {                   
        // To make scrolling work, use EnsureVisible
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

    onListRefreshed();

    SetRedraw(TRUE);
    
    FUNCTION_EXIT
}

void RadioCurrentCallListCtrl::refreshChildRow(
    int childVIndex, 
    const TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant, 
    const CallData& callInformation)     
{   
    SetVItemText(childVIndex, COL_CALL, participant.name.in());
    SetVItemText(childVIndex, COL_NUMBER, participant.TSI.in());
    SetVItemText(childVIndex, COL_LOCATION, participant.location.in());
    
    if (!participant.isAcknowledged)
    {
        SetVItemText(childVIndex, COL_STATUS, "Not Connected");
    }
    else if (TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED == participant.pttState)
    {
        SetVItemText(childVIndex, COL_STATUS, "Talking");
    }
    else
    {
        SetVItemText(childVIndex, COL_STATUS, "Listening");
    }
}


void RadioCurrentCallListCtrl::refreshConferenceParentRow(int vIndex, const CallData& callInformation)
{
	// TD15912
    //SetVItemText(vIndex, COL_CALL, "CONFERENCE");
	CString callType;
	switch(callInformation.call->callType)
	{
		case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupAcknowledgeCall:
			//TD17301-cw
			//callType = "Acknowledged";//cl-17301-lkm
			callType = "Acknowledge";
			//TD17301
			break;
		case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupUnacknowledgeCall:
			//TD17301-cw
			//callType = "Unacknowledged";//cl-17301-lkm
			callType = "Unacknowledge";
			//TD17301
			break;
		case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupBroadcastCall:
			callType = "Broadcast";
			break;
		default:
			break;
	}
	SetVItemText(vIndex, COL_CALL, callType);
	// TD15912

    std::ostringstream ostr;

    // We only display who is currently talking if the call is connected
    // If don't know who is talking, leave blank
    if (TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Connected == callInformation.call->callStatus &&
         callInformation.talkingTSI.length() > 0)
    {
        std::string callerName("");

        // Find the name of the talking party
        for (int i = 0; i < (int) callInformation.call->participants.length(); ++i )
        {       
            TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant = callInformation.call->participants[i];

            if (strcmp(participant.TSI.in(), callInformation.talkingTSI.c_str()) == 0)
            {
                // Found the name
                callerName = participant.name.in();
                break;
            }
        }

        // 'name(tsi) talking'
        ostr << callerName << " (" << callInformation.talkingTSI << ")";
    }


    if (ostr.str().length() > 0)
    {
        // Displaying text indicating that someone is talking
        SetVItemText(vIndex, COL_STATUS, "Talking");
    }
    else
    {
        // Use call status in status field text if no-one talking (in a connected call)
        SetVItemText(vIndex, COL_STATUS, getStatusText(callInformation.call->callStatus) );
    }

    SetVItemText(vIndex, COL_NUMBER, ostr.str().c_str());

	
	

	// For group calls need to display the TSI and Location of the group if it is not an Acknowledged call
	// For acknowledged calls, the current talking subscriber's details will be displayed, not the group's details
	switch(callInformation.call->callType)
	{
		case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupUnacknowledgeCall:
		case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupBroadcastCall:
			{
				if( TA_Base_Bus::IRadioCorbaDef::ECallSource::OutgoingCall == callInformation.call->callSource) 
				{
  					SetVItemText(vIndex, COL_LOCATION, callInformation.call->calledLocation);
					SetVItemText(vIndex, COL_NUMBER, callInformation.call->calledTSI);
				}
				else if( TA_Base_Bus::IRadioCorbaDef::ECallSource::IncomingCall == callInformation.call->callSource) 				{
					SetVItemText(vIndex, COL_LOCATION, callInformation.call->callerLocation);
					SetVItemText(vIndex, COL_NUMBER, callInformation.call->callerTSI);
				}
			}
			break;

		default:
			break;
	}
}


int RadioCurrentCallListCtrl::addCallToControl(const CallData& callInformation)
{    
    FUNCTION_ENTRY("addCallToControl");
    
    TA_ASSERT(0 != callInformation.call, "Call details NULL");

    const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetails = *callInformation.call;

	try
	{
        int     imageNumber;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"hongzhi1: Call Details: callID:%d, session_Ref:%d, consolekey:%d, time:%d, callerName:%s, callerTSI:%s, callerLocation:%s, calledName:%s, calledTSI:%s, calledLocation:%s, callStatus:%d, callCategory:%d, callSource:%d, callType:%d, isEmergency:%d, isInvisibleToGUIApps:%d",
            callDetails.callID,callDetails.sessionRef,callDetails.consoleKey,callDetails.time,callDetails.callerName,callDetails.callerTSI,callDetails.callerLocation,callDetails.calledName,callDetails.calledTSI,callDetails.calledLocation,callDetails.callStatus,callDetails.callCategory,callDetails.callSource,callDetails.callType,callDetails.isEmergency,callDetails.isInvisibleToGUIApps);

		switch (callDetails.callCategory)
		{
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::TextMessage:imageNumber = 0;
																	break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::CallDriver:	imageNumber = 1;
																	break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::FullDuplex:	imageNumber = 2;
																	break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex:	imageNumber = 3;
																	break;
			default:
				imageNumber = -1;
		}

		imageNumber = -1;  // switch off for demo

        int parentVIndex;

		if (imageNumber==-1)
		{
			parentVIndex = AddItem(callDetails.callTime,0,0);
		}
		else
		{
			parentVIndex = AddItem(callDetails.callTime,imageNumber,0);
		}

		SetVItemData(parentVIndex, callDetails.callID);

		if (callDetails.callCategory==TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex)
		{
			// Add to the list control
            refreshConferenceParentRow(parentVIndex, callInformation);
			// TD19462 by hongzhi, Site: Call details did not show in current call stack for single half-duplex call.
			if (callDetails.callSource == TA_Base_Bus::IRadioCorbaDef::ECallSource::IncomingCall)
			{
				if(callDetails.callType == TA_Base_Bus::IRadioCorbaDef::ECallType::SingleCall)
				{
					SetVItemText(parentVIndex, COL_CALL, callDetails.callerName);
					SetVItemText(parentVIndex, COL_NUMBER, callDetails.callerTSI);
					SetVItemText(parentVIndex, COL_LOCATION, callDetails.callerLocation);
				}
			}
			else
			{
				if(callDetails.callType == TA_Base_Bus::IRadioCorbaDef::ECallType::SingleCall)
				{
					SetVItemText(parentVIndex, COL_CALL, callDetails.calledName);
					SetVItemText(parentVIndex, COL_NUMBER, callDetails.calledTSI);
					SetVItemText(parentVIndex, COL_LOCATION, callDetails.calledLocation);
				}
			}
		}
		else
		{
			// Add to the list control
			if (callDetails.callSource == TA_Base_Bus::IRadioCorbaDef::ECallSource::IncomingCall)
			{
				SetVItemText(parentVIndex, COL_CALL, callDetails.callerName);
				SetVItemText(parentVIndex, COL_NUMBER, callDetails.callerTSI);
				SetVItemText(parentVIndex, COL_LOCATION, callDetails.callerLocation);
			}
			else
			{
				SetVItemText(parentVIndex, COL_CALL, callDetails.calledName);
				SetVItemText(parentVIndex, COL_NUMBER, callDetails.calledTSI);
				SetVItemText(parentVIndex, COL_LOCATION, callDetails.calledLocation);
			}
			SetVItemText(parentVIndex, COL_STATUS, getStatusText(callDetails.callStatus));
		}

		// If a group call then load the associated subscribers
		if (TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex == callDetails.callCategory)
		{
			// Request group members be obtained            
            for (int i = 0; i < (int) callDetails.participants.length(); ++i )
            {
                const TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant = callDetails.participants[i];
                
                // No time displayed for children
                int childVIndex = AddItem("", 0, 1);
                // Reference parent call item
                SetVItemData(childVIndex, callDetails.callID);

                refreshChildRow(childVIndex, participant, callInformation);
            }            
		}

        return parentVIndex;
    }
    catch (...)
    {
		// Error placing into list control
		int parentVIndex = AddItem("", 0, 0);
		SetVItemData(parentVIndex, callDetails.callID);
		SetVItemText(parentVIndex, 4, "Unable to insert item into list.");     

        return parentVIndex;   
    }

	FUNCTION_EXIT;
}


void RadioCurrentCallListCtrl::addCallGroupMembers(long callID)
{

}

void RadioCurrentCallListCtrl::removeCall(long callID)
{
 	FUNCTION_ENTRY("removeCall");

	// Find the call in the stack
	CallMap::iterator itr = m_callMap.find(callID);

    if (m_callMap.end() == itr)
    {
        // Call not in list, ignore update request
        return;
    }

    try
    {    
        // Reclaim memory
        delete itr->second.call;
        m_callMap.erase(itr);
    
        // Refresh control
        performListRefresh();
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::removeCall");

	FUNCTION_EXIT;

}

void RadioCurrentCallListCtrl::updateCall(long callID)
{
 	FUNCTION_ENTRY("updateCall");

	// Find the call in the stack
	CallMap::iterator itr = m_callMap.find(callID);

    if (m_callMap.end() == itr)
    {
        // Call not in list, ignore update request
        return;
    }

    // Remember who was talking
    std::string talkingTSI = itr->second.talkingTSI;

    try
    {
        // Refresh the call details (delete old call to reclaim memory)
        delete itr->second.call;
        m_callMap.erase(itr);
    
        // Can treat update as adding a new call
        addCall(callID, false, talkingTSI);
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::updateCall");

	FUNCTION_EXIT;
}


void RadioCurrentCallListCtrl::PreSubclassWindow() 
{
 	FUNCTION_ENTRY("PreSubclassWindow");

	CTreeListCtrl::PreSubclassWindow();
    setupListControl();

	FUNCTION_EXIT;
}


void RadioCurrentCallListCtrl::setupListControl()
{
 	FUNCTION_ENTRY("setupListControl");

	// Create the required columns
	for (int i = 0; i < COL_INDEX_MAX; ++i )
    {
        InsertColumn(i, "");
    }

	// Attach the image list
    //SetImageList(&m_ImageList,LVSIL_SMALL);

	FUNCTION_EXIT;
}

void RadioCurrentCallListCtrl::startSubscription()
{
 	FUNCTION_ENTRY("startSubscription");
    
    try
    {
    	if (!m_bSubscribed)
    	{
    		// Hold processing of subscriptions
    		TA_Base_Core::ThreadGuard guard(m_updateLock);
    
    		// Start subscription to changes (call and PTT)
    		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::RadioCallStatus, this, m_radioEntityKey, 0, m_radioEntityLocation, NULL );
    		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::RadioPTTNotification, this, m_radioEntityKey, 0, m_radioEntityLocation, NULL );
    		m_bSubscribed = true;
    
    		// Load all call details
    		m_isLoaded = false;  // Ensure list is reloaded
    		loadCalls();
    	}
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::startSubscription");

	FUNCTION_EXIT;
}

void RadioCurrentCallListCtrl::stopSubscription()
{
 	FUNCTION_ENTRY("stopSubscription");

    try
    {
	    if (m_bSubscribed)
	    {
    		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		    m_bSubscribed = false;
	    }
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::stopSubscription");

	FUNCTION_EXIT;
}

void RadioCurrentCallListCtrl::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {

    	// Wait until list has been loaded
    	TA_Base_Core::ThreadGuard guard(m_updateLock);
    
    	// Don't process if window is not active
        if (!m_bSubscribed) return;
    
    	if (loadCalls())
    	{
    		// The data structure you want
    		TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatus = NULL;
            TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification* pttNotificationStatus = 0;
    
    		std::string messageTypeKey(message.messageTypeKey);
    
    		// Extract to the messageState
    		if ( ( message.messageState >>= callStatus ) != 0)
    		{
    			// Copy Message as processing will be done after function returns
    			TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusEvent = new TA_Base_Bus::IRadioCorbaDef::CallStatusEvent(*callStatus);
    
    			// Post message for processing by main GUI thread
    			PostMessage(CALL_NOTIFICATION_MSG,0, (LPARAM)callStatusEvent);
    		}
            else if ( (message.messageState >>= pttNotificationStatus) != 0)
            {
                TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification* pttStatus = new TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification(*pttNotificationStatus);
    
                PostMessage(PTT_NOTIFICATION_MSG, 0, (LPARAM)pttStatus);
            }
    		else
    		{
    			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Event does not contain dataPointEvent");
    		}
    	}
    }
    SILENT_RM_CATCH_HANDLER("RadioCurrentCallListCtrl::receiveSpecialisedMessage");

	FUNCTION_EXIT;
}



const char* RadioCurrentCallListCtrl::getStatusText(TA_Base_Bus::IRadioCorbaDef::ECallStatusType status)
{
 	FUNCTION_ENTRY("getStatusText");

	switch (status)
	{
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing:      return "Ringing";
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Connected:    return "Connected";
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Disconnected: return "Disconnected";
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::OnHold:       return "On hold";
		default:
			return "Unknown";
	}

	FUNCTION_EXIT;
}


void RadioCurrentCallListCtrl::OnDestroy()
{
    // Cancel subscription prior to destroying window
	stopSubscription();
}


DWORD RadioCurrentCallListCtrl::getForegroundColourForItem(int nItem)
{
    int vIndex = GetVirtualIndexFromActual(nItem);

    if (vIndex >= 0)
    {
        long callID = (long)GetVItemData(vIndex);

        CallMap::iterator itr = m_callMap.find(callID);
        TA_ASSERT(itr != m_callMap.end(), "Call details not defined");

        // TES #494, emergency calls displayed in red
        if (itr->second.call->isEmergency)
        {
            return RGB(255, 0, 0);
        }
    }
    
    return CTreeListCtrl::getForegroundColourForItem(nItem);    
}
