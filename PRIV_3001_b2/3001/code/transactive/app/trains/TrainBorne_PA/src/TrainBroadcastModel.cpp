/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainBroadcastModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainBroadcastModel models behaviour for the train broadcast GUI tab of the PA Manager GUI
  */

#include "app/trains/TrainBorne_PA/src/stdafx.h"

#pragma warning ( disable : 4267 4312 4311 )
//#include "app/trains/TrainBorne_PA/src/BroadcastParametersDlg.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/PAErrorHandler.h"
#include "app/trains/TrainBorne_PA/src/ITrainBroadcastPage.h"
#include "app/trains/TrainBorne_PA/src/TrainBroadcastPage.h"
//#include "app/trains/TrainBorne_PA/src/PaManagerModel.h"
//#include "app/trains/TrainBorne_PA/src/PaRightsManager.h"
#include "app/trains/TrainBorne_PA/src/TrainBroadcastModel.h"
#include "app/trains/TrainBorne_PA/src/TrainDvaStatusModel.h"
#include "app/trains/TrainBorne_PA/src/PaRightsManager.h"
//#include "app/trains/TrainBorne_PA/src/pamanager.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"

#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/exceptions/src/ValueNotSetException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdlib.h>

#pragma warning ( disable : 4267 4312 4311 )


TrainBroadcastModel::TrainBroadcastModel( ITrainBroadcastPage& pageView)
    : GenericBroadcastModel( pageView ),
      m_trainBroadcastPage( pageView ),
      m_inboundUpdateBuffer( WM_PA_TRAIN_INBOUND_LIST_UPDATE ),
      m_outboundUpdateBuffer( WM_PA_TRAIN_OUTBOUND_LIST_UPDATE ),
      m_duration( 0 ),
      m_period( 0 ),
      m_cyclicalEnabled( false ),
	  m_trainSelectionSubscriber(NULL),
      m_hasOverrideOption( false ),
      m_selectedStaticGroup( "" ),
	  m_bEmergency( false ),
	  m_selectedDvaMessageText("")
{
    FUNCTION_ENTRY( "TrainBroadcastModel" );
    FUNCTION_EXIT;
}


TrainBroadcastModel::~TrainBroadcastModel()
{
    FUNCTION_ENTRY( "~TrainBroadcastModel" );

    TA_ASSERT( ::IsWindow( getPageHwnd() ), "Window no longer valid while in destructor" );

    TA_ASSERT( 0 == m_trainSelectionSubscriber, "Subscriber should be destroyed in enableUpdateListener" );

    FUNCTION_EXIT;
}


void TrainBroadcastModel::attemptTrainSubscriberConnection()
{
    FUNCTION_ENTRY( "attemptTrainSubscriberConnection" );

    //TA_ASSERT( 0 == m_trainSelectionSubscriber, "Object already exists" );

    //unsigned long locationKey = CachedConfig::getInstance()->getEntityLocationKey();

    try
    {
        // Add TA_TrainSelectionComponent, and dependencies on this and
        // TrainAgentCorbaDef (which already exists in build all workspace)
        //m_trainSelectionSubscriber = new TA_IRS_Bus::TrainSelector( *this, locationKey, m_currentStationMode );
		//(dynamic_cast<TrainBroadcastPage*>(&m_trainBroadcastPage))->getTrainSelector().updateGUITrainRadioGroups();
    }
    catch ( const TA_Base_Core::DatabaseException& )
    {
        PaErrorHandler::displayError( PaErrorHandler::DATABASE_ERROR );
    }
    catch ( const TA_Base_Core::DataException& )
    {
        PaErrorHandler::displayError( PaErrorHandler::INVALID_CONFIG_DATA );
    }
    catch ( const TA_Base_Core::TransactiveException& )
    {
        // Usually ObjectResolutionNotSet error
        PaErrorHandler::displayError( PaErrorHandler::INVALID_CONFIG_DATA );
    }
    catch ( ... )
    {
        // bug fix: manager should be able to run without train agent
    }

    FUNCTION_EXIT;
}


bool TrainBroadcastModel::isTrainSubscriberConnected() const
{
    FUNCTION_ENTRY( "isTrainSubscriberConnected" );

    // If object exists, assume it's connected
    FUNCTION_EXIT;
    return 0 != m_trainSelectionSubscriber;
}


void TrainBroadcastModel::enableUpdateListener( bool bEnable )
{
    FUNCTION_ENTRY( "enableUpdateListener" );

    // Should only be called while the page window is valid (OnInitDialog/DestroyWindow)
    TA_ASSERT( getPageHwnd() != 0, "Internal check failed: Incorrect startup/shutdown ordering" );

    if ( bEnable )
    {
        // Only attempt to subscribe if not already subscribed
		//TA_ASSERT(!isTrainSubscriberConnected(), "Internal check failed: m_trainSelectionSubscriber == NULL");
        if ( !isTrainSubscriberConnected() )
        {
            attemptTrainSubscriberConnection();
        }
    }
    else
    {
        // Shut down listener
        if ( m_trainSelectionSubscriber )
        {
            //delete m_trainSelectionSubscriber;
            m_trainSelectionSubscriber = 0;
        }
    }

    FUNCTION_EXIT;
}


void TrainBroadcastModel::updateInboundList( TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                                             const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails )
{
    FUNCTION_ENTRY( "updateInboundList" );

    // Removed [the debug/testing trainselector causes this assert to trip]
    //TA_ASSERT(trainDetails.isInbound, "Updating in bound list with outbound train");

    m_inboundUpdateBuffer.bufferUpdate( getPageHwnd(), action, trainDetails );

    FUNCTION_EXIT;
}


void TrainBroadcastModel::updateOutboundList( TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                                              const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails )
{
    FUNCTION_ENTRY( "updateOutboundList" );

    // Removed [the debug/testing trainselector causes this assert to trip]
    //TA_ASSERT(!trainDetails.isInbound, "Updating outbound list with inbound train");

    m_outboundUpdateBuffer.bufferUpdate( getPageHwnd(), action, trainDetails );

    FUNCTION_EXIT;
}


TrainListUpdateContainer::BufferedItem TrainBroadcastModel::fetchUpdatedInboundTrainData()
{
    FUNCTION_ENTRY( "fetchUpdatedInboundTrainData" );
    FUNCTION_EXIT;
    return m_inboundUpdateBuffer.fetchUpdate( getPageHwnd() );
}


TrainListUpdateContainer::BufferedItem TrainBroadcastModel::fetchUpdatedOutboundTrainData()
{
    FUNCTION_ENTRY( "fetchUpdatedOutboundTrainData" );
    FUNCTION_EXIT;
    return m_outboundUpdateBuffer.fetchUpdate( getPageHwnd() );
}


void TrainBroadcastModel::updateSelectionCombo( const std::vector<std::string>& trainListNames )
{
    FUNCTION_ENTRY( "updateSelectionCombo" );

    // Post notification of new list group data - only need to store latest update
    TA_Base_Core::ThreadGuard sync  ( m_dynamicGroupListLock );

    m_dynamicGroupList = trainListNames;

    // Change notification sent out to window
    ::PostMessage( getPageHwnd(), WM_PA_TRAIN_DYN_LIST_UPDATE, 0, 0 );

    FUNCTION_EXIT;
}


std::vector<std::string> TrainBroadcastModel::fetchDynamicGroupList()
{
    FUNCTION_ENTRY( "fetchDynamicGroupList" );

    TA_Base_Core::ThreadGuard sync  ( m_dynamicGroupListLock );

    FUNCTION_EXIT;
    return m_dynamicGroupList;
}


HWND TrainBroadcastModel::getPageHwnd() const
{
    FUNCTION_ENTRY( "getPageHwnd" );

    // Shouldn't be requesting handle if page no longer exists
    TA_ASSERT( m_trainBroadcastPage.getWindow(), "Page no longer exists" );
    TA_ASSERT( m_trainBroadcastPage.getWindow()->m_hWnd != 0, "Page no longer exists" );

    FUNCTION_EXIT;
    return m_trainBroadcastPage.getWindow()->m_hWnd;
}


void TrainBroadcastModel::unsetBroadcastType()
{
    FUNCTION_ENTRY( "unsetBroadcastType" );

    ThreadGuard sync( m_lock );
    m_broadcastType.resetValue();

    FUNCTION_EXIT;
}


void TrainBroadcastModel::setBroadcastType( TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType )
{
    FUNCTION_ENTRY( "setBroadcastType" );

    ThreadGuard sync( m_lock );
    m_broadcastType.setValue( broadcastType );

    FUNCTION_EXIT;
}


void TrainBroadcastModel::executeBroadcast( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trainCoverage )
{
    FUNCTION_ENTRY( "executeBroadcast" );

    m_trainCoverage = trainCoverage;

    // Make cursor into hour glass for duration of the execution
    // Note: If the function succeeds and brings up the new dialog,
    // the cursor will automatically be restored to normal..
    CWaitCursor hourGlassCursor;

    try
    {
        switch ( m_broadcastType.getValue() )
        {
            case TA_Base_Bus::IPAAgentCorbaDef::TrainDva:
                executeDva();
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::TrainLive:
                executeLive();
                break;
            default:
                TA_ASSERT( false, "Unrecognised BroadcastType" );
                break;
        }
    }
    catch ( const TA_Base_Core::InvalidPasConfigurationException& )
    {
        PaErrorHandler::displayError( PaErrorHandler::BROADCAST_TRAIN_VERSION_FAILURE );
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached:
            PaErrorHandler::displayError(PaErrorHandler::MAXIMUM_BROADCAST_COUNT_REACHED);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        default:
            break;
        }
    }
    catch ( const CORBA::Exception& )
    {
        PaErrorHandler::displayError( PaErrorHandler::PA_AGENT_FAIL );
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }

    FUNCTION_EXIT;
}


void TrainBroadcastModel::executeDva()
{
    FUNCTION_ENTRY( "executeDva" );
	
	static const CString title("Execute Dva Broadcast");

    TA_Base_Bus::TransActiveMessage userMsg;

    // Convert a TCHAR string to a LPCSTR
    CT2CA pszConvertedAnsiString (m_selectedDvaMessageText);

    // construct a std::string using the LPCSTR input
    std::string msgContent (pszConvertedAnsiString.m_psz);

    userMsg << msgContent;
    if (IDNO == userMsg.showMsgBox(IDS_UW_611004))
    {
        return;
    }

    std::string broadcastId = "";

    {
        ThreadGuard lockBroadcastDva( m_lock );

        //short libraryVersion            = 0;
        unsigned long libraryVersion = getTrainDvalibraryVersion();

        TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams timeInterval = { m_period, m_duration };

        if (!m_cyclicalEnabled)
        {
            // Not a cyclical broadcast, so cannot set start / stop time
            timeInterval.periodInSecs = 0;
            timeInterval.durationInSecs = 0;
        }

        //if ( !PaRightsManager::getInstance().hasRight( PaRightsManager::TRAIN_PA_PRERECORDED_ANNOUNCEMENT ) )
        //{
        //    PaErrorHandler::displayError( PaErrorHandler::INSUFFICIENT_RIGHTS );

        //    FUNCTION_EXIT;
        //    return;
        //}

	    // Update CORBA_CALL_RETURN parameter add m_bEmergency
        CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), broadcastTrainDva, ( m_trainCoverage,
                                                                                                    static_cast<unsigned long>(m_selectedMessage.getValue()),
                                                                                                    libraryVersion,
                                                                                                    timeInterval,
                                                                                                    m_hasOverrideOption,
                                                                                                    CachedConfig::getInstance()->getSessionId().c_str(),
																								    m_bEmergency) );
        // We must release the model lock before displaying broadcast - since
        // there may be interactions while the TrainBroadcastPage modal dialog displayed
    }

    // Bring up the status dialog
    m_trainBroadcastPage.displayDvaStatusDialog( broadcastId );

    FUNCTION_EXIT;
}


void TrainBroadcastModel::executeLive()
{
    FUNCTION_ENTRY( "executeLive" );

    //if ( !PaRightsManager::getInstance().hasRightToBroadcastTrainLive() )
    //{
    //    PaErrorHandler::displayError( PaErrorHandler::INSUFFICIENT_RIGHTS );

    //    FUNCTION_EXIT;
    //    return;
    //}
    TA_Base_Bus::TransActiveMessage userMsg;
    if (IDNO == userMsg.showMsgBox(IDS_UW_611005))
    {
        return;
    }

    // find the static group TSI from the name
    std::string groupTsi = "";

    try
    {
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup selectedGroup = m_trainSelectionSubscriber->getRadioGroupForName( m_selectedStaticGroup );

        groupTsi = selectedGroup.groupTsi;
    }
    catch( TA_IRS_Bus::InvalidParameterException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );
    }

    std::string broadcastId;
    {
        // We must release the model lock before displayLiveStatusDialog - since
        // there may be interactions while the TrainBroadcastPage modal dialog displayed
        // (so generate lock within this scope so it's destroyed)
        ThreadGuard sync( m_lock );

        CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), broadcastTrainLive, ( m_trainCoverage,
                                                                                         m_hasOverrideOption,
                                                                                         groupTsi.c_str(),
                                                                                         CachedConfig::getInstance()->getSessionId().c_str() ) );

    }

    // Bring up the status dialog
    m_trainBroadcastPage.displayLiveStatusDialog( broadcastId );

    FUNCTION_EXIT;
}


void TrainBroadcastModel::setSelectedMessage( int id )
{
    FUNCTION_ENTRY( "setSelectedMessage" );

    TA_Base_Core::ThreadGuard sync  ( m_lock );
    if ( id >= 0 )
    {
        m_selectedMessage.setValue( id );
    }
    else
    {
        m_selectedMessage.resetValue();
    }

    FUNCTION_EXIT;
}


void TrainBroadcastModel::setDuration( time_t start )
{
    FUNCTION_ENTRY( "setDuration" );

    TA_Base_Core::ThreadGuard sync  ( m_lock );
    m_duration = start;

    FUNCTION_EXIT;
}


void TrainBroadcastModel::setPeriod( time_t stop )
{
    FUNCTION_ENTRY( "setPeriod" );

    TA_Base_Core::ThreadGuard sync  ( m_lock );
    m_period = stop;

    FUNCTION_EXIT;
}


void TrainBroadcastModel::setCyclicalEnabled( bool enabled )
{
    FUNCTION_ENTRY( "setCyclicalEnabled" );

    TA_Base_Core::ThreadGuard sync  ( m_lock );
    m_cyclicalEnabled = enabled;

    FUNCTION_EXIT;
}


const std::string TrainBroadcastModel::getSessionId() const
{
    FUNCTION_ENTRY( "getSessionId" );
    FUNCTION_EXIT;
    return CachedConfig::getInstance()->getSessionId();
}


TA_IRS_Bus::TrainDvaMessageRecords TrainBroadcastModel::getMessages( const std::string& filterKeyword )
{
    FUNCTION_ENTRY( "getMessages" );
    FUNCTION_EXIT;
    return TA_IRS_Bus::CachedMaps::getInstance()->getTrainDvaMessageRecords( filterKeyword );
}


bool TrainBroadcastModel::getHasOverrideOption()
{
    FUNCTION_ENTRY( "getHasOverrideOption" );
    FUNCTION_EXIT;
    return m_hasOverrideOption;
}


void TrainBroadcastModel::setSelectedStaticGroup( const std::string& groupName )
{
    FUNCTION_ENTRY( "setSelectedStaticGroup" );

    m_selectedStaticGroup = groupName;

    FUNCTION_EXIT;
}


void TrainBroadcastModel::updateFallbackStatus(bool inFallback)
{
    FUNCTION_ENTRY( "updateFallbackStatus" );

	//Fix : DTL-1487
    //m_trainBroadcastPage.getTrainSelectorGUI().updateFallbackStatus( inFallback );

    FUNCTION_EXIT;
}


bool TrainBroadcastModel::hasRadioGroupSupport()
{
    FUNCTION_ENTRY( "hasRadioGroupSupport" );
    FUNCTION_EXIT;
    return m_trainBroadcastPage.getTrainSelectorGUI().hasRadioGroupSupport();
}


void TrainBroadcastModel::updateRadioGroupCombo(const std::vector<std::string>& trainListNames)
{
    FUNCTION_ENTRY( "updateRadioGroupCombo" );

	m_trainBroadcastPage.updateTrainLiveStaticGroupNames( trainListNames );
    //m_trainBroadcastPage.getTrainSelectorGUI().updateRadioGroupCombo( trainListNames );

    FUNCTION_EXIT;
}


TA_IRS_Bus::TrainInformationTypes::TrainDetailsList TrainBroadcastModel::getSelectedTrainData() const
{
    FUNCTION_ENTRY( "getSelectedTrainData" );
    FUNCTION_EXIT;
    return m_trainBroadcastPage.getTrainSelectorGUI().getSelectedTrainData();
}


bool TrainBroadcastModel::testCanObtainLock()
{
    FUNCTION_ENTRY( "testCanObtainLock" );

    TA_Base_Core::ThreadGuard sync  ( m_lock );

    FUNCTION_EXIT;
    return true;
}

void TrainBroadcastModel::initializeStatusNameSubscriber( HWND hWnd )
{
	PaStatusNameChangeSubscriber::getInstance().addListenerWindow(hWnd);
}

std::vector<std::string> TrainBroadcastModel::getTrainDvaBroadcastZones()
{
	return TrainBroadcastModel::getDvaBroadcastZones(CachedConfig::getInstance()->getAgentObject(), 
		TA_Base_Bus::IPAAgentCorbaDef::TrainDva,
		USE_RIGHTS_CHECKING);
}

std::vector<std::string> TrainBroadcastModel::getDvaBroadcastZones( PAAgentNamedObject& paAgent, const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType& filter, ERightsCheckingMode filterByRightToView /*= USE_RIGHTS_CHECKING*/ )
{

	CWaitCursor hourGlassCursor;

	std::vector<std::string>    result;
	TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* broadcastIds = NULL;

	try
	{
		CORBA_CALL_RETURN( broadcastIds, paAgent, getBroadcastIdTypes, () );//limin
	}
	catch (const TA_Base_Core::ObjectResolutionException&)
	{
		throw;
	}
	catch(const CORBA::Exception&)
	{
		// Some unspecific error occurred with CORBA servant, just means an empty array
		// (reduced functionality error)
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
		return result;
	}

	TA_ASSERT(broadcastIds != NULL, "broadcastIds NULL");


	for (unsigned int i = 0; i < broadcastIds->length(); i ++)
	{
		const TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdType& item = (*broadcastIds)[i];

		if (filter == item.broadcastType)
		{
			bool placeInOutput = false;

			// We can only add those items we have a right to display - unless rights checking has been override
			if (IGNORE_RIGHTS_CHECKING == filterByRightToView)
			{
				placeInOutput = true;   
			}
			else if (PaRightsManager::getInstance().hasRightToDisplayBroadcastWithSessionId(
				std::string(item.sessionId.in()) ) )
			{
				// Else not ignoring rights checking, only place in output if appropriate rights present
				placeInOutput = true;
			}   

			if (placeInOutput)
			{
				result.push_back(std::string(item.broadcastId.in()));
			}     
		}
	}

	// Clean up
	delete broadcastIds;
	broadcastIds = 0;

	return result;
}

bool TrainBroadcastModel::confirmStatusNameExists( const std::string& broadcastId )
{
	try
	{        
		CWaitCursor hourGlassCursor;

		TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig_var var = 
			CachedConfig::getInstance()->getAgentObject()->getBroadcastConfig(broadcastId.c_str());        

		return true;
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            PaErrorHandler::displayError(PaErrorHandler::BROADCAST_ID_NOT_FOUND);
            break;
        default:
            break;
        }
    }
	catch (...)
	{
		PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
    }

    return false;
}

int TrainBroadcastModel::terminateAndRemoveBroadcastIds( const std::vector<std::string>& broadcastIds )
{
	return terminateAndRemoveBroadcastIds(broadcastIds, CachedConfig::getInstance()->getAgentObject());
}

int TrainBroadcastModel::terminateAndRemoveBroadcastIds( const std::vector<std::string>& broadcastIds, PAAgentNamedObject& paAgent, EBroadcastOrigin origin /*= BO_UNKNOWN*/ )
{
	CWaitCursor hourGlassCursor;

	int numberOfNonRemovals = 0;

	// Progressively go through and kill each specified broadcast id
	for (int i = 0; i < broadcastIds.size(); i ++)
	{
		try
		{             
			bool ignoreRightsChecking = false;

			// TD #3152
			// There is a subset of broadcast types that can be cleared without
			// regard for operator priveleges (because they were created locally, and
			// are effectively internal to the system - transparent to user) - don't
			// want such broadcasts to pop up errors saying insufficient priveleges
			switch (origin)
			{
			case BO_RECORDING_BROADCAST:    // FALLTHROUGH
			case BO_LIVE_BROADCAST:         // FALLTHROUGH
			case BO_PLAYBACK_BROADCAST: 
				ignoreRightsChecking = true;
				break;
			};

			// Technically if we don't have a right to clear a broadcast, we are
			// still allowed to terminate it (just not remove it) - but it makes
			// sense for this function to do neither if that privelege is not present

			// TES #703: Cannot terminate broadcasts that are currently in progress
			bool allowedToClearBroadcast = PaRightsManager::getInstance().hasRightToClearBroadcast(
				broadcastIds[i].c_str(), paAgent) &&
				!PaRightsManager::getInstance().isBroadcastInProgress(
				broadcastIds[i].c_str(), paAgent);

			if (ignoreRightsChecking || allowedToClearBroadcast)                    
			{
				CORBA_CALL( paAgent, terminateBroadcast, ( broadcastIds[i].c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );
				CORBA_CALL( paAgent, removeBroadcast, ( broadcastIds[i].c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );
			}
			else
			{
				PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
			}
        }
        catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
            switch ( expWhat.eErrorId )
            {
            case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
                // This broadcast ID already terminated, ignore exception & continue
                // (this is not an error)
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
                // The removeBroadcast failed, which indicates terminateBroadcast
                // didn't work correctly (perhaps another user initiated
                // the broadcast again) - we just don't remove it from the list

                // There is a known issue where broadcast does not complete immediately
                // so it's quite likely for this error to occur (at this point in time)
            case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
                // No privilege to remove this one
                numberOfNonRemovals ++;
                break;
            default:
                break;
            }
        }
	}

	return numberOfNonRemovals;
}

TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType TrainBroadcastModel::collectStatusNameChange( DWORD id, HWND hWnd, std::string& from, std::string& to )
{
	const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData nameChange = 
		PaStatusNameChangeSubscriber::getInstance().getMessageData(id, hWnd);

	from = std::string(nameChange.fromBroadcastId);
	to = std::string(nameChange.toBroadcastId);
	return nameChange.broadcastType;
}

void TrainBroadcastModel::setSelectedDvaMessageText( const CString& msgText )
{
	m_selectedDvaMessageText = msgText;
}

unsigned long TrainBroadcastModel::getTrainDvalibraryVersion() throw (TA_Base_Core::InvalidPasConfigurationException)
{
    try
    {
        return TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion();
    }
    catch ( const TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
        TA_THROW( TA_Base_Core::InvalidPasConfigurationException() );
    }
}

void TrainBroadcastModel::restoreBroadcast(const std::string& broadcastId)
{
    try
    {
        CORBA_CALL(CachedConfig::getInstance()->getAgentObject(), restoreBroadcast, (broadcastId.c_str(), ""));
    }
    catch (const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in());
    }
    catch (const TA_Base_Core::ObjectResolutionException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", expWhat.what());
    }
    catch (const CORBA::Exception&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", "Agent fail");
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception when removeAnnouncement");
    }
}
