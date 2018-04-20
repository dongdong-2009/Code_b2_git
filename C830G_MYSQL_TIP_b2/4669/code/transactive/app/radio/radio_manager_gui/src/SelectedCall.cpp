/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SelectedCall.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/07/30 11:11:09 $
  * Last modified by:  $Author: builder $
  *
  */
// SelectedCall.cpp: implementation of the CSelectedCall class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/radio_manager_gui/src/stdafx.h"

#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"
#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"
#include "app/radio/radio_manager_gui/src/SelectedCall.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/radio/radiosearchpage/src/searchpage.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/GroupInUseException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"

#include <iomanip>
#include <iostream>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CSelectedCall* CSelectedCall::m_theClassInstance;
TA_Base_Core::NonReEntrantThreadLockable CSelectedCall::m_singletonLock;

CSelectedCall* CSelectedCall::GetInstance()
{
    FUNCTION_ENTRY( "GetInstance" );

    TA_Base_Core::ThreadGuard guard( m_singletonLock );

	// If guard acquired then make sure the singleton is still NULL.

	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
        m_theClassInstance = new CSelectedCall();
	}
	
    FUNCTION_EXIT;
	return m_theClassInstance;
}


void CSelectedCall::removeInstance()
{
    FUNCTION_ENTRY( "removeInstance" );

    TA_Base_Core::ThreadGuard guard( m_singletonLock );

    delete m_theClassInstance;
	m_theClassInstance = NULL;

    FUNCTION_EXIT;
}


CSelectedCall::CSelectedCall()
              : m_isHotLine(false),
                m_callDisplayed(false),
                m_displayedCallType(CALL_NONE),
                m_groupCall(true)
{
    FUNCTION_ENTRY( "CSelectedCall" );
    FUNCTION_EXIT;
}


CSelectedCall::~CSelectedCall()
{
    FUNCTION_ENTRY( "~CSelectedCall" );

    if ( NULL != m_trainRadioGroupCorbaProxy.get() )
    {
        m_trainRadioGroupCorbaProxy->removeObserver( this );
    }

    FUNCTION_EXIT;
}


void CSelectedCall::Clear(void)
{
    FUNCTION_ENTRY( "Clear" );

    CString blank;

    SetCall(blank,blank,false);
    m_callDisplayed = false;
    // m_freeCallDisplayed = false;
    m_displayedCallType = CALL_NONE;

    FUNCTION_EXIT;
}


bool CSelectedCall::IsCallDisplayed(void)
{
    FUNCTION_ENTRY( "IsCallDisplayed" );
    FUNCTION_EXIT;
    return m_callDisplayed;
}


void CSelectedCall::setCallDisplayed(bool callDisplayed)
{
    FUNCTION_ENTRY( "setCallDisplayed" );

    m_callDisplayed = callDisplayed;

    //m_freeCallDisplayed = callDisplayed;
    m_displayedCallType = FREE_CALL;

    FUNCTION_EXIT;
}


void CSelectedCall::SetDynamicGroupCall(CString& label)
{
    FUNCTION_ENTRY( "SetDynamicGroupCall" );

    // Get the parent for controls in the "Selected Area"
    CDialog* pDlg = (CDialog*)AfxGetApp()->m_pMainWnd;
    ASSERT (pDlg!=NULL);

    // Display the number - empty for dynamic groups
    pDlg->SetDlgItemText(IDC_SA_TETRAID, "");

    // Use the supplied label
    pDlg->SetDlgItemText(IDC_SA_ID, label);
    // Clear the location - not applicable for dynamic groups
    pDlg->SetDlgItemText(IDC_SA_LOCATION, "");

    m_callDisplayed = true;
    //m_freeCallDisplayed = false;
    m_displayedCallType = DYNAMIC_GROUP;
    m_isHotLine = false;

    // Tell main dialog to update its buttons
    pDlg->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);

    FUNCTION_EXIT;
}


// Update the area
void CSelectedCall::SetCall(CString& tsiNumber, CString& label, bool hotline)
{
    FUNCTION_ENTRY( "SetCall" );

    // Get the parent for controls in the "Selected Area"
    //CDialog* pDlg = dynamic_cast<CDialog*>( AfxGetApp()->m_pMainWnd);
    CDialog* pDlg = (CDialog*)AfxGetApp()->m_pMainWnd;
    ASSERT (pDlg!=NULL);

    // Display the number
    pDlg->SetDlgItemText(IDC_SA_TETRAID,tsiNumber);

    bool isTrain = false;
    CString correctedTetraId = getCorrectedTetraId(tsiNumber, &isTrain);

    // Search directory for matching details
    if (!isTrain)
    {
        try
        {
            // Get the parent class and retrieve the reference to the readio manager
            CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
            TA_Base_Bus::IRadioCorbaDef::SubscriberDetails_var subscriber;
            CORBA_CALL_RETURN( subscriber, manager->getRadioCorbaDef(), getSubscriberDetails, ( tsiNumber ) );//limin
            //Enable the Include Button in the RadioManager GUI depending on the type of the call selected now
            //and the call selected in the current call list
            if(subscriber->type == TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER)
            {
                CCurrentCall* currentCall = CCurrentCall::GetInstance();
                manager->m_includeButton.EnableWindow((currentCall->IsCallSelected())
                         &&(!currentCall->IsHalfDuplexCallSelected())
                         &&(manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_INCLUDE_CALL)));
                m_groupCall = false;
            }
            else
            {
                manager->m_includeButton.EnableWindow(false);
                m_groupCall = true;
            }

            pDlg->SetDlgItemText(IDC_SA_ID,subscriber->subscriberName);
            pDlg->SetDlgItemText(IDC_SA_LOCATION,subscriber->locationName);
        }
        catch(...)
        {
            // Not found in directory so use the supplied label
            pDlg->SetDlgItemText(IDC_SA_ID,label);
            // Clear the location, we don't know where subscriber is (TES #464)
            pDlg->SetDlgItemText(IDC_SA_LOCATION, "");
        }
    }
    else
    {

        // must treat trains in special manner - only require location detail to be refreshed
        pDlg->SetDlgItemText(IDC_SA_ID, label);

        try
        {
            // Get the parent class and retrieve the reference to the readio manager
            CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
            CCurrentCall* currentCall = CCurrentCall::GetInstance();
            //Enable the include Button if the call in the Current Call list is Single
            manager->m_includeButton.EnableWindow((currentCall->IsCallSelected())
                             &&(!currentCall->IsHalfDuplexCallSelected())
                             &&(manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_INCLUDE_CALL)));
            m_groupCall = false;
            unsigned long locationKey = getTrainDetailsFromTSI(tsiNumber).currentLocation;
            CString locationName = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getLocationNameFromKey(locationKey);
            pDlg->SetDlgItemText(IDC_SA_LOCATION, locationName);
        }
        catch (...)
        {
            pDlg->SetDlgItemText(IDC_SA_LOCATION, "");
        }
    }

    m_callDisplayed = true;
    //m_freeCallDisplayed = false;
    m_displayedCallType = NORMAL_CALL;
    m_isHotLine = hotline;

    // Tell main dialog to update its buttons
    pDlg->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);

    FUNCTION_EXIT;
}


void CSelectedCall::saveFreeCall(void)
{
    FUNCTION_ENTRY( "saveFreeCall" );

    //if (m_freeCallDisplayed)
    if (FREE_CALL == m_displayedCallType)
    {
        CString tetraId = GetTetraID();

        // Check not already in list
        CDialog* pDlg = (CDialog*)AfxGetApp()->m_pMainWnd;
        ASSERT (pDlg!=NULL);

        // AJP - This needs to be redone for safe casting
        //CComboBox* combo = dynamic_cast<CComboBox*>(pDlg->GetDlgItem(IDC_SA_TETRAID));
        CComboBox* combo = (CComboBox*)pDlg->GetDlgItem(IDC_SA_TETRAID);

        //combo.SubclassDlgItem(IDC_SA_TETRAID,AfxGetApp()->m_pMainWnd);
        if (combo->FindStringExact(-1,tetraId)==CB_ERR)
        {
            // Add to list
            combo->InsertString(0,tetraId);
        }
    }

    FUNCTION_EXIT;
}


CString CSelectedCall::GetTetraID() const
{
    FUNCTION_ENTRY( "GetTetraID" );
    FUNCTION_EXIT;
    return GetControlText(IDC_SA_TETRAID);
}


CString CSelectedCall::GetID()
{
    FUNCTION_ENTRY( "GetID" );
    FUNCTION_EXIT;
    return GetControlText(IDC_SA_ID);
}


CString CSelectedCall::GetLocation()
{
    FUNCTION_ENTRY( "GetLocation" );
    FUNCTION_EXIT;
    return GetControlText(IDC_SA_LOCATION);
}


CString CSelectedCall::GetControlText(int nID) const
{
    FUNCTION_ENTRY( "GetControlText" );

    CString   controlText;

    // Get the parent for controls in the "Selected Area"
    CDialog* pDlg = (CDialog*)AfxGetApp()->m_pMainWnd;
    ASSERT (pDlg!=NULL);
    pDlg->GetDlgItemText(nID,controlText);

    FUNCTION_EXIT;
    return controlText;
}


bool CSelectedCall::IsGroup()
{
    FUNCTION_ENTRY( "IsGroup" );
    FUNCTION_EXIT;
    return m_groupCall;
}


bool CSelectedCall::IsHotLine()
{
    FUNCTION_ENTRY( "IsHotLine" );
    FUNCTION_EXIT;
    return m_isHotLine;
}


/**
  *  SetIsGroup
  *
  * Set whether or not the call is a group call
  *
  * @param bool isGroup True if the call is a group cal, false otherwise.
  */
void CSelectedCall::SetIsGroup(bool isGroup)
{
    FUNCTION_ENTRY( "SetIsGroup" );

    m_groupCall = isGroup;

    FUNCTION_EXIT;
}


bool CSelectedCall::isNonEmptyDynamicGroupSelected() const
{
    FUNCTION_ENTRY( "isNonEmptyDynamicGroupSelected" );
    FUNCTION_EXIT;
    return isDynamicGroup() &&
            TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getDynamicGroupTSIs().size() > 0;
}


bool CSelectedCall::prepareTrainDriverCall(const TA_IRS_Bus::CallTypes::ECallType callType, const TA_IRS_Bus::CallTypes::ECallPriority callPriority)
{
    FUNCTION_ENTRY( "prepareTrainDriverCall" );

    initialiseTrainAgentProxy();

    TA_IRS_Bus::CallTypes::ItsiList radioSubscribers;
    TA_IRS_Bus::CommonTypes::TrainIdList trainSubscribers;

    bool isStaticGroupCall = false;
    CString tetraId = GetTetraID();

    if ( false == isDynamicGroup() )
    {
        // this is a single or static group

        // check if it is a group
        if ( true == IsGroup() )
        {
            isStaticGroupCall = true;

            getTrainsFromStaticGroupMembers( trainSubscribers );
        }
        else
        {
            // this is not a group TSI
            // check the TSI if its a train or individual
            try
            {
				unsigned char trainID = getTrainIdFromTSI( tetraId );

				if ( trainID != 0 )
				{
					trainSubscribers.insert( trainID );
				}
				else
				{
					radioSubscribers.push_back( std::string( tetraId ) );
				}
            }
            catch ( ... )
            {
                radioSubscribers.push_back( std::string( tetraId ) );
            }
        }
    }
    else
    {
        // this is a dynamic group, members must be sorted
        separateTrainsFromDynamicGroupMembers( radioSubscribers, trainSubscribers );
    }

    // check if there are any trains involved
    if ( true == trainSubscribers.empty() )
    {
        // not a train call
        FUNCTION_EXIT;
        return false;
    }

    // now the call must be made
    try
    {
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        TA_IRS_Bus::CallTypes::DriverCallResult result;

        // determine whether to call the static group or make a DGNA
        if ( true == isStaticGroupCall )
        {
            result = m_trainDriverCallProxy->makeDriverCall( trainSubscribers,
                                                             std::string( tetraId ),
                                                             false,
                                                             callType,
                                                             callPriority,
                                                             sessionId );
        }
        else
        {
            result = m_trainDriverCallProxy->makeDriverCall( trainSubscribers,
                                                             radioSubscribers,
                                                             false,
                                                             callType,
                                                             callPriority,
                                                             sessionId );
        }

        // process result
        bool callMade = ( result.callId != 0 );
        std::ostringstream errorStream;

        for ( TA_IRS_Bus::CallTypes::VoiceCallResponseList::iterator iter = result.resultList.begin();
        iter != result.resultList.end(); ++iter )
        {
            if ( TA_IRS_Bus::CallTypes::CallTrainProceeding == iter->result )
            {
                // no error, skip
                continue;
            }

            errorStream << "PV" << std::setw(2) << std::setfill('0') << static_cast< unsigned int >( iter->trainId ) << ": ";

            switch ( iter->result )
            {
            case TA_IRS_Bus::CallTypes::CallTrainProceeding:
                // success
                break;
            case TA_IRS_Bus::CallTypes::CallTrainAudioRouteInUse:
            case TA_IRS_Bus::CallTypes::CallTrainAudioRouteInUseOptionToOverride:
                errorStream << "Train Busy" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::CallTrainReset:
                errorStream << "Call Reset" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::BadCommandBadChecksum:
            case TA_IRS_Bus::CallTypes::BadCommandUnknownCommand:
            case TA_IRS_Bus::CallTypes::BadCommandBadParameter:
            case TA_IRS_Bus::CallTypes::BadCommandUnexpectedCommand:
                errorStream << "Bad Command" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::InvalidTrainError:
                errorStream << "Access Denied" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::TransmissionFailureError:
                errorStream << "Transmission Failure" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::TransactionTimeoutError:
                errorStream << "Train Timeout" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::TransactionCancelledError:
                errorStream << "Operation Cancelled" << std::endl;
                break;
            case TA_IRS_Bus::CallTypes::MpuChangeoverError:
                errorStream << "MPU Changeover" << std::endl;
                break;
            }
        }

        // if the call was not made, or there were errors, show an error dialog
        if ( ( false == callMade ) ||
             ( false == errorStream.str().empty() ) )
        {
            // Unable to make call to train:
            // <list of trains and errors>
            // The call was <not> established

            errorStream << "The call was ";
            if ( false == callMade )
            {
                errorStream << "not established";
            }
            else
            {
                errorStream << "established successfully to remaining participants";
            }

            std::string errorString = "\n";
            errorString.append( errorStream.str() );

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << errorString;
            CString errMsg = userMsg.constructMessage( IDS_UW_632005 );

            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::AgentCommunicationException", ex.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString error = "Error while";
            userMsg << error;
            CString errMsg = userMsg.constructMessage(IDS_UW_632022);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", ex.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString error = "Access denied while";
            userMsg << error;
            CString errMsg = userMsg.constructMessage(IDS_UW_632022);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }
        catch ( TA_IRS_Bus::InvalidParameterException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::InvalidParameterException", ex.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString error = "Error while";
            userMsg << error;
            CString errMsg = userMsg.constructMessage(IDS_UW_632022);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }
        catch ( TA_IRS_Bus::GroupInUseException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::GroupInUseException", ex.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString error = "Train GOA talkgroup in use while";
            userMsg << error;
            CString errMsg = userMsg.constructMessage(IDS_UW_632022);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }
        catch ( TA_IRS_Bus::TrainRadioException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::TrainRadioException", ex.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            CString error = "Error while";
            userMsg << error;
            CString errMsg = userMsg.constructMessage(IDS_UW_632022);
            RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
        }

        FUNCTION_EXIT;
        return true;
    }


void CSelectedCall::answerDriverCall(TA_IRS_Bus::CommonTypes::TrainIdType trainId)
{
    FUNCTION_ENTRY( "answerDriverCall" );

    CWaitCursor wait;

    const std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

    initialiseTrainAgentProxy();

    // Allow exceptions to propogate
    m_trainDriverCallProxy->answerOccCallRequest( trainId, sessionId );

    FUNCTION_EXIT;
}


TA_IRS_Bus::TrainInformationTypes::TrainDetails CSelectedCall::getTrainDetailsFromTSI(const CString& tsi)
{
    FUNCTION_ENTRY( "getTrainDetailsFromTSI" );
    FUNCTION_EXIT;
    return TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getTrainDetailsFromTSI( tsi );
}


TA_IRS_Bus::CommonTypes::TrainIdType CSelectedCall::getTrainIdFromTSI(const CString& tsi)
{
    FUNCTION_ENTRY( "getTrainIdFromTSI" );

    std::string properString = const_cast<CString&>(tsi).GetBuffer(0);
    FUNCTION_EXIT;
    return TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getTrainIdForTsi( properString );
}


CString CSelectedCall::getCorrectedTetraId() const
{
    FUNCTION_ENTRY( "getCorrectedTetraId" );
    FUNCTION_EXIT;
    return getCorrectedTetraId(GetTetraID());
}


void CSelectedCall::processTrainRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& event )
{
    FUNCTION_ENTRY( "processTrainRadioGroup" );
    
    TA_THREADGUARD( m_radioGroupsLock );

    m_radioGroups[ event.groupTsi ] = event;
    
    FUNCTION_EXIT;
}


CString CSelectedCall::getCorrectedTetraId(const CString& tetraId, bool* isTrainTetraId) const
{
    FUNCTION_ENTRY( "getCorrectedTetraId" );

    try
    {
        TA_IRS_Bus::TrainInformationTypes::TrainDetails details = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getTrainDetailsFromTSI( tetraId );

        if (0 != isTrainTetraId)
        {
            *isTrainTetraId = true;
        }

        CString adjustedTsi = details.primaryTsi.c_str();

        FUNCTION_EXIT;
        return adjustedTsi;
    }
    catch( ... )
    {
        // valid, not all TSIs will be from trains
    }

    if (0 != isTrainTetraId)
    {
        *isTrainTetraId = false;
    }

    FUNCTION_EXIT;
    return tetraId;
}


void CSelectedCall::setHotLine(bool isHotLine)
{
    FUNCTION_ENTRY( "setHotLine" );

    m_isHotLine = isHotLine;

    FUNCTION_EXIT;
}


void CSelectedCall::separateTrainsFromDynamicGroupMembers(TA_IRS_Bus::CallTypes::ItsiList& radioSubscribers, TA_IRS_Bus::CommonTypes::TrainIdList& trainSubscribers)
{
    FUNCTION_ENTRY( "separateTrainsFromDynamicGroupMembers" );

    // gets the TSIs in the dynamic group, then separates into trains and non trains

    radioSubscribers.clear();
    trainSubscribers.clear();

    TA_IRS_Bus::CSearchPage* searchPage = TA_IRS_App::SearchPageHelper::getInstance().getSearchPage();

    std::vector<CString> tsiList = searchPage->getDynamicGroupTSIs();

    for ( std::vector<CString>::iterator iter = tsiList.begin();
          tsiList.end() != iter; ++iter )
    {
        // check if the TSI is a train or an individual and add to the correct list
        try
        {
			unsigned char trainID = getTrainIdFromTSI( *iter );

			if ( trainID != 0 )
			{
				trainSubscribers.insert( trainID );
			}
			else
			{
				radioSubscribers.push_back( std::string( iter->GetBuffer(0) ) );
			}
        }
        catch ( ... )
        {
            radioSubscribers.push_back( std::string( iter->GetBuffer(0) ) );
        }
    }

    FUNCTION_EXIT;
}


void CSelectedCall::initialiseTrainAgentProxy()
{
    FUNCTION_ENTRY( "initialiseTrainAgentProxy" );

    //if m_trainDriverCallProxy is not initialised
    //get the session ID
    //initialise the proxy factory in case it isnt already
    //get the call driver proxy


    if ( !m_trainDriverCallProxy )
    {
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        
        TA_IRS_Bus::TrainAgentProxyFactory& trainAgentProxyFactory = TA_IRS_Bus::TrainAgentProxyFactory::getInstance();
        trainAgentProxyFactory.guiInitialise( sessionId );

        m_trainDriverCallProxy = trainAgentProxyFactory.createTrainDriverCallCorbaProxy();
        m_trainRadioGroupCorbaProxy = trainAgentProxyFactory.createTrainRadioGroupCorbaProxy();

        m_trainRadioGroupCorbaProxy->addObserver( this );

        try
        {
            TA_THREADGUARD( m_radioGroupsLock );

            m_radioGroups = m_trainRadioGroupCorbaProxy->getTrainRadioGroups( 0 );
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

    }

    FUNCTION_EXIT;
}


void CSelectedCall::getTrainsFromStaticGroupMembers(TA_IRS_Bus::CommonTypes::TrainIdList& trainSubscribers)
{
    FUNCTION_ENTRY( "getTrainsFromStaticGroupMembers" );


    // TODO later on this can be made more efficient if the search page caches some of it...

    trainSubscribers.clear();

    try
    {
        // Get the parent class and retrieve the reference to the radio manager
        CRadioManagerDialog* manager = dynamic_cast< CRadioManagerDialog* >( AfxGetApp()->GetMainWnd() );

        TA_Base_Bus::IRadioCorbaDef::SubscriberList_var groupMembers;

        // get the group members
        CORBA_CALL_RETURN( groupMembers, manager->getRadioCorbaDef(), getGroupMembers, ( GetTetraID() ) );//limin

        std::string groupTsi = GetTetraID();

        TA_THREADGUARD( m_radioGroupsLock );

        // if its a GOA group
        // then the group members must be filtered based on coverage
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_radioGroups.find( groupTsi );

        if ( m_radioGroups.end() != findIter )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Calling a GOA group %s",
                         GetTetraID() );

            // for each group member
            for ( unsigned int i = 0; i < groupMembers->length(); ++i )
            {
                try
                {
                    // get the train details
                    TA_IRS_Bus::TrainInformationTypes::TrainDetails details =
                        TA_IRS_App::SearchPageHelper::getInstance().getSearchPage()->getTrainDetailsFromTSI( groupMembers[i].TSI.in() );

                    // check if the train is valid or not
                    if ( ( false == details.atsValid ) &&
                         ( false == details.communicationValid ) )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                     "Train %d is not included in the call to the group, it is not valid",
                                     details.trainId );
                    }
                    else
                    {
                        // get the train's location
                        unsigned long trainLocation = 0;

                        // use ATS info if ats is valid
                        if ( true == details.atsValid )
                        {
                            trainLocation = details.currentLocation;
                        }
                        // fallback to communicating location if comms are valid
                        else if ( TA_IRS_Bus::LocationCache::getInstance().getOccLocationKey() != details.communicatingLocation )
                        {
                            trainLocation = details.communicatingLocation;
                        }

                        // if the train location is not known,
                        // or in the coverage, include it
                        if ( ( 0 == trainLocation ) ||
                             ( findIter->second.locationCoverage.end() != findIter->second.locationCoverage.find( trainLocation ) ) )
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                         "Train %d is included in the call to the group, it is covered in the location",
                                         details.trainId );

                            trainSubscribers.insert( details.trainId );
                        }
                        else
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                         "Train %d is not included in the call to the group, it is not covered in the location",
                                         details.trainId );
                        }
                    }
                }
                catch( ... )
                {
                    // valid, not all TSIs will be from trains
                }
            }
        }
        else // not a GOA group, add all members
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Calling a static group %s",
                         GetTetraID() );

            // for each group member
            for ( unsigned int i = 0; i < groupMembers->length(); ++i )
            {
                // check if its a train, and if it is insert into the list
                try
                {
					unsigned char trainID = getTrainIdFromTSI( groupMembers[i].TSI.in() );

					if ( trainID != 0 )
					{
						trainSubscribers.insert( trainID );
					}
                }
                catch ( ... )
                {
                }
            }
        }
    }
    catch(...)
    {
    }

    FUNCTION_EXIT;
}


