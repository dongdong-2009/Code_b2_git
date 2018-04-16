/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioCallStackLibrary/src/RadioCallStackListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class provides a call list which is self managing.  It will automatically
  * update with call notifications from the radio.
  *
  */
// RadioCallStackListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RadioCallStackListCtrl.h"
#include "..\res\LibraryResource.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "core\message\src\MessageSubscriptionManager.h"
#include "core\synchronisation\src\ThreadGuard.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "bus/radio/RadioEntityAccess/src/RadioEntityAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;

const int CALL_NOTIFICATION_MSG = WM_USER + 100;
const DWORD EMERGENCY_TEXT_COLOUR = RGB(255, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CRadioCallStackListCtrl

CRadioCallStackListCtrl::CRadioCallStackListCtrl(EStackType stackType) 
                        : m_stackType(stackType), 
						  m_consoleId(0),
						  m_bSubscribed(false),
						  m_radioEntityKey(0),
                          m_radioEntityLocation(0),
						  m_isLoaded(false),
						  m_canCallTrain(false)

{
 	FUNCTION_ENTRY("Constructor");

	m_consoleId = RadioEntityAccess::getConsoleKey();
	
	HICON hIcon;

	//setRadioName("Radio_OCC");

	// Setup image list
	m_ImageList.Create(16,16,ILC_COLOR8,4,4);
	hIcon = AfxGetApp()->LoadIcon(IDI_CALL_SDS);
    m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_EMERG_CALL_SDS);
    m_ImageList.Add(hIcon);

	hIcon = AfxGetApp()->LoadIcon(IDI_CALL_DRIVER);
    m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_EMERG_CALL_DRIVER);
    m_ImageList.Add(hIcon);

	hIcon = AfxGetApp()->LoadIcon(IDI_CALL_VOICE);
    m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_EMERG_CALL_VOICE);
    m_ImageList.Add(hIcon);

	hIcon = AfxGetApp()->LoadIcon(IDI_CALL_CONFERENCE);
    m_ImageList.Add(hIcon);
	hIcon = AfxGetApp()->LoadIcon(IDI_EMERG_CALL_CONFERENCE);
    m_ImageList.Add(hIcon);

	FUNCTION_EXIT;
}

CRadioCallStackListCtrl::~CRadioCallStackListCtrl()
{
 	FUNCTION_ENTRY("Destructor");
	stopSubscription();
	FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CRadioCallStackListCtrl, ListCtrlSelNoFocus)
	//{{AFX_MSG_MAP(CRadioCallStackListCtrl)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CALL_NOTIFICATION_MSG, onCallNotification)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioCallStackListCtrl message handlers

void CRadioCallStackListCtrl::setRadioName(std::string radioName)
{
 	FUNCTION_ENTRY("setRadioName");

    try
    {
    	// Set the remote name for the radio object
    	m_radio.setEntityName( radioName,false );
    
    	// Determine entity key for the local radio agent
    	// This is used to filter the incoming notifications from the radio agents
        m_radioEntityLocation = RadioEntityAccess::getLocationKey(m_consoleId);
		m_radioEntityKey = RadioEntityAccess::getRadioEntityKey(m_radioEntityLocation);
    
    	// Restart the subscription and reload the list
    	if (!m_bSubscribed)
    	{
			startSubscription();
    	}
	}
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::setRadioName");
    }

	FUNCTION_EXIT;
}


void CRadioCallStackListCtrl::setCallTrainPermitted(bool canCallTrain)
{
 	FUNCTION_ENTRY("setCallTrainPermitted");
	if (m_canCallTrain != canCallTrain)
	{
		m_canCallTrain = canCallTrain;
		m_isLoaded	= false;
		loadCalls();
	}
	FUNCTION_EXIT;
}

BOOL CRadioCallStackListCtrl::setColumnName(int nCol, char*  columnName)
{
 	FUNCTION_ENTRY("setColumnName");

	LVCOLUMN  column;

	column.mask = LVCF_TEXT|LVCF_SUBITEM;
	column.iSubItem = nCol;
	column.pszText = columnName;

	FUNCTION_EXIT;
	return SetColumn(nCol,&column);
}


long CRadioCallStackListCtrl::getSelectedCallID()
{
 	FUNCTION_ENTRY("getSelectedCallID");

	POSITION SelPos = GetFirstSelectedItemPosition();
	ASSERT (SelPos!=NULL);

	int nCol = GetNextSelectedItem(SelPos);

    ASSERT (nCol >= 0);

	FUNCTION_EXIT;
	return GetItemData(nCol);	
}



void CRadioCallStackListCtrl::getSelectedCallList(std::vector<long>& callList)
{
 	FUNCTION_ENTRY("getSelectedCallList");

	callList.clear();

	POSITION SelPos = GetFirstSelectedItemPosition();

	while (SelPos!=NULL)
	{
		int nCol = GetNextSelectedItem(SelPos);
		callList.push_back(GetItemData(nCol));
	}

	FUNCTION_EXIT;
}

bool CRadioCallStackListCtrl::IsCallSelected()
{
 	FUNCTION_ENTRY("IsCallSelected");

	POSITION SelPos = GetFirstSelectedItemPosition();

	FUNCTION_EXIT;
	return (SelPos!=NULL);	
}


bool CRadioCallStackListCtrl::selectCall(long callId)
{
    FUNCTION_ENTRY("selectCall");

    // clear any previous selection
    deselectAll();

	// Find the call in the stack
	int iPos=0;
	while ( iPos < GetItemCount() )
	{
		long selectedCallID;

		// Get the stored reference
		selectedCallID = GetItemData(iPos);

		// Is it the one we want?
        if ( selectedCallID == callId )
		{
		    // select it
            SetItemState(iPos, LVIS_SELECTED, LVIS_SELECTED);

            // done
            FUNCTION_EXIT;
			return true;
        }

		iPos++;
	}

	FUNCTION_EXIT;
    return false;
}


void CRadioCallStackListCtrl::deselectAll()
{
    UINT uSelectedCount = GetSelectedCount();
    UINT i;
    int  nItem = -1;
    
    if (uSelectedCount > 0)
    {
        // Remove all selection masks
        for (i = 0; i < uSelectedCount;i++)
        {
            nItem = GetNextItem(nItem, LVNI_SELECTED);
            
            TA_ASSERT(nItem != -1, "Operation failed");
                        
            SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}


void CRadioCallStackListCtrl::processUpdate(TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatusEvent)
{
 	FUNCTION_ENTRY("processUpdate");

	bool isCallTrainRequest   = (callStatusEvent.callCategory==TA_Base_Bus::IRadioCorbaDef::ECallCategory::CallDriver);

    //
    // TD9386 PISF/TST/350 WorkPackage 63.0027
    // If the notification is a SessionUpdate, need to reestablish
    // the session key, which has been put in the callID field
/*  Deleted because of multiple tcp server support, radio session not necessary
    if ((callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::SessionUpdate)
            && (callStatusEvent.sessionRef==m_sessionRef))
    {
        m_sessionRef = callStatusEvent.callID;
        // update considered consumed right here
        FUNCTION_EXIT;
        return;
    }
*/
	if (m_stackType == CRadioCallStackListCtrl::EStackType::OthersStack)
	{
		// Check if call belongs to this call stack (Not our call or a train call we cannot handle)
		if ((!isCallTrainRequest&&(callStatusEvent.consoleKey!=m_consoleId))
			||(isCallTrainRequest&&!m_canCallTrain))
		{
			// Add, Update or remove the call
			if (callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing)
			{
				addCall(callStatusEvent.callID);
			}
			else if (callStatusEvent.callStatus!=TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Deleted)
			{
				updateCall(callStatusEvent.callID,callStatusEvent.callStatus,callStatusEvent.callCategory);
			}
			else
			{
				removeCall(callStatusEvent.callID);
			}
		}
	}
	else if (m_stackType == CRadioCallStackListCtrl::EStackType::IncomingStack)
	{
		// Check if call belongs to this call stack
		if ((callStatusEvent.consoleKey==m_consoleId) || (isCallTrainRequest&&m_canCallTrain))
		{					
			if (callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing)
			{
				addCall(callStatusEvent.callID);
			}
			else
			{
				// If its there remove it
				removeCall(callStatusEvent.callID);
			}
		}
		else if ((callStatusEvent.consoleKey==m_consoleId) && (callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Deleted)) // TD13059
		{
			removeCall(callStatusEvent.callID);
		}
	}
	else if (m_stackType == CRadioCallStackListCtrl::EStackType::HeldStack)
	{
		// Check if call belongs to this call stack
		if ((callStatusEvent.consoleKey==m_consoleId)&&(!isCallTrainRequest))
		{
			if (callStatusEvent.callStatus==TA_Base_Bus::IRadioCorbaDef::ECallStatusType::OnHold)
			{
				addCall(callStatusEvent.callID);
			}
			else
			{
				removeCall(callStatusEvent.callID);
			}
		}
	}

	FUNCTION_EXIT;
}

bool CRadioCallStackListCtrl::loadCalls()
{
 	FUNCTION_ENTRY("loadCalls");

	// Check if already loaded, no need to reload
	if (m_isLoaded)
	{
		return true;
	}

	// Clear current list - might have error message in it
	if(this->m_hWnd!=NULL)
	{
		DeleteAllItems();
	}

	// Get the list of calls
	try
	{
		TA_Base_Bus::IRadioCorbaDef::CallList* callList = 0;
		CORBA_CALL_RETURN( callList,
		                   m_radio,
		                   getCallList,
		                   ( m_consoleId ) );

		// Add the calls
		for (unsigned int iCall=0;iCall<callList->length();iCall++)
		{
			processUpdate((*callList)[iCall]);
		}

		delete callList;
		m_isLoaded = true;
	}
	catch (...)
	{
		InsertItem(0,"Unable to get the call list from Radio Agent.");
	}

	FUNCTION_EXIT;
	return m_isLoaded;
}


void CRadioCallStackListCtrl::addCall(long callID)
{
 	FUNCTION_ENTRY("addCall");

	TA_Base_Bus::IRadioCorbaDef::CallDetailsType*   callDetails;
	int		iPosition;
	int     imageNumber;


	// Get the call details
	try
	{
		CORBA_CALL_RETURN( callDetails,
		                   m_radio,
		                   getCallDetails,
		                   ( callID, m_consoleId ) );

		switch (callDetails->callCategory)
		{
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::TextMessage:
                imageNumber = 0;
				break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::CallDriver:	
                imageNumber = 2;
				break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::FullDuplex:
            	imageNumber = 4;
				break;
			case TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex:	
                imageNumber = 6;
				break;
			default:
				imageNumber = -1;
		}

        if (imageNumber >= 0 && callDetails->isEmergency)
        {
            ++imageNumber;
        }

		if (imageNumber==-1)
		{
			iPosition = InsertItem(0,callDetails->callTime);
		}
		else
		{
			iPosition = InsertItem(0,callDetails->callTime,imageNumber);
		}

		SetItemData(iPosition,callDetails->callID);

		CString callStatusStr = getStatusText(callDetails->callStatus, callDetails->callCategory);
		if (m_stackType == CRadioCallStackListCtrl::EStackType::OthersStack)
		{
			// Add to the list control
			SetItemText(iPosition,1,callDetails->callerName);
			SetItemText(iPosition,2,callDetails->callerLocation);  
			SetItemText(iPosition,3,callDetails->calledName);
			SetItemText(iPosition,4,callDetails->calledLocation);
			SetItemText(iPosition,5,callStatusStr);
		}
		else
		{
			// Add to the list control
			SetItemText(iPosition,1,callDetails->callerName);
			SetItemText(iPosition,2,callDetails->callerTSI);
			SetItemText(iPosition,3,callDetails->callerLocation);
			SetItemText(iPosition,4,callStatusStr);
		}

        // TES #492 - emergency calls should be in red
        if (callDetails->isEmergency)
        {
            setItemColour(callDetails->callID,
                                         EMERGENCY_TEXT_COLOUR,
                                         TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                         TA_Base_Bus::ListCtrlSelNoFocus::CT_FOREGROUND);
        }
        else
        {
            // Just in case any colour settings managed to stay present from
            // old items (shouldn't be able to happeb), clear any item data
            //  colour settings for the normal calls
            try
            {
                clearItemColour(callDetails->callID, TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA);            
            }
            catch (...)
            {
            }
        }

		// Free up the CORBA object
		delete callDetails;
	}
	catch (TA_Base_Bus::IRadioCorbaDef::invalidCallIDException&)
	{
		// Cannot get call details - ignore the call as it must have been deleted 
		// by another user between getList and this call.
	}
	catch(...)
	{
		// Cannot access the radio agent
		iPosition = InsertItem(0,"Unable to get the call details from Radio Agent.");
		SetItemData(iPosition,callID);
		m_isLoaded = false;  // Reload the call list on next notification
	}

	FUNCTION_EXIT;
}

void CRadioCallStackListCtrl::removeCall(long callID)
{
 	FUNCTION_ENTRY("removeCall");

    try
    {
    	// Find the call in the stack
    	int iPos=0;
    	while (iPos<GetItemCount())
    	{
    		long  selectedCallID;
    
    		// Get the stored reference
    		selectedCallID = GetItemData(iPos);
    
    		// Is it the one we want?
            if (selectedCallID==callID)
    		{
                // Clear any possible colour mappings for item prior to removal
                // (these mappings will only exist for emergency calls)
                try
                {
    		        clearItemColour(callID, TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA);
                }
                catch (...)
                {
                }
    
    		    // Remove from list
                DeleteItem(iPos);
    		    break;
            }
    
    		// try next 
    		iPos++;
    	}
    }
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::removeCall");
    }

	FUNCTION_EXIT;

}

void CRadioCallStackListCtrl::updateCall(long callID,TA_Base_Bus::IRadioCorbaDef::ECallStatusType status, TA_Base_Bus::IRadioCorbaDef::ECallCategory category)
{
 	FUNCTION_ENTRY("updateCall");

	// Find the call in the stack
	int iPos=0;
	while (iPos<GetItemCount())
	{
		long  selectedCallID;

		// Get the stored reference
		selectedCallID = GetItemData(iPos);

		// Is it the one we want?
        if (selectedCallID==callID)
		{
			CString callStatusStr = getStatusText(status, category);

		    // Update the status
			if (m_stackType == CRadioCallStackListCtrl::EStackType::OthersStack)
			{
				SetItemText(iPos,5,callStatusStr);
			}
			else
			{
				SetItemText(iPos,4,callStatusStr);
			}
			break;
        }

		iPos++;
	}

	FUNCTION_EXIT;
}


void CRadioCallStackListCtrl::PreSubclassWindow() 
{
 	FUNCTION_ENTRY("PreSubclassWindow");

	ListCtrlSelNoFocus::PreSubclassWindow();
    setupListControl();
	//startSubscription();

	FUNCTION_EXIT;
}

BOOL CRadioCallStackListCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
 	FUNCTION_ENTRY("Create");

    BOOL bResult = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

    try
    {
	    if (bResult!=0)
	    {
    		//startSubscription();
		    setupListControl();
	    }
    }
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::Create");
    }

	FUNCTION_EXIT;

	return bResult;
}


void CRadioCallStackListCtrl::setupListControl()
{
 	FUNCTION_ENTRY("setupListControl");

	// Create the required columns
	InsertColumn(1,"Date/Time");    // Changed Aug-13-04 for TES #570
	InsertColumn(2,"");
	InsertColumn(3,"");
	InsertColumn(4,"");
	InsertColumn(5,"");
	InsertColumn(6,"");
	InsertColumn(7,"");

	// Attach the image list
    SetImageList(&m_ImageList,LVSIL_SMALL);

	FUNCTION_EXIT;
}

void CRadioCallStackListCtrl::startSubscription()
{
 	FUNCTION_ENTRY("startSubscription");

    try
    {
    	if (!m_bSubscribed)
    	{
    		// Hold processing of subscriptions
    		TA_Base_Core::ThreadGuard guard(m_updateLock);
    
    		// Start subscription to changes
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "subscribeTo RadioCall Status: m_radioEntityKey = %d,m_radioEntityLocation = %d",m_radioEntityKey,m_radioEntityLocation);
    		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::RadioCallStatus, this, m_radioEntityKey, 0, m_radioEntityLocation, NULL );
    		m_bSubscribed = true;
    
    		// Load all call details
    		m_isLoaded = false;  // Ensure list is reloaded
    		loadCalls();
    	}
    }
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::startSubscription");
    }

	FUNCTION_EXIT;
}

void CRadioCallStackListCtrl::stopSubscription()
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
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::stopSubscription");
    }

	FUNCTION_EXIT;
}

afx_msg LRESULT CRadioCallStackListCtrl::onCallNotification(WPARAM wParam, LPARAM lParam)
{
 	FUNCTION_ENTRY("onCallNotification");

    try
    {
	    TA_Base_Bus::IRadioCorbaDef::CallStatusEvent*  callStatusEvent = reinterpret_cast<TA_Base_Bus::IRadioCorbaDef::CallStatusEvent*>(lParam);
	    TA_ASSERT(callStatusEvent!=NULL,"No call event specified on call.");
	    processUpdate(*callStatusEvent);
	    delete callStatusEvent;
    }
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::onCallNotification");
    }

	FUNCTION_EXIT;
	return 0;
}

void CRadioCallStackListCtrl::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {
    	// Wait until the initial list loading is complete
    	TA_Base_Core::ThreadGuard guard(m_updateLock);
    
    	// Don't process if window is not active
        if (!m_bSubscribed) return;
    
    	// Make sure the list is loaded
    	if (loadCalls())
    	{
    		// The data structure you want
    		TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatus = NULL;
    
    		std::string messageTypeKey(message.messageTypeKey);
    		// Extract to the messageState
    		if ( ( message.messageState >>= callStatus ) != 0)
    		{
    			// process messageState now
    			// Copy Message as processing will be done after function returns
    			TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusEvent = new TA_Base_Bus::IRadioCorbaDef::CallStatusEvent(*callStatus);
    
    			// Post message for processing by main GUI thread
    			PostMessage(CALL_NOTIFICATION_MSG,0,(LPARAM)callStatusEvent);
    		}
    		else
    		{
    			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Event does not contain dataPointEvent");
    		}
    	}
    }
    catch (...)
    {
        LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "Unknown", 
                "While processing CRadioCallStackListCtrl::receiveSpecialisedMessage");
    }

	FUNCTION_EXIT;
}



const char* CRadioCallStackListCtrl::getStatusText(TA_Base_Bus::IRadioCorbaDef::ECallStatusType status, TA_Base_Bus::IRadioCorbaDef::ECallCategory category)
{
 	FUNCTION_ENTRY("getStatusText");

	switch (status)
	{
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Ringing:
			{
				if (category == TA_Base_Bus::IRadioCorbaDef::CallDriver)
				{
					return "CallBack";
				}
				return "Ringing";
			}
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Connected:    return "Connected";
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::Disconnected: return "Disconnected";
		case TA_Base_Bus::IRadioCorbaDef::ECallStatusType::OnHold:       return "On hold";
		default:
			return "Unknown";
	}

	FUNCTION_EXIT;
}
