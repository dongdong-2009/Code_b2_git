/*
 * The soiurce code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DirectLineCache.cpp $
 * @author:  Andy Parker
 * @version: $Revision: #2 $
 *
 * Last mofification: $DateTime: 2009/02/10 16:25:11 $
 * Last modified by:  $Author: builder $
 *
 * This class manages the set of direct lines used by the application.  It 
 * is accessed by the direct line property page and direct line configuration
 * property page
 *
 */

// System includes
#include "stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioDirectLines.h"
#include "app/radio/radio_manager_gui/src/Resource.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"
#include "DirectLineCache.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

const CString   CDirectLineCache::HOT_LINE_SET_NAME = "HotLines";

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Global instance used by Direct Line and Direct Line config.
CDirectLineCache   CDirectLineCache::s_Instance;

void CDirectLineCache::SetUsername(CString userName)
{
	m_userName = userName;
}


CDirectLineCache::CDirectLineCache()
:
m_cacheReloadRequired(false),
m_processSubscribedMessages(false)
{
    // Setup the required button sets
    m_buttonSets[0] = "User";
    m_buttonSets[1] = "Stations";
    m_buttonSets[2] = "OCR";
    m_buttonSets[3] = "DCR";
    m_buttonSets[4] = "Others";
}

CDirectLineCache::~CDirectLineCache()
{
}

UINT CDirectLineCache::GetButtonSetCount()
{
	return sizeof(m_buttonSets)/sizeof(m_buttonSets[0]);
}

CString CDirectLineCache::GetButtonSetName(UINT iSet)
{
	ASSERT (iSet>=0);
	ASSERT (iSet<GetButtonSetCount());

	return m_buttonSets[iSet];
}

bool CDirectLineCache::ReloadCache()
{
    // Called from updating thread
    TA_Base_Core::ThreadGuard guard(m_commsLock);

    bool bSuccessful = true;
    bool exceptionThrown = true;

    try
    {
        CWaitCursor wait;
    	
    	CString userButtonSetName;
    
    	// Get the hot buttons from the agent
        if (!m_radioDirectLines.GetDirectHotLines(m_hotLines))
    	{
    		TRACE0("Unable to load direct dial hot lines\n");
    		m_hotLines.clear();
    		bSuccessful = false;
    	}
    
    	// Get the user defined set of buttons
    	userButtonSetName = m_buttonSets[0];
    	//userButtonSetName += m_userName;
        if (!m_radioDirectLines.GetDirectLines((LPCSTR)userButtonSetName,m_buttonLines[0])) 
        {
    	   TRACE0("Unable to load user defined direct dial lines\n");
      	   m_buttonLines[0].clear(); 
    	   bSuccessful = false;
        }
    
    
    	// Get the standard sets of buttons from the agent
    	for (UINT iSet=1;iSet<GetButtonSetCount();iSet++)
    	{
           if (!m_radioDirectLines.GetDirectLines((LPCSTR)m_buttonSets[iSet],m_buttonLines[iSet])) 
    	   {
    		   TRACE1("Unable to load direct dial lines '%s'\n",m_buttonSets[iSet]);
      		   m_buttonLines[iSet].clear(); 
    		   bSuccessful = false;
    	   }
    	}

        exceptionThrown = false;
    }
    SILENT_RM_CATCH_HANDLER("ReloadCache");

    if (exceptionThrown)
    {
        bSuccessful = false;
    }

	return bSuccessful;
}

bool CDirectLineCache::UpdateButton(UINT buttonSet,UINT buttonNumber,CString Label,CString Name)
{
    return true;
}

UINT  CDirectLineCache::GetButtonsPerSet(UINT buttonSet)
{
	ASSERT (buttonSet>=0);
	ASSERT (buttonSet<GetButtonSetCount());
	return m_buttonLines[buttonSet].size();
}


CString CDirectLineCache::GetButtonLabel(UINT buttonSet,UINT buttonNumber)
{
	ASSERT (buttonSet>=0);
	ASSERT (buttonSet<GetButtonSetCount());
	ASSERT (buttonNumber<m_buttonLines[buttonSet].size());

    return CString(m_buttonLines[buttonSet][buttonNumber].label.c_str());
}

void CDirectLineCache::SetButtonLabel(UINT buttonSet,UINT buttonNumber, std::string label)
{
	ASSERT (buttonSet>=0);
	ASSERT (buttonSet<GetButtonSetCount());
	ASSERT (buttonNumber<m_buttonLines[buttonSet].size());

	m_buttonLines[buttonSet][buttonNumber].label = label;
}

CString CDirectLineCache::GetButtonNumber(UINT buttonSet,UINT buttonNumber)
{

	ASSERT (buttonSet>=0);
	ASSERT (buttonSet<GetButtonSetCount());
	ASSERT (buttonNumber<m_buttonLines[buttonSet].size());

    return CString(m_buttonLines[buttonSet][buttonNumber].number.c_str());
}

void CDirectLineCache::SetButtonNumber(UINT buttonSet,UINT buttonNumber, std::string number)
{
	ASSERT (buttonSet>=0);
	ASSERT (buttonSet<GetButtonSetCount());
	ASSERT (buttonNumber<m_buttonLines[buttonSet].size());

	m_buttonLines[buttonSet][buttonNumber].number = number;
}


int  CDirectLineCache::GetHotButtonsInSet(void)
{
	return m_hotLines.size();
}


CString CDirectLineCache::GetHotButtonLabel(UINT buttonNumber)
{
	ASSERT (buttonNumber<m_hotLines.size());
	return CString(m_hotLines[buttonNumber].label.c_str());
}

CString CDirectLineCache::GetHotButtonNumber(UINT buttonNumber)
{
	ASSERT (buttonNumber<m_hotLines.size());
	return CString(m_hotLines[buttonNumber].number.c_str());
}

void CDirectLineCache::SetHotButtonLabel(UINT buttonNumber, std::string label)
{
	ASSERT (buttonNumber<m_hotLines.size());

	m_hotLines[buttonNumber].label = label;
}

void CDirectLineCache::SetHotButtonNumber(UINT buttonNumber, std::string number)
{
	ASSERT (buttonNumber<m_hotLines.size());

	m_hotLines[buttonNumber].number = number;
}


void CDirectLineCache::UnInitData()
{
    if (m_processSubscribedMessages)
    {
        TA_Base_Core::ThreadGuard guard(m_commsLock);

        // Ignore any further subscription messages
        m_processSubscribedMessages = false;
    
        try
        {   
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        }
        SILENT_RM_CATCH_HANDLER("unsubscribeToMessages");
    }
}


void CDirectLineCache::InitData(const std::string& radioName)
{
	FUNCTION_ENTRY("InitData");
    
    try
    {
        m_radioDirectLines.InitEntries();

        if (radioName.length() > 0)
        {
    	    // Determine entity key for the local radio agent
    	    // This is used to filter the incoming notifications from the radio agents    
            std::auto_ptr<TA_Base_Core::IEntityData> entity(
                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity(radioName));
    
        	TA_ASSERT(entity.get() != NULL,"Radio entity could not be found.");
        
        	unsigned long radioEntityKey = entity->getKey();
            unsigned long radioEntityLocation = entity->getLocation();
        
            // Begin subscription prior to refreshing cache
        	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
                    TA_Base_Core::RadioComms::RadioSpeedDialUpdateNotification, 
                    this, 
                    radioEntityKey, 
                    0, 
                    radioEntityLocation, 
                    NULL );
           
            m_processSubscribedMessages = true;
        }
    }
    SILENT_RM_CATCH_HANDLER("SpeedDialUpdateNotification subscription");    
    
    if (!m_processSubscribedMessages)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "subscribe to Speed Dial Updates";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to subscribe to Speed Dial Updates");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
        
    }

    // Load the buttons
	ReloadCache();
}


void CDirectLineCache::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {
        TA_Base_Core::ThreadGuard guard(m_commsLock);
    
        if (!m_processSubscribedMessages)
        {
            return;
        }
        TA_Base_Bus::IRadioCorbaDef::SpeedDialUpdateNotification* update = 0;

        if ( ( message.messageState >>= update ) != 0)
	    {
            processUpdate(*update);
        }
    }
    SILENT_RM_CATCH_HANDLER("CDirectLineCache::receiveSpecialisedMessage");

    FUNCTION_EXIT;
}


void CDirectLineCache::processUpdate(const TA_Base_Bus::IRadioCorbaDef::SpeedDialUpdateNotification& update)
{
    // Ignore the contents of the update itself, just perform a full reload
    // (this could be made much more efficient..)
    m_cacheReloadRequired = true;

    // The refresh message, when process, should call onRefreshNotificationReceived
    AfxGetApp()->m_pMainWnd->PostMessage(REFRESH_DIALOG_MSG, 1, 0);
}


void CDirectLineCache::onRefreshNotificationReceived()
{	   
    TA_Base_Core::ThreadGuard guard(m_commsLock);

    if (m_cacheReloadRequired)
    {
        ReloadCache();
        m_cacheReloadRequired = false;
    }
}


void CDirectLineCache::ApplyHotButtonChanges(UINT hotButtonNumber)
{
    changeButtonLineDetails(m_hotLines,hotButtonNumber,HOT_LINE_SET_NAME);
}

void CDirectLineCache::ApplyButtonChanges(UINT buttonSet,UINT buttonNumber)
{
	ASSERT (buttonSet >= 0);
	ASSERT (buttonSet < GetButtonSetCount());

    changeButtonLineDetails(m_buttonLines[buttonSet],buttonNumber,GetButtonSetName(buttonSet));
}

void CDirectLineCache::changeButtonLineDetails(CRadioDirectLines::DirectLineVector& buttonLine,UINT buttonNumber,
                                               const CString& buttonSetName)
{    
	ASSERT (buttonNumber < buttonLine.size());

    // Get the parent class and retrieve the reference to the readio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    TA_Base_Bus::IRadioCorbaDef::SpeedDialEntry entry;

    entry.label = buttonLine[buttonNumber].label.c_str();
    entry.TSI = buttonLine[buttonNumber].number.c_str();
    entry.position = buttonNumber;

    try
    {    
        CORBA_CALL( manager->getRadioCorbaDef(), updateSpeedDial, ( buttonSetName,entry,
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
    }
    catch(CORBA::Exception& ex)
    {
        std::string reason = "CORBA exception caught while attempting to save button details: " + 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",reason.c_str());

        // Re-load the cache to undo any previous changes
        ReloadCache();

        // Re-throw the exception for error message purposes
        throw;
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown","Unknown exception caught while attempting to save button details.");

        // Re-load the cache to undo any previous changes
        ReloadCache();

        // Re-throw the exception for error message purposes
        throw;
    }
}
