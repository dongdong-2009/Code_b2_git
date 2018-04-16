/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "app/signs/tis_agent/src/TrainAgentSubscriber.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <iomanip>
#include <iostream>

namespace TA_IRS_App
{
    const std::string TrainAgentSubscriber::SUCCESS_STATUS = "Successful";
    const std::string TrainAgentSubscriber::TTIS_IN_USE_STATUS = "Busy"; //In Use
    const std::string TrainAgentSubscriber::TTIS_RESET_STATUS = "Reset";
    const std::string TrainAgentSubscriber::TTIS_FAILED_STATUS = "TTIS Failed";
    const std::string TrainAgentSubscriber::TTIS_DIFFERENT_VERSION_STATUS = "Version Mismatch"; //Different Version Number
    const std::string TrainAgentSubscriber::INVALID_TRAIN_ERROR_STATUS = "Access Denied";
    const std::string TrainAgentSubscriber::TRANSMISSION_FAILURE_ERROR_STATUS = "Transmission Failure";
    const std::string TrainAgentSubscriber::TRANSACTION_TIMEOUT_ERROR_STATUS = "Time-out";
    const std::string TrainAgentSubscriber::TRANSACTION_CANCELLED_ERROR_STATUS = "Cancelled";
    const std::string TrainAgentSubscriber::BAD_COMMAND_BAD_CHECKSUM_STATUS = "Bad Checksum";
    const std::string TrainAgentSubscriber::BAD_COMMAND_UNKNOWN_COMMAND_STATUS = "Unknown Command";
    const std::string TrainAgentSubscriber::BAD_COMMAND_BAD_PARAMETER_STATUS = "Bad Parameter";
    const std::string TrainAgentSubscriber::BAD_COMMAND_UNEXPECTED_COMMAND_STATUS = "Unexpected Command";
    const std::string TrainAgentSubscriber::MPU_CHANGEOVER_ERROR_STATUS = "Failed due to changeover";
    const std::string TrainAgentSubscriber::TRANSFER_FAILED_STATUS = "Transfer Failed";
    const std::string TrainAgentSubscriber::TIMED_OUT_STATUS = "TimedOut";
    const std::string TrainAgentSubscriber::TIMS_FAILED_STATUS = "TIMS Failed";
	const std::string TrainAgentSubscriber::TTIS_TRAIN_FAILED_STATUS = "Failed train"; //Failed, rejected by the train
	
	const std::string TrainAgentSubscriber::TRAIN_LIBRARY_DOWNLOAD_SUCCESS_STATUS = "Completed";
	const std::string TrainAgentSubscriber::TRAIN_LIBRARY_UPGRADE_SUCCESS_STATUS = "Upgraded";
	const std::string TrainAgentSubscriber::TRAIN_LIBRARY_DOWNLOAD_TIME_OUT = "Time-out";


    TrainAgentSubscriber::TrainAgentSubscriber(TA_IRS_App::ITrainEventCallback& callback)
        : m_callback(callback)
    {
        FUNCTION_ENTRY( "TrainAgentSubscriber" );

        //Get an instance of the train proxy, and
        //subscribe to TIS updates
        //query for the current versions and
        //update the callback


        m_trainInformationCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();
		m_trainInformationCorbaProxy->addObserver( this );
        m_trainTisCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainTisCorbaProxy();
        m_trainTisCorbaProxy->addObserver( this );

        initialiseInitialVersions();

        FUNCTION_EXIT;
    }


    TrainAgentSubscriber::~TrainAgentSubscriber()
    {
        FUNCTION_ENTRY( "~TrainAgentSubscriber" );

        //deregister for TIS updates
        m_trainTisCorbaProxy->removeObserver( this );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails TrainAgentSubscriber::getTrainDetails(const TA_IRS_Bus::CommonTypes::TrainIdType& trainId)
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        //find the train in the cached map
        //return it
        //
        //throw an exception if not found


        TA_THREADGUARD( m_trainDetailsLock );

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetails.find( trainId );

        if ( findIter == m_trainDetails.end() || findIter->second.serviceNumber.find("/_") != std::string::npos)
		{		
			reloadTrainsDetails();		
		}

		findIter = m_trainDetails.find( trainId );
        if ( findIter == m_trainDetails.end() )
        {
            std::ostringstream msg;
            msg << "Train " << static_cast<unsigned long>( trainId ) << " not found";

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException( msg.str().c_str(), trainId );
        }

        FUNCTION_EXIT;
        return findIter->second;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap TrainAgentSubscriber::getTrainList()
    {
        FUNCTION_ENTRY( "getTrainList" );

        //return all cached trains


        TA_THREADGUARD( m_trainDetailsLock );

        FUNCTION_EXIT;
        return m_trainDetails;
    }


    bool TrainAgentSubscriber::isTrainUnderAgentAreaControl(TA_IRS_Bus::CommonTypes::TrainIdType trainId)
    {
        FUNCTION_ENTRY( "isTrainUnderAgentAreaControl" );
        FUNCTION_EXIT;
        return isTrainUnderAgentAreaControl( getTrainDetails( trainId ) );
    }


    bool TrainAgentSubscriber::isTrainUnderAgentAreaControl(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "isTrainUnderAgentAreaControl" );

        TA_IRS_Bus::LocationCache& locationcache = TA_IRS_Bus::LocationCache::getInstance();
        unsigned long agentLocation = locationcache.getLocationKey();
        TA_Base_Core::ILocation::ELocationType agentLocationType = locationcache.getLocationTypeByKey( agentLocation );

        if ( agentLocationType == TA_Base_Core::ILocation::OCC )
        {
            agentLocationType = TA_Base_Core::ILocation::STATION;
        }

        // train is valid for us if its in our area or communicating with this agent
        FUNCTION_EXIT;
        return ( ( true == trainDetails.atsValid) &&
                 ( true == trainDetails.isLocalisationValid ) &&
                 ( locationcache.getLocationTypeByKey( trainDetails.currentLocation ) == agentLocationType) ) ||
               ( ( true == trainDetails.communicationValid ) &&
                 ( agentLocation == trainDetails.communicatingLocation ) );
    }


    void TrainAgentSubscriber::processTisLibraryVersionUpdate(const TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate& event)
    {
        FUNCTION_ENTRY( "processTisLibraryVersionUpdate" );

        //Simply notify the callback of the new
        //details with those from the event

        m_callback.updateTrainLibraryVersions( event.trainId, event.libraryVersion, event.scheduleVersion );

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::processTisLibraryUpdate(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate&  event)
    {
        FUNCTION_ENTRY( "processTisLibraryUpdate" );

		// Only OCC need process the train library download or upgrade status change message
        TA_IRS_Bus::LocationCache& locationcache = TA_IRS_Bus::LocationCache::getInstance();
        unsigned long agentLocation = locationcache.getLocationKey();
        TA_Base_Core::ILocation::ELocationType agentLocationType = locationcache.getLocationTypeByKey( agentLocation );
		if ( agentLocationType != TA_Base_Core::ILocation::OCC && agentLocationType != TA_Base_Core::ILocation::DPT)
        {
			FUNCTION_EXIT;
			return;
        }
        //Based on the event type, notify the
        //callback of a download start,
        //completion, or upgrade

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"trainId[%d], updatetype[%d], commandStatus[%d], downloadStatus[%d], version[%d]",
			event.trainId, event.updateType,
			event.commandStatus, event.downloadResult, event.version);

        TA_IRS_App::ITrainEventCallback::ETrainLibraryType libraryType;

        switch ( event.updateType )
        {
        case TA_IRS_Bus::TtisTypes::LibraryDownloadStart:
        case TA_IRS_Bus::TtisTypes::LibraryDownloadComplete:
        case TA_IRS_Bus::TtisTypes::LibraryUpgradeResult:
            libraryType = TA_IRS_App::ITrainEventCallback::MessageLibrary;
            break;
        case TA_IRS_Bus::TtisTypes::ScheduleDownloadStart:
        case TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete:
        case TA_IRS_Bus::TtisTypes::ScheduleUpgradeResult:
            libraryType = TA_IRS_App::ITrainEventCallback::TimeSchedule;
            break;
        default:
            TA_ASSERT( false, "Invalid update type" );
            break;
        }

        switch ( event.updateType )
        {
        case TA_IRS_Bus::TtisTypes::LibraryDownloadStart:
        case TA_IRS_Bus::TtisTypes::ScheduleDownloadStart:
            {
				if((event.commandStatus == TA_IRS_Bus::TtisTypes::TtisSuccess)
					&& (event.downloadResult == TA_IRS_Bus::TtisTypes::Success))
				{
					// send a download start
					m_callback.trainDownloadStarted( event.trainId, libraryType, event.version );
				}
				else // failed, and download end
				{					
                    // send a download completion message on failure, because it failed to start
                    m_callback.trainDownloadComplete( event.trainId,
                                                      libraryType,
                                                      false,
                                                      convertDownloadStatusToString(event),
                                                      event.version );
				}

                break;
            }
        case TA_IRS_Bus::TtisTypes::LibraryDownloadComplete:
        case TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete:
            {
                // check if the download success
                bool success = 
					(event.downloadResult == TA_IRS_Bus::TtisTypes::Success) && 
					(event.commandStatus == TA_IRS_Bus::TtisTypes::TtisSuccess);            

                // notify of download completion
                m_callback.trainDownloadComplete( event.trainId,
                                                  libraryType,
                                                  success,
                                                  convertDownloadStatusToString(event),
                                                  event.version );
            }
            break;
        case TA_IRS_Bus::TtisTypes::LibraryUpgradeResult:
        case TA_IRS_Bus::TtisTypes::ScheduleUpgradeResult:
            // only check the command status, the download result is not used
            m_callback.trainUpgradeComplete( event.trainId,
                                             libraryType,
                                             event.commandStatus == TA_IRS_Bus::TtisTypes::TtisSuccess, // test for success
                                             convertUpgradStatusToString( event ),
                                             event.version );
            break;
        default:
            TA_ASSERT( false, "Invalid update type" );
            break;
        }

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::processTisReturnStatus(const TA_IRS_Bus::TtisTypes::TisReturnStatus&  event)
    {
        FUNCTION_ENTRY( "processTisReturnStatus" );

        //call the appropriate handler based on
        //the return status original command


        TA_IRS_Bus::TtisTypes::ETtisOriginalDisplayCommand originalCommand = event.originalParameters.getOriginalCommand();

        switch ( originalCommand )
        {
        case TA_IRS_Bus::TtisTypes::TtisFreeTextCommand:
            m_callback.freeTextMessageRequestComplete( event.trainId,
                                                       event.timestamp,
                                                       event.sessionId,
                                                       convertTrainParametersToTisParameters( event.originalParameters.getFreeTextParameters() ), // converted original parameters
                                                       event.result == TA_IRS_Bus::TtisTypes::TtisSuccess, // success or failure test
                                                       convertCommandResultToString( event.result ) ); // result string
            break;
        case TA_IRS_Bus::TtisTypes::TtisPredefinedCommand:
            m_callback.predefinedMessageRequestComplete( event.trainId,
                                                         event.timestamp,
                                                         event.sessionId,
                                                         convertTrainParametersToTisParameters( event.originalParameters.getPredefinedParameters() ), // converted original parameters
                                                         event.result == TA_IRS_Bus::TtisTypes::TtisSuccess, // success or failure test
                                                         convertCommandResultToString( event.result ) ); // result string
            break;
        case TA_IRS_Bus::TtisTypes::TtisResetCommand:
            m_callback.clearRequestComplete( event.trainId,
                                             event.timestamp,
                                             event.sessionId,
                                             convertTrainParametersToTisParameters( event.originalParameters.getResetParameters() ), // converted original parameters
                                             event.result == TA_IRS_Bus::TtisTypes::TtisSuccess, // success or failure test
                                             convertCommandResultToString( event.result ) ); // result string
            break;
        default:
            TA_ASSERT( false, "Invalid ETtisOriginalDisplayCommand" );
            break;
        }

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::processTrainDetails(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& event)
    {
        FUNCTION_ENTRY( "processTrainDetails" );

        bool needToReevaluate = true;

        {
            TA_THREADGUARD( m_trainDetailsLock );

            TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetails.find( event.trainId );

            if ( findIter != m_trainDetails.end() )
            {
                TA_IRS_Bus::LocationCache& locationcache = TA_IRS_Bus::LocationCache::getInstance();
                unsigned long agentLocation = locationcache.getLocationKey();
                TA_Base_Core::ILocation::ELocationType agentLocationType = locationcache.getLocationTypeByKey( agentLocation );

                if ( agentLocationType == TA_Base_Core::ILocation::OCC )
                {
                    agentLocationType = TA_Base_Core::ILocation::STATION;
                }

                // train is valid for us if its in our area or communicating with this agent
                bool newValidity = ( ( true == event.atsValid) &&
                                     ( true == event.isLocalisationValid ) &&
                                     ( locationcache.getLocationTypeByKey( event.currentLocation ) == agentLocationType) ) ||
                                   ( ( true == event.communicationValid ) &&
                                     ( agentLocation == event.communicatingLocation ) );

                bool oldValidity = ( ( true == findIter->second.atsValid) &&
                                     ( true == findIter->second.isLocalisationValid ) &&
                                     ( locationcache.getLocationTypeByKey( findIter->second.currentLocation ) == agentLocationType) ) ||
                                   ( ( true == findIter->second.communicationValid ) &&
                                     ( agentLocation == findIter->second.communicatingLocation ) );

                // re-evaluate if the train validity changed or it changed location types
                needToReevaluate = newValidity != oldValidity;
            }

            m_trainDetails[ event.trainId ] = event;
        }

        if ( true == needToReevaluate )
        {
            m_callback.verifyTrainDataVersions( event.trainId );
        }

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::processAgentOperationMode(const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event)
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );

        //do nothing

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::processTrainSelectionUpdate(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& event)
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );

        //do nothing

        FUNCTION_EXIT;
    }


    void TrainAgentSubscriber::initialiseInitialVersions()
    {
        FUNCTION_ENTRY( "initialiseInitialVersions" );

        try
        {
            TA_THREADGUARD( m_trainDetailsLock );

            TA_IRS_Bus::TrainInformationTypes::TrainDetailsList allTrains = m_trainInformationCorbaProxy->getTrainList();

            for ( TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator iter = allTrains.begin();
                  allTrains.end() != iter; ++iter )
            {
                m_trainDetails[ iter->trainId ] = *iter;
            }
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        // get the train version list from the train proxy
        //for each version, call
        //processTisLibraryVersionUpdate
		/*
        try
        {
            TA_IRS_Bus::TtisTypes::TisLibraryVersionList allVersions = m_trainTisCorbaProxy->getLibraryVersions();

            for ( TA_IRS_Bus::TtisTypes::TisLibraryVersionList::iterator iter = allVersions.begin();
                  allVersions.end() != iter; ++iter )
            {
                processTisLibraryVersionUpdate( *iter );
            }
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }
		*/

        FUNCTION_EXIT;
    }


    std::string TrainAgentSubscriber::convertDownloadResultToString(TA_IRS_Bus::TtisTypes::EDownloadResult downloadResult)
    {
        FUNCTION_ENTRY( "convertDownloadResultToString" );

        //return the appropriate status string
        //constant


        switch ( downloadResult )
        {
            case TA_IRS_Bus::TtisTypes::Success:
            FUNCTION_EXIT;
            return TRAIN_LIBRARY_DOWNLOAD_SUCCESS_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TransferFailed:
            FUNCTION_EXIT;
            return TRANSFER_FAILED_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TimedOut:
            FUNCTION_EXIT;
            return TRAIN_LIBRARY_DOWNLOAD_TIME_OUT;
            break;
        case TA_IRS_Bus::TtisTypes::MpuChangeover:
            FUNCTION_EXIT;
            return MPU_CHANGEOVER_ERROR_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TimsFailed:
            FUNCTION_EXIT;
            return TIMS_FAILED_STATUS;
            break;
        default:
            TA_ASSERT( false, "Invalid EDownloadResult" );
            break;
        }

        FUNCTION_EXIT;
        return "";
    }


	std::string TrainAgentSubscriber::convertDownloadStatusToString(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update)
	{
		// if download and command status all success
		if((update.downloadResult == TA_IRS_Bus::TtisTypes::Success) 
			&& (update.commandStatus == TA_IRS_Bus::TtisTypes::TtisSuccess))
		{
			return "Completed";
		}
		std::string statusString;
		// check the download status
		switch(update.downloadResult)
		{
			case TA_IRS_Bus::TtisTypes::TimedOut:
				statusString = "Time-out";
				break;
			case TA_IRS_Bus::TtisTypes::Cancelled:
				statusString = "Cancelled";
				break;
			case TA_IRS_Bus::TtisTypes::Upgraded:
				statusString = "Upgraded";
				break;
			default:
				break;
		}
		// if not got the status string, check command status
		if(true == statusString.empty())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Command Status Type : %d",update.commandStatus);
			if(update.commandStatus == TA_IRS_Bus::TtisTypes::TtisInUse)
			{
				statusString = "Train Busy";
			}
			else if(update.commandStatus == TA_IRS_Bus::TtisTypes::TtisFailed)
			{
				statusString = "Failed Train";
			}
			else if (update.commandStatus == TA_IRS_Bus::TtisTypes::TtisDutyHandover)
			{
				statusString = "Failed due to degraded";
			}
			else if (update.commandStatus == TA_IRS_Bus::TtisTypes::MpuChangeoverError)
			{
				statusString = "Failed due to changeover";
			}
			else if (update.commandStatus == TA_IRS_Bus::TtisTypes::TtisEndDwnloadFailed)
			{
				statusString = "Unsuccessful";
			}
			else
			{
				statusString = "Unsuccessful";
			}
		}
		return statusString;
	}

	std::string TrainAgentSubscriber::convertUpgradStatusToString(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update)
	{
		switch(update.commandStatus)
		{
			case TA_IRS_Bus::TtisTypes::TtisSuccess:
				return "Upgraded";
			case TA_IRS_Bus::TtisTypes::TtisInUse:
			case TA_IRS_Bus::TtisTypes::TtisReset:
			case TA_IRS_Bus::TtisTypes::TtisFailed:
			case TA_IRS_Bus::TtisTypes::TtisDifferentVersion:
				return "Failed train";
			case TA_IRS_Bus::TtisTypes::MpuChangeoverError:
				return "Failed due to changeover";
			default:
				return "Time-out";		
		}
	}

    std::string TrainAgentSubscriber::convertCommandResultToString(TA_IRS_Bus::TtisTypes::ETtisCommandResult commandStatus)
    {
        FUNCTION_ENTRY( "convertCommandResultToString" );

        //return the appropriate status string
        //constant


        switch( commandStatus )
        {
        case TA_IRS_Bus::TtisTypes::TtisSuccess:
            FUNCTION_EXIT;
            return SUCCESS_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TtisInUse:
            FUNCTION_EXIT;
            return TTIS_IN_USE_STATUS;
            break;
		case TA_IRS_Bus::TtisTypes::TtisDifferentVersion:
            FUNCTION_EXIT;
			return TTIS_DIFFERENT_VERSION_STATUS;
			break;
		case TA_IRS_Bus::TtisTypes::TransactionTimeoutError:
			FUNCTION_EXIT;
			return TRANSACTION_TIMEOUT_ERROR_STATUS;
			break;
        case TA_IRS_Bus::TtisTypes::TtisReset:
            FUNCTION_EXIT;
            return TTIS_RESET_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TtisFailed:
            FUNCTION_EXIT;
            return TTIS_FAILED_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::InvalidTrainError:
            FUNCTION_EXIT;
            return INVALID_TRAIN_ERROR_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TransmissionFailureError:
            FUNCTION_EXIT;
            return TRANSMISSION_FAILURE_ERROR_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::TransactionCancelledError:
            FUNCTION_EXIT;
            return TRANSACTION_CANCELLED_ERROR_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::BadCommandBadChecksum:
            FUNCTION_EXIT;
            return BAD_COMMAND_BAD_CHECKSUM_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::BadCommandUnknownCommand:
            FUNCTION_EXIT;
            return BAD_COMMAND_UNKNOWN_COMMAND_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::BadCommandBadParameter:
            FUNCTION_EXIT;
            return BAD_COMMAND_BAD_PARAMETER_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::BadCommandUnexpectedCommand:
            FUNCTION_EXIT;
            return BAD_COMMAND_UNEXPECTED_COMMAND_STATUS;
            break;
        case TA_IRS_Bus::TtisTypes::MpuChangeoverError:
            FUNCTION_EXIT;
            return MPU_CHANGEOVER_ERROR_STATUS;
            break;
        default:
            //TA_ASSERT( false, "Invalid ETtisCommandResult" );
			FUNCTION_EXIT;
			return TTIS_TRAIN_FAILED_STATUS;
            break;
        }

        FUNCTION_EXIT;
        return "";
    }


    TA_Base_Core::TTISFreeTextMessageParameters TrainAgentSubscriber::convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& trainParameters)
    {
        FUNCTION_ENTRY( "convertTrainParametersToTisParameters" );

        //create a TTISFreeTextMessageParameters,
        //and use conversion functions to
        //populate each member from the train type


        TA_Base_Core::TTISFreeTextMessageParameters result;

        result.pidList = convertPidListToTis( trainParameters.pidList );
        result.fontSize = convertFontSizeToTis( trainParameters.fontSize );
        result.justification = convertJustificationToTis( trainParameters.justification );
        result.intensity = convertIntensityToTis( trainParameters.intensity );
        result.displayMode = convertDisplayModeToTis( trainParameters.displayMode );
        result.perRun = trainParameters.perRun;
        result.message = trainParameters.message.c_str();

        FUNCTION_EXIT;
        return result;
    }


    TA_Base_Core::TTISMessageResetParameters TrainAgentSubscriber::convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisResetParameters& trainParameters)
    {
        FUNCTION_ENTRY( "convertTrainParametersToTisParameters" );

        //create a TTISMessageResetParameters,
        //and use conversion functions to
        //populate each member from the train type


        TA_Base_Core::TTISMessageResetParameters result;

        result.pidList = convertPidListToTis( trainParameters.pidList );
        result.messageType = convertResetMessageTypeToTis( trainParameters.messageType );

        FUNCTION_EXIT;
        return result;
    }


    TA_Base_Core::TrainPIDList TrainAgentSubscriber::convertPidListToTis(const TA_IRS_Bus::TtisTypes::TtisPidList& pidList)
    {
        FUNCTION_ENTRY( "convertPidListToTis" );

        TA_Base_Core::TrainPIDList result;
        result.length( pidList.size() );

        for ( unsigned long i = 0; i < pidList.size(); ++i )
        {
            result[i] = convertPidToTis( pidList[i] );
        }

        FUNCTION_EXIT;
        return result;
    }


    TA_Base_Core::TTISPredefinedMessageParameters TrainAgentSubscriber::convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& trainParameters)
    {
        FUNCTION_ENTRY( "convertTrainParametersToTisParameters" );

        //create a
        //TTISPredefinedMessageParameters, and
        //use conversion functions to populate
        //each member from the train type


        TA_Base_Core::TTISPredefinedMessageParameters result;

        result.pidList = convertPidListToTis( trainParameters.pidList );
        result.libraryVersion = trainParameters.libraryVersion;
        result.messageId = trainParameters.messageId;
        result.perRun = trainParameters.perRun;
        FUNCTION_EXIT;
        return result;
    }


    TA_Base_Core::ETTISClearType TrainAgentSubscriber::convertResetMessageTypeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisResetMessageType messageType)
    {
        FUNCTION_ENTRY( "convertResetMessageTypeToTis" );

        switch ( messageType )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisResetAll:
            FUNCTION_EXIT;
            return TA_Base_Core::CLEAR_ALL;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisResetEmergency:
            FUNCTION_EXIT;
            return TA_Base_Core::CLEAR_EMERGENCY;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisResetNormal:
            FUNCTION_EXIT;
            return TA_Base_Core::CLEAR_NORMAL;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisResetPriority6:
            FUNCTION_EXIT;
            return TA_Base_Core::CLEAR_PRIORITY6;
            break;
        default:
            TA_ASSERT( false, "Invalid messageType" );
            break;
        }

        FUNCTION_EXIT;
        return  TA_Base_Core::CLEAR_ALL;
    }


    TA_Base_Core::ELibrarySection TrainAgentSubscriber::convertLibrarySectionToTis(unsigned char identifierType)
    {
        FUNCTION_ENTRY( "convertLibrarySectionToTis" );

        switch ( identifierType )
        {
        case 0:
            FUNCTION_EXIT;
            return TA_Base_Core::EMERGENCY_SECTION;
            break;
        case 1:
            FUNCTION_EXIT;
            return TA_Base_Core::NORMAL_SECTION;
            break;
        default:
            TA_ASSERT( false, "Invalid identifierType" );
            break;
        }

        FUNCTION_EXIT;
        return  TA_Base_Core::EMERGENCY_SECTION;
    }


    TA_Base_Core::EPIDSelection TrainAgentSubscriber::convertPidToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid)
    {
        FUNCTION_ENTRY( "convertPidToTis" );

        switch ( pid )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidAllTrain:
            FUNCTION_EXIT;
            return TA_Base_Core::ENTIRE_TRAIN;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1:
            FUNCTION_EXIT;
            return TA_Base_Core::ALL_CAR1;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2:
            FUNCTION_EXIT;
            return TA_Base_Core::ALL_CAR2;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3:
            FUNCTION_EXIT;
            return TA_Base_Core::ALL_CAR3;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid1:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR1_PID1;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid2:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR1_PID2;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid3:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR1_PID3;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid4:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR1_PID4;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid1:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR2_PID1;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid2:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR2_PID2;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid3:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR2_PID3;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid4:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR2_PID4;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid1:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR3_PID1;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid2:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR3_PID2;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid3:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR3_PID3;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid4:
            FUNCTION_EXIT;
            return TA_Base_Core::CAR3_PID4;
            break;
        default:
            TA_ASSERT( false, "Invalid ETtisPid" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::ENTIRE_TRAIN;
    }


    TA_Base_Core::ETTISLEDFontSize TrainAgentSubscriber::convertFontSizeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize fontSize)
    {
        FUNCTION_ENTRY( "convertFontSizeToTis" );

        switch ( fontSize )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisFontSmall:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_FONT_SIZE_SMALL;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisFontLarge:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_FONT_SIZE_LARGE;
            break;
        default:
            TA_ASSERT( false, "Invalid fontSize" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::TTIS_FONT_SIZE_SMALL;
    }


    TA_Base_Core::EJustification TrainAgentSubscriber::convertJustificationToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisJustification justification)
    {
        FUNCTION_ENTRY( "convertJustificationToTis" );

        switch ( justification )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationLeft:
            FUNCTION_EXIT;
            return TA_Base_Core::LEFT;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationCentre:
            FUNCTION_EXIT;
            return TA_Base_Core::CENTRED;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationRight:
            FUNCTION_EXIT;
            return TA_Base_Core::RIGHT;
            break;
        default:
            TA_ASSERT( false, "Invalid justification" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::LEFT;
    }


    TA_Base_Core::ETTISMessagePriority TrainAgentSubscriber::convertMessagePriorityToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority priority)
    {
        FUNCTION_ENTRY( "convertMessagePriorityToTis" );

        switch ( priority )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityOne:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_1;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityTwo:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_2;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityThree:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_3;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFour:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_4;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFive:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_5;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySix:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_6;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySeven:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_7;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityEight:
            FUNCTION_EXIT;
            return TA_Base_Core::TTIS_PRIORITY_8;
            break;
        default:
            TA_ASSERT( false, "Invalid priority" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::TTIS_PRIORITY_1;
    }


    TA_Base_Core::EDisplayMode TrainAgentSubscriber::convertDisplayModeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisDisplayMode displayMode)
    {
        FUNCTION_ENTRY( "convertDisplayModeToTis" );

        switch ( displayMode )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollLeft:
            FUNCTION_EXIT;
            return TA_Base_Core::SCROLL_LEFT;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollRight:
            FUNCTION_EXIT;
            return TA_Base_Core::SCROLL_RIGHT;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollUp:
            FUNCTION_EXIT;
            return TA_Base_Core::SCROLL_UP;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollDown:
            FUNCTION_EXIT;
            return TA_Base_Core::SCROLL_DOWN;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeInstantOn:
            FUNCTION_EXIT;
            return TA_Base_Core::NORMAL_STATIONARY;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeBlinking:
            FUNCTION_EXIT;
            return TA_Base_Core::BLINKING;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeWiping:
            FUNCTION_EXIT;
            return TA_Base_Core::WIPING;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeLikeSnow:
            FUNCTION_EXIT;
            return TA_Base_Core::SNOW;
            break;
        default:
            TA_ASSERT( false, "Invalid displayMode" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::SCROLL_LEFT;
    }


    TA_Base_Core::ETTISLEDIntensity TrainAgentSubscriber::convertIntensityToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisIntensity intensity)
    {
        FUNCTION_ENTRY( "convertIntensityToTis" );

        switch ( intensity )
        {
        case TA_IRS_Bus::ProtocolTtisTypes::TtisIntensityNormal:
            FUNCTION_EXIT;
            return TA_Base_Core::NORMAL;
            break;
        case TA_IRS_Bus::ProtocolTtisTypes::TtisIntensityHigh:
            FUNCTION_EXIT;
            return TA_Base_Core::HIGH;
            break;
        default:
            TA_ASSERT( false, "Invalid intensity" );
            break;
        }

        FUNCTION_EXIT;
        return TA_Base_Core::NORMAL;
    }


    std::string TrainAgentSubscriber::convertStartTimeToTis(const TA_IRS_Bus::TtisTypes::TimeInterval& displayPeriod)
    {
        FUNCTION_ENTRY( "convertStartTimeToTis" );
        FUNCTION_EXIT;
        return convertTimeToTis( displayPeriod.startHour, displayPeriod.startMin );
    }


    std::string TrainAgentSubscriber::convertTimeToTis(unsigned char hour, unsigned char minute)
    {
        FUNCTION_ENTRY( "convertTimeToTis" );

        std::ostringstream timeStr;

        timeStr << std::setw(2) << std::setfill('0') << static_cast<unsigned long>( hour );
        timeStr << std::setw(2) << std::setfill('0') << static_cast<unsigned long>( minute );

        FUNCTION_EXIT;
        return timeStr.str();
    }


    std::string TrainAgentSubscriber::convertEndTimeToTis(const TA_IRS_Bus::TtisTypes::TimeInterval& displayPeriod)
    {
        FUNCTION_ENTRY( "convertEndTimeToTis" );
        FUNCTION_EXIT;
        return convertTimeToTis( displayPeriod.stopHour, displayPeriod.stopMin );
    }

	void TrainAgentSubscriber::reloadTrainsDetails()
	{	
		FUNCTION_ENTRY( "reloadTrainsDetails" );
		
        try
        {
            TA_THREADGUARD( m_trainDetailsLock );
			
            TA_IRS_Bus::TrainInformationTypes::TrainDetailsList allTrains = m_trainInformationCorbaProxy->getTrainList();
			
            for ( TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator iter = allTrains.begin();
			allTrains.end() != iter; ++iter )
            {
                m_trainDetails[ iter->trainId ] = *iter;
            }
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }       
        FUNCTION_EXIT;
	}
}
