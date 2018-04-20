/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/CallStackHistoryPage.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  *
  */
// CallStackHistoryPage.cpp : implementation file
//

#include "stdafx.h"
#include <time.h>
#include "resource.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"
#include "app/radio/radio_manager_gui/src/SelectedCall.h"
#include "core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.h"
#include "core/data_access_interface/radio/src/IRACallStackHistory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/utilities/src/RunParams.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "CallStackHistoryPage.h"
#include "CurrentCall.h"
#include "RmLayoutHelper.h"

#include "ace/OS.h"

const char* ORIGIN_IN_TEXT  = "IN";
const char* ORIGIN_OUT_TEXT = "OUT";

const char* CALL_TYPE_VOICE_TEXT      = "VOICE";
const char* CALL_TYPE_CONFERENCE_TEXT = "CONFERENCE";
const char* CALL_TYPE_GROUP_ACK		  = "ACKNOWLEDGE";
const char* CALL_TYPE_GROUP_UNACK	  = "UNACKNOWLEDGE";
const char* CALL_TYPE_GROUP_BROADCAST = "BROADCAST";
const char* CALL_TYPE_TEXT_TEXT       = "TEXT";

const int   MAX_HISTORY_ENTRIES = 50;


const int	TIME_COLUMN_NUMBER       = 0;
const int	ORIGIN_COLUMN_NUMBER     = 1;
const int	TYPE_COLUMN_NUMBER       = 2;
const int	IDENTIFIER_COLUMN_NUMBER = 3;
const int	LOCATION_COLUMN_NUMBER   = 4;


const char* TIME_COLUMN_LABEL       = "Date/Time";
const char* ORIGIN_COLUMN_LABEL     = "Origin";
const char* TYPE_COLUMN_LABEL       = "Type";
const char* IDENTIFIER_COLUMN_LABEL = "Caller/Called ID";
const char* LOCATION_COLUMN_LABEL   = "Loc";


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCallStackHistoryPage, CDialog)




/////////////////////////////////////////////////////////////////////////////
// CCallStackHistoryPage property page

CCallStackHistoryPage::CCallStackHistoryPage() 
	: CDialog(CCallStackHistoryPage::IDD),
	  m_location(0)
{
	//{{AFX_DATA_INIT(CCallStackHistoryPage)
	m_inCheckBox = FALSE;
	m_outCheckBox = FALSE;
	m_confCheckBox = FALSE;
	//}}AFX_DATA_INIT
}

CCallStackHistoryPage::~CCallStackHistoryPage()
{
}

void CCallStackHistoryPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallStackHistoryPage)
	DDX_Control(pDX, IDC_COMBO1, m_locationCombo);
	DDX_Control(pDX, IDC_HISTORYCALLLIST, m_HistoryCallList);
	DDX_Check(pDX, IDC_CHECK1, m_inCheckBox);
	DDX_Check(pDX, IDC_CHECK2, m_outCheckBox);
	DDX_Check(pDX, IDC_CHECK3, m_confCheckBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCallStackHistoryPage, CDialog)
	//{{AFX_MSG_MAP(CCallStackHistoryPage)
	ON_BN_CLICKED(IDC_CHECK1, onOriginInChecked)
	ON_BN_CLICKED(IDC_CHECK2, onOriginOutChecked)
	ON_BN_CLICKED(IDC_CHECK3, onConferenceChecked)
	ON_CBN_SELCHANGE(IDC_COMBO1, onLocationChanged)
	ON_BN_CLICKED(IDC_BUTTON1, onRedialSelected)
	ON_BN_CLICKED(IDC_BUTTON2, onRedialLastOutgoing)
	ON_BN_CLICKED(IDC_BUTTON4, onRedialLastIncoming)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HISTORYCALLLIST, onItemChangedHistoryCalllist)
    ON_NOTIFY(NM_DBLCLK, IDC_HISTORYCALLLIST, onDblclkHistoryCalllist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCallStackHistoryPage::addCallToHistory(const long callID)
{
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
	TA_ASSERT(manager!=NULL,"Main window incorrectly defined.");

	// Get the call details
	try
	{
		TA_Base_Bus::IRadioCorbaDef::CallDetailsType* details = NULL;
		std::string identifier;
		std::string location;
		std::string itsi;

        CORBA_CALL_RETURN( details, manager->getRadioCorbaDef(), getCallDetails, ( callID,
				           ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );//limin
		if (details!=NULL)
		{
			// Determine parameters
			TA_Base_Core::IRACallStackHistory::OriginType originType;
			if (details->callSource==TA_Base_Bus::IRadioCorbaDef::ECallSource::OutgoingCall)
			{
				originType = TA_Base_Core::IRACallStackHistory::OUTBOUND;
				identifier = details->calledName;
				location = 	details->calledLocation;
				itsi = details->calledTSI;
			}
			else
			{
				originType = TA_Base_Core::IRACallStackHistory::INBOUND;
				identifier = details->callerName;
				location = 	details->callerLocation;
				itsi = details->callerTSI;
			}


			TA_Base_Core::IRACallStackHistory::CallType callType = TA_Base_Core::IRACallStackHistory::TEXT;
			if (details->callCategory==TA_Base_Bus::IRadioCorbaDef::HalfDuplex)
			{
				switch(details->callType)
				{
				case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupAcknowledgeCall:
					callType = TA_Base_Core::IRACallStackHistory::GROUP_ACK;
					break;
				case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupUnacknowledgeCall:
					callType = TA_Base_Core::IRACallStackHistory::GROUP_UNACK;
					break;
				case TA_Base_Bus::IRadioCorbaDef::ECallType::GroupBroadcastCall:
					callType = TA_Base_Core::IRACallStackHistory::GROUP_BROADCAST;
					break;
				default:
					callType = TA_Base_Core::IRACallStackHistory::CONFERENCE;
				}
			}
			else if (details->callCategory==TA_Base_Bus::IRadioCorbaDef::FullDuplex)
			{
				callType = TA_Base_Core::IRACallStackHistory::VOICE;
			}

			// Add the call to the stack
			// td17294
	        // addCallToHistory(details->time,originType,callType,identifier,location,itsi);
			addCallToHistory(details->time,originType,callType,identifier,location,itsi,details->isEmergency); 
			

			delete details;
			details=NULL;
		}
	}
	catch (...)
	{
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "get call details to add to history stack";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to get call details to add to history stack.");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
	}



}

void CCallStackHistoryPage::addCallToHistory(const time_t& timestamp,
											 TA_Base_Core::IRACallStackHistory::OriginType originType,
											 TA_Base_Core::IRACallStackHistory::CallType callType,
											 const std::string& identifier,
											 const std::string& location,
											 const std::string& itsi,
											 bool isEmergency) // td17294
{
	std::string identifierWithTSI = identifier;
	identifierWithTSI += " (";
	identifierWithTSI += itsi;
	identifierWithTSI += ")";

    try
    {
	    // Add the new call to the history list
	    addCallToHistoryList(timestamp,originType,callType,identifierWithTSI,location,isEmergency);

	    // Save to database 
	    TA_Base_Core::RACallStackHistoryAccessFactory::getInstance().addCallStackEntry(m_profile,m_location,timestamp,
																			  originType,callType,identifierWithTSI,
																			  location,isEmergency);
    }
    SILENT_RM_CATCH_HANDLER("CCallStackHistoryPage::addCallToHistory");
}



BOOL CCallStackHistoryPage::OnInitDialog() 
{
//	SetFont(CDialogFont::GetInstance(),true);
	CDialog::OnInitDialog();
	
	m_inCheckBox= true;
	m_outCheckBox= true;
	m_confCheckBox= true;
	UpdateData(false);
	
	// Setup the call list headings
	m_HistoryCallList.InsertColumn(TIME_COLUMN_NUMBER,TIME_COLUMN_LABEL);

	m_HistoryCallList.InsertColumn(ORIGIN_COLUMN_NUMBER,ORIGIN_COLUMN_LABEL);
	m_HistoryCallList.InsertColumn(TYPE_COLUMN_NUMBER,TYPE_COLUMN_LABEL);
	m_HistoryCallList.InsertColumn(IDENTIFIER_COLUMN_NUMBER,IDENTIFIER_COLUMN_LABEL);
	m_HistoryCallList.InsertColumn(LOCATION_COLUMN_NUMBER,LOCATION_COLUMN_LABEL);
//	m_HistoryCallList.InsertColumn(4,"E");

    const DWORD TIME_COL_WIDTH = 110;
    const DWORD TYPE_COL_WIDTH = 140;
    const DWORD ORIGIN_COL_WIDTH = 60;
    const DWORD IDENTIFIER_COL_WIDTH = 330;
    const DWORD LOC_COL_WIDTH = 20;
    
	m_HistoryCallList.SetColumnWidth(TIME_COLUMN_NUMBER,    TIME_COL_WIDTH);
	m_HistoryCallList.SetColumnWidth(ORIGIN_COLUMN_NUMBER,  ORIGIN_COL_WIDTH);
	m_HistoryCallList.SetColumnWidth(TYPE_COLUMN_NUMBER,    TYPE_COL_WIDTH);
	m_HistoryCallList.SetColumnWidth(IDENTIFIER_COLUMN_NUMBER,  IDENTIFIER_COL_WIDTH);
	m_HistoryCallList.SetColumnWidth(LOCATION_COLUMN_NUMBER,    LOC_COL_WIDTH);

    // associate red with true
    m_HistoryCallList.setItemColour( true,
                                     (DWORD) RGB(255,0,0),
                                     TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                     TA_Base_Bus::ListCtrlSelNoFocus::CT_FOREGROUND );


//	addExampleCalls();

	m_HistoryCallList.SetExtendedStyle(LVS_EX_FULLROWSELECT);


    // TES #422 - Provide default formatting
    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_HistoryCallList);

	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCallStackHistoryPage::Init()
{
	getLocationAndProfile();
	loadHistory();
	onUpdateRights();
}

void CCallStackHistoryPage::onOriginInChecked() 
{
	loadHistory();
}

void CCallStackHistoryPage::onOriginOutChecked() 
{
	loadHistory();	
}

void CCallStackHistoryPage::onConferenceChecked() 
{
	loadHistory();	
}

void CCallStackHistoryPage::onLocationChanged() 
{
	loadHistory(false);		
}

void CCallStackHistoryPage::onRedialSelected() 
{
	POSITION pos = m_HistoryCallList.GetFirstSelectedItemPosition();
	if (pos!=NULL)
	{
        try
        {
    		int selectedItem = m_HistoryCallList.GetNextSelectedItem(pos);

            // Get caller/called details
            CString identifierWithTSI = m_HistoryCallList.GetItemText(selectedItem,IDENTIFIER_COLUMN_NUMBER);
            CString identifier = identifierWithTSI.Left(identifierWithTSI.Find('(',0));
            
            CString tsiNumber = identifierWithTSI;
            tsiNumber.Delete(0,tsiNumber.Find('(',0)+1);
            tsiNumber.Remove(')');
            
            // Update selected area
            CSelectedCall::GetInstance()->SetCall(tsiNumber, identifier, false);
            
            // Make a call if voice
            CString callType = m_HistoryCallList.GetItemText(selectedItem,TYPE_COLUMN_NUMBER);
            if ((callType==CALL_TYPE_VOICE_TEXT)||(callType==CALL_TYPE_CONFERENCE_TEXT)
				||(callType==CALL_TYPE_GROUP_ACK)||(callType==CALL_TYPE_GROUP_UNACK)
				||(callType==CALL_TYPE_GROUP_BROADCAST))
            {
            	AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND,IDC_CALL);
            }
            else if (callType == CALL_TYPE_TEXT_TEXT)
            {
                // TD#3905 - Display read text message dialog
                // AfxGetMainWnd()->PostMessage(WM_DISPLAY_SDS_MESSAGE, callID, 0);
            }
        }
        SILENT_RM_CATCH_HANDLER("CCallStackHistoryPage::onRedialSelected");
	}
}

void CCallStackHistoryPage::onRedialLastOutgoing() 
{
	bool isFound = false;

	// Find last outgoing
	for (int item=0;item<m_HistoryCallList.GetItemCount();item++)
	{
		CString callOrigin = m_HistoryCallList.GetItemText(item,ORIGIN_COLUMN_NUMBER);
		if ((!isFound) && (callOrigin == ORIGIN_OUT_TEXT))
		{
			// Select the call
			m_HistoryCallList.SetItemState(item,LVIS_SELECTED,LVIS_SELECTED);
			isFound = true;
		}
		else
		{
			// Deselect call
			m_HistoryCallList.SetItemState(item,0,LVIS_SELECTED);
		}
	}
	
	if (isFound)
	{
		onRedialSelected();
	}
}

void CCallStackHistoryPage::onRedialLastIncoming() 
{
	bool isFound = false;

	// Find last Incoming
	for (int item=0;item<m_HistoryCallList.GetItemCount();item++)
	{
		CString callOrigin = m_HistoryCallList.GetItemText(item,ORIGIN_COLUMN_NUMBER);
		if ((!isFound) && (callOrigin == ORIGIN_IN_TEXT))
		{
			// Select the call
			m_HistoryCallList.SetItemState(item,LVIS_SELECTED,LVIS_SELECTED);
			isFound = true;
		}
		else
		{
			// Deselect call
			m_HistoryCallList.SetItemState(item,0,LVIS_SELECTED);
		}
	}
	
	if (isFound)
	{
		onRedialSelected();
	}
}

void CCallStackHistoryPage::onUpdateRights()
{
    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
    
    	bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_MAKE_VOICE_CALL);
    
    	bool isLocationFiltered = false;
    	if (enable)
    	{
    		CString locationFilter;
    		m_locationCombo.GetWindowText(locationFilter);
    		//TD17306
			if(locationFilter.Compare("<ALL>") != 0)
			{
				isLocationFiltered = true;
			}

    		//isLocationFiltered = (!locationFilter.IsEmpty());
    	}
    
    	GetDlgItem(IDC_BUTTON1)->EnableWindow(enable&&(m_HistoryCallList.GetSelectedCount()>0));
    	GetDlgItem(IDC_BUTTON2)->EnableWindow(enable&&!isLocationFiltered&&(IsDlgButtonChecked(IDC_CHECK2)==BST_CHECKED)&&(getCallCount(false)>0));
    	GetDlgItem(IDC_BUTTON4)->EnableWindow(enable&&!isLocationFiltered&&(IsDlgButtonChecked(IDC_CHECK1)==BST_CHECKED)&&(getCallCount(true)>0));
    }
    SILENT_RM_CATCH_HANDLER("CCallStackHistoryPage::onUpdateRights");
}

afx_msg void CCallStackHistoryPage::OnCancel()
{
	// Do nothing
}

afx_msg void CCallStackHistoryPage::OnOK()
{
	// Do nothing
}




void CCallStackHistoryPage::addCallToHistoryList(const time_t& timestamp,
												 TA_Base_Core::IRACallStackHistory::OriginType originType,
							                     TA_Base_Core::IRACallStackHistory::CallType callType,
												 const std::string& identifier,
							                     const std::string& location,
												  bool isEmergency )
{
    try
    {   
    	if ( ( callType==TA_Base_Core::IRACallStackHistory::CONFERENCE ) ||
			 ( callType==TA_Base_Core::IRACallStackHistory::GROUP_ACK ) ||
			 ( callType==TA_Base_Core::IRACallStackHistory::GROUP_UNACK ) ||
			 ( callType==TA_Base_Core::IRACallStackHistory::GROUP_BROADCAST ) )
    	{
    		if ( BST_CHECKED != IsDlgButtonChecked( IDC_CHECK3 ) )
    		{
    			// Conference call and conference not checked
    			return;
    		}
    	}
    	else
    	{
    		// Check if the filter should exclude the display of this call
    		if ( ( TA_Base_Core::IRACallStackHistory::INBOUND == originType ) && 
                 ( BST_CHECKED != IsDlgButtonChecked( IDC_CHECK1 ) ) )
    		{
    			// Incoming call and in origin not checked
    			return;
    		}
    
            if ( ( TA_Base_Core::IRACallStackHistory::OUTBOUND == originType ) && 
                 ( BST_CHECKED != IsDlgButtonChecked( IDC_CHECK2 ) ) )
    		{
    			// Outgoing call and out origin not checked
    			return;
    		}
    	}
    
    	CString locationFilter;
    	m_locationCombo.GetWindowText(locationFilter);
		if ( locationFilter.Compare("<ALL>") != 0 )
    	{
			if ( locationFilter.Compare("<NO LOCATION>") != 0 ) //filter specified
			{
				if ( ( locationFilter.Compare("") != 0 ) &&
                     ( stricmp( locationFilter, location.c_str() ) != 0 ) )
				{
					// Location filter specified and it does not match this call   
    				return;
				}
			}
			else // filter = NO LOCATION
			{
				if ( stricmp( "", location.c_str() ) != 0 )
				{
					return;
				}
			}
    	}

        // Ensure max of 50
    	while ( m_HistoryCallList.GetItemCount() >= MAX_HISTORY_ENTRIES )
    	{  
			DWORD itemData = m_HistoryCallList.GetItemData(MAX_HISTORY_ENTRIES-1);
			
			m_HistoryCallList.DeleteItem( MAX_HISTORY_ENTRIES - 1 );
    	}
    
    	// Create new entry
    	char szBuffer[100];
    	struct tm today;
        ACE_OS::localtime_r( &timestamp, &today );
    	strftime( szBuffer, sizeof(szBuffer), "%d-%b %H:%M:%S", &today );
    
    	int iPosition = m_HistoryCallList.InsertItem( 0, szBuffer, 0 );
        m_HistoryCallList.SetItemData( iPosition, isEmergency ); // red is associated with true item data

    	m_HistoryCallList.SetItemText( iPosition, TIME_COLUMN_NUMBER, szBuffer );  // Do again incase ordering of cols changed

    	std::string originTypeText = ORIGIN_IN_TEXT;
    	if ( TA_Base_Core::IRACallStackHistory::OUTBOUND == originType )
    	{
    		originTypeText = ORIGIN_OUT_TEXT;
    	}
    	
        m_HistoryCallList.SetItemText( iPosition, ORIGIN_COLUMN_NUMBER, originTypeText.c_str() );
    
    	
    	// Set the fields
    	std::string callTypeText = CALL_TYPE_VOICE_TEXT;
    	if ( TA_Base_Core::IRACallStackHistory::TEXT == callType )
    	{
    		callTypeText = CALL_TYPE_TEXT_TEXT;
    	}
    	else if ( TA_Base_Core::IRACallStackHistory::CONFERENCE == callType )
    	{
    		callTypeText = CALL_TYPE_CONFERENCE_TEXT;
    	}
		else if ( TA_Base_Core::IRACallStackHistory::GROUP_ACK == callType )
		{
			callTypeText = CALL_TYPE_GROUP_ACK;
		}
		else if ( TA_Base_Core::IRACallStackHistory::GROUP_UNACK == callType )
		{
			callTypeText = CALL_TYPE_GROUP_UNACK;
		}
		else if ( TA_Base_Core::IRACallStackHistory::GROUP_BROADCAST == callType )
		{
			callTypeText = CALL_TYPE_GROUP_BROADCAST;
		}

		m_HistoryCallList.SetItemText( iPosition, TYPE_COLUMN_NUMBER, callTypeText.c_str() );
    	m_HistoryCallList.SetItemText( iPosition, IDENTIFIER_COLUMN_NUMBER, identifier.c_str() );
    	m_HistoryCallList.SetItemText( iPosition, LOCATION_COLUMN_NUMBER, location.c_str() );

        m_HistoryCallList.SetRedraw( TRUE );
    
    	// Add location to list if not already there
    	if ( ( false == location.empty() ) &&
             ( CB_ERR == m_locationCombo.FindStringExact( -1, location.c_str() ) ) )
    	{
			m_locationCombo.InsertString( m_locationCombo.GetCount() - 1, location.c_str() );
    	}
    }
    SILENT_RM_CATCH_HANDLER("CCallStackHistoryPage::addCallToHistoryList");
}

int CCallStackHistoryPage::getCallCount(bool isOriginIn)
{
	int  count = 0;

	for (int item=0;item<m_HistoryCallList.GetItemCount();item++)
	{
		CString callOrigin = m_HistoryCallList.GetItemText(item,ORIGIN_COLUMN_NUMBER);
		if (((isOriginIn) && (callOrigin == ORIGIN_IN_TEXT)) ||
		    ((!isOriginIn) && (callOrigin == ORIGIN_OUT_TEXT)))
		{
			count++;
		}
	}

	return count;
}

void CCallStackHistoryPage::loadHistory(bool resetLocations/*=true*/)
{
	// Clear call stack and location combo lists
	if(m_HistoryCallList.m_hWnd!=NULL)
	{
		m_HistoryCallList.DeleteAllItems();
	}
	if (resetLocations)
	{
		m_locationCombo.ResetContent();
		//m_locationCombo.AddString("");
		m_locationCombo.AddString("<ALL>");  //TD17306
		m_locationCombo.AddString("<NO LOCATION>");//TD17306
	}

	try
	{
		// Load the existing history list for the user/profile
		TA_Base_Core::RACallStackHistoryAccessFactory::CallStackHistoryList history;
		history = TA_Base_Core::RACallStackHistoryAccessFactory::getInstance().getList(m_profile,m_location);

		for (unsigned int entry = 0; entry<history.size();entry++)
		{
			addCallToHistoryList(history[entry]->getTimestamp(),
								 history[entry]->getOrigin(),
								 history[entry]->getCallType(),
								 history[entry]->getIdentifier(),
								 history[entry]->getLocation(),
								 history[entry]->isCallEmergency()

				                );
			delete history[entry];
			history[entry]=NULL;
		}

	}
	catch (...)
	{
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "load previous call history from database";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to load previous call history from database.");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
	}

	// Recheck buttons
	onUpdateRights();	

}

void CCallStackHistoryPage::getLocationAndProfile()
{
	std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_ASSERT(sessionID.size()>0,"No sessionID obtained from run parameters.");

	TA_Base_Core::IProfile*  profile = NULL;

	try
	{
		TA_Base_Bus::AuthenticationLibrary  authLib(true);

		TA_Base_Bus::SessionInfo sessionInfo = authLib.getSessionInfo(sessionID,sessionID);
		m_location = sessionInfo.LocationId;

		profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(sessionInfo.ProfileId[0]);
		m_profile = profile->getName();
		delete profile;
		profile = NULL;
	}
	catch (...)
	{
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "determine session profile and location";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to determine session profile and location.");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
		delete profile;
	}
}


void CCallStackHistoryPage::onItemChangedHistoryCalllist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	onUpdateRights();	
	*pResult = 0;
}


void CCallStackHistoryPage::addExampleCalls()
{
	// Add some examples
	int   iPosition;
	
	iPosition = m_HistoryCallList.InsertItem(0,"18-Aug 11:20:02",0);
	m_HistoryCallList.SetItemText(iPosition,1,"IN");
	m_HistoryCallList.SetItemText(iPosition,2,"UNACkNOWLEDGED");
	m_HistoryCallList.SetItemText(iPosition,3,"SO");
	m_HistoryCallList.SetItemText(iPosition,4,"DBG");

	iPosition = m_HistoryCallList.InsertItem(1,"18-Aug 11:21:02",0);
	m_HistoryCallList.SetItemText(iPosition,1,"IN");
	m_HistoryCallList.SetItemText(iPosition,2,"TEXT");
	m_HistoryCallList.SetItemText(iPosition,3,"DCO");
	m_HistoryCallList.SetItemText(iPosition,4,"DCR");

	iPosition = m_HistoryCallList.InsertItem(2,"18-Aug 11:22:02",0);
	m_HistoryCallList.SetItemText(iPosition,1,"IN");
	m_HistoryCallList.SetItemText(iPosition,2,"VOICE");
	m_HistoryCallList.SetItemText(iPosition,3,"DCO");
	m_HistoryCallList.SetItemText(iPosition,4,"DCR");

	iPosition = m_HistoryCallList.InsertItem(3,"18-Aug 11:23:02",0);
	m_HistoryCallList.SetItemText(iPosition,1,"IN");
	m_HistoryCallList.SetItemText(iPosition,2,"CONFERENCE");
	m_HistoryCallList.SetItemText(iPosition,3,"SO MSN,SO DBG,SO CVC");
	m_HistoryCallList.SetItemText(iPosition,4,"");

	iPosition = m_HistoryCallList.InsertItem(4,"18-Aug 11:24:02",0);
	m_HistoryCallList.SetItemText(iPosition,1,"OUT");
	m_HistoryCallList.SetItemText(iPosition,2,"VOICE");
	m_HistoryCallList.SetItemText(iPosition,3,"SO");
	m_HistoryCallList.SetItemText(iPosition,4,"MSM");
}


void CCallStackHistoryPage::onDblclkHistoryCalllist(NMHDR* pNMHDR, LRESULT* pResult)
{
    onRedialSelected();
}
	
