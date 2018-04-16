/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/TrainLiveBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #8 $
  * Last modification : $DateTime: 2017/11/15 13:35:59 $
  * Last modified by : $Author: huang.wenbo $
  *
  *
  */

#include <time.h>

#include "core/utilities/src/DebugUtil.h"

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/GroupInUseException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/TrainLiveBroadcast.h"





using namespace TA_Base_Core;


namespace TA_IRS_App
{


    TrainLiveBroadcast::TrainLiveBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
        const std::string& sessionId)
        : AbstractTrainBroadcast( broadcastConfigP, sessionId),
        m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theTrainLiveConfig() ),
        m_nextState(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS)
    {
        FUNCTION_ENTRY( "TrainLiveBroadcast" );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = 
            calculateEffectiveCoverageIndices(m_theSpecifics.trains, m_theSpecifics.trains);

        // Set the initial effective coverage to be the entire list of trains
        // The false flag prevents the sending of the synchronization message
        setEffectiveCoverageIndices( effectiveCoverageIndices, false );

        // now upon startup, set the state to connecting.
        //
        // The states will be
        // 1. Connecting
        // 2. Ready for live announcement
        // 3. (Begin pressed) In Progress
        // etc
        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex],
                TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING);
        }

        FUNCTION_EXIT;
    }


    TrainLiveBroadcast::~TrainLiveBroadcast()
    {
        FUNCTION_ENTRY( "~TrainLiveBroadcast" );
        FUNCTION_EXIT;
    }


    int TrainLiveBroadcast::getIndexForTrain(CORBA::Octet trainId)
    {
        FUNCTION_ENTRY( "getIndexForTrain" );

        // find trainId

        for (unsigned int index=0; index< m_theSpecifics.trains.length(); ++index)
        {
            if (m_theSpecifics.trains[index] == trainId)
            {
                // found train id
                FUNCTION_EXIT;
                return index;
            }
        }

        // not found
        FUNCTION_EXIT;
        return -1;
    }


    bool TrainLiveBroadcast::getIsPartOfBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId)
    {
        FUNCTION_ENTRY( "getIsPartOfBroadcast" );

        // check the announcement ID first
        if (announcementId == getAnnouncementId())
        {
            // find trainId

            for (unsigned int index=0; index< m_theSpecifics.trains.length(); ++index)
            {
                if (m_theSpecifics.trains[index] == trainId)
                {
                    // found train id
                    FUNCTION_EXIT;
                    return true;

                }
            }
        }

        // not found
        FUNCTION_EXIT;
        return false;
    }


    void TrainLiveBroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "validateBroadcastConfig" );

        ThreadGuard guard( m_lock );

        // At least one train must be specified
        if (m_theSpecifics.trains.length() == 0)
        {
            FUNCTION_EXIT;
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "No trains requested" );
        }

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::execute()
    {
        FUNCTION_ENTRY( "execute" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage effectiveTrains =
            getEffectiveTrainCoverage(m_theSpecifics.trains);

        // package up the list of trains for TrainAgent
        TA_IRS_Bus::CommonTypes::TrainIdList trainList;

        //trainList.length(effectiveTrains.length());
        for (unsigned int i = 0; i < effectiveTrains.length(); ++i)
        {
            trainList.insert( static_cast< TA_IRS_Bus::CommonTypes::TrainIdType >( effectiveTrains[i] ) );
        }

        try
        {
            setAnnouncementId(getNextAvailableTrainAnnouncementId());
        }
        catch(TA_Base_Core::NoFreeAnnounceIdException&)
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);

            FUNCTION_EXIT;
            return;
        }


        // set the next state
        m_nextState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY;

        try
        {
            m_trainPaCorbaProxy->setupLiveAnnouncement( trainList,
                getAnnouncementId(),
                m_theSpecifics.hasOverrideOption,
                m_theSpecifics.staticGroupName.in(),
                getSessionId().c_str() );

            // the results for each train come in as comms messages
            // if the setup succeeded, a ready for live announcement comes in (processRuntimeNotification)
            // if the setup failed, a command result comes in (handleCommandResult)
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& )
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::TrainAccessDeniedException& )
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& )
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::GroupInUseException& )
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_STATIC_GROUP_IN_USE);

            FUNCTION_EXIT;
            return;
        }

        // Clear any past errors only for the transition from Execution to Runtime
        // In the case of the transition from Execution to Termination to PendingRemoval,
        // we want to retain the GlobalState so that the PA Manager can see the
        // errors encountered.
        m_progressReportSentry->resetGlobalState();

        // Finished executing each train once so we move to the runtime state
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Runtime);

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::handleCommandResult( TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& effectiveCoverageIndices,
        unsigned int trainIndexIndex,
        const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
    {
        FUNCTION_ENTRY( "handleCommandResult" );

        ThreadGuard guard( m_lock );

        switch (event.commandResult)
        {
        case TA_IRS_Bus::PaTypes::PaProceeding:
            {
                // based on the broadcast phase, set the correct status
                TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState broadcastState = m_progressReportSentry->getBroadcastState();

                switch ( broadcastState )
                {
                    // setting up the live announcement
                case TA_Base_Bus::IPAAgentCorbaDef::Creation:
                case TA_Base_Bus::IPAAgentCorbaDef::Execution:
                case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY );
                    break;

                    // ending a live announcement
                case TA_Base_Bus::IPAAgentCorbaDef::Termination:
                case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Receive Statue3=1(Proceeding) when Train Live Broadcast terminated." );
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST );
                    break;

                default:
                    break;
                }
            }
            break;

        case TA_IRS_Bus::PaTypes::PaInUse:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY);
            break;

        case TA_IRS_Bus::PaTypes::PaInUseOptionToOverride:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY_WITH_OVERRIDE_OPTION);
            break;

        case TA_IRS_Bus::PaTypes::PaInUseFirstCycleCancelled:
            // not a valid response for this operation
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN);
            break;

        case TA_IRS_Bus::PaTypes::PaReset:
            //m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET);
            {
                // based on the broadcast phase, set the correct status
                TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState broadcastState = m_progressReportSentry->getBroadcastState();

                switch ( broadcastState )
                {
                    // proceeding the live announcement
                case TA_Base_Bus::IPAAgentCorbaDef::Creation:
                case TA_Base_Bus::IPAAgentCorbaDef::Execution:
                case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
                    m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET);
					break;
                    // ending a live announcement
                case TA_Base_Bus::IPAAgentCorbaDef::Termination:
                case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Receive acknowledge message Statue3=5(Proceeding) when Train Live Broadcast terminated." );
					m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET );
                    break;

                default:
                    break;
                }
            }
            break;

        case TA_IRS_Bus::PaTypes::PaFailed:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DUE_TO_NO_AUDIO);
            //m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN);
            break;

        case TA_IRS_Bus::PaTypes::PaDifferentOrigin:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_ORIGIN);
            break;

        case TA_IRS_Bus::PaTypes::PaDifferentId:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST);
            break;

        case TA_IRS_Bus::PaTypes::PaDifferentVersion:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_VERSION);
            break;

        case TA_IRS_Bus::PaTypes::BadCommandBadChecksum:
        case TA_IRS_Bus::PaTypes::BadCommandUnknownCommand:
        case TA_IRS_Bus::PaTypes::BadCommandBadParameter:
        case TA_IRS_Bus::PaTypes::BadCommandUnexpectedCommand:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BAD_COMMAND);
            break;

        case TA_IRS_Bus::PaTypes::InvalidTrainError:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_PERMISSION);
            break;

        // DTL-735 add start
        case TA_IRS_Bus::PaTypes::TransactionTimeoutError:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_RESPONSE_FROM_TRAIN);
            break;
        // DTL-735 add end

        case TA_IRS_Bus::PaTypes::TransmissionFailureError:
        // DTL-735 delete start
        //case TA_IRS_Bus::PaTypes::TransactionTimeoutError:
        // DTL-735 delete end
        case TA_IRS_Bus::PaTypes::TransactionCancelledError:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN);
            break;

        case TA_IRS_Bus::PaTypes::MpuChangeoverError:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER);
            break;

        default:
            // unknown enumeration value
            break;
        }

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices TrainLiveBroadcast::getTrainsRequiringContinue()
    {
        FUNCTION_ENTRY( "getTrainsRequiringContinue" );

        //See runtimeSpecifics
        //Get all train statuses, and return all those with the status LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        std::vector<unsigned long> temp;
        unsigned int i;
        for ( i = 0; i < effectiveCoverageIndices.length(); ++i)
        {
            unsigned int index = effectiveCoverageIndices[i];
            if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED == m_progressReportSentry->getLocalState(index) )
            {
                temp.push_back( index );
            }
        }

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveTrainsIndex;
        effectiveTrainsIndex.length( temp.size() );
        for ( i = 0; i < temp.size(); ++i )
        {
            effectiveTrainsIndex[i] = temp[i];
        }

        FUNCTION_EXIT;
        return effectiveTrainsIndex;
    }


    void TrainLiveBroadcast::runtimeSpecifics()
    {
        FUNCTION_ENTRY( "runtimeSpecifics" );

        // The update of the progress report structure is acheived by the CORBA messaging threads
        // which call one of the following:
        //
        // beginTrainLiveAnnouncement()
        // continueTrainLiveAnnouncement()
        // endTrainLiveAnnouncement()
        // processRuntimeNotification()
        //

        // The sending of the progressReport is performed by the BroadcastWorker as soon as we finish here

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Localstate is %d, next localstate is %d", m_progressReportSentry->getLocalState(0), m_nextState);

        if ( true == areAllTrainsFinished() )
        {
            // Terminate
            setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
        }
        // else stay in Runtime

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        ThreadGuard guard( m_lock );

        // check if all train results have been received
        // or if the timeout has been exceeded
        if (( 0 == getAnnouncementId() ) || areAllTrainsFinished() || (( 0 != m_terminateTimeLimit ) && ( time(NULL) > m_terminateTimeLimit )))
        {
            // Reset the announce id so that it can be used by new broadcasts
            releaseUsingAnnouncementId( m_trainAnnouncementId );

            setAnnouncementId(0);

            updateFinalProgressReport();

            setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
        }

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::beginTrainLiveAnnouncement() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "beginTrainLiveAnnouncement" );

        ThreadGuard guard( m_lock );

        try
        {
            m_trainPaCorbaProxy->beginLiveAnnouncement( getAnnouncementId(), getSessionId().c_str() );

            // set the next state - if the begin completed ok
            m_nextState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS;

            TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

            // set each ready train to in progress
            for (unsigned int indexIndex = 0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
            {
                // only update the state to in progress if it was ready,
                if ( m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]) ==
                    TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY )
                {
                    m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex],
                        TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS);
                }
            }
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::TrainAccessDeniedException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_IRS_Bus::TrainRadioException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_RADIO_CALL_RESET);
            FUNCTION_EXIT;
            return;
        }
        catch( ... )
        {
            // Unexpected exception...
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);

            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::continueTrainLiveAnnouncement()
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices trainsRequiringContinue = getTrainsRequiringContinue();

        try
        {
            m_trainPaCorbaProxy->continueLiveAnnouncement( getAnnouncementId(), getSessionId().c_str() );
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT);
            return;
        }
        catch ( const TA_IRS_Bus::TrainAccessDeniedException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT);
            return;
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
            return;
        }
        catch( ... )
        {
            // Unexpected exception...
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
            // operation not allowed - abort and send back to the PA Manager
            return;
        }

        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trainCoverage;
        trainCoverage.length( trainsRequiringContinue.length() );

        int nTempIndex = 0;
        for (unsigned int i = 0; i < trainsRequiringContinue.length(); ++i)
        {
            m_progressReportSentry->setLocalState( trainsRequiringContinue[i], m_nextState );
            unsigned int nTrainIndex = trainsRequiringContinue[i];
            if ( nTrainIndex < m_theSpecifics.trains.length() )
            {
                trainCoverage[nTempIndex] = m_theSpecifics.trains[nTrainIndex];
                ++nTempIndex;
            }
        }
		trainCoverage.length(nTempIndex);

        PasHelpers::getInstance()->submitTrainPAContinueLiveAnnouncementAuditMessage(
            getBroadcastId(), trainCoverage, getSessionId() );

        setHasProgressReportChanged( true );
    }

    void TrainLiveBroadcast::endTrainLiveAnnouncement() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "endTrainLiveAnnouncement" );

        ThreadGuard guard( m_lock );

        // Do the actual disconnection work in terminate()

        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Aborting);

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::disconnect()
    {
        FUNCTION_ENTRY( "disconnect" );

        ThreadGuard guard( m_lock );

        // set the next state
        m_nextState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED;

        m_terminateTimeLimit = time( NULL ); // now

        unsigned long maximumWaitTime = 120000; // default time of 2 minutes

        try
        {
            maximumWaitTime = m_trainPaCorbaProxy->getCommandTimeout();
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        try
        {
            m_trainPaCorbaProxy->endLiveAnnouncement( getAnnouncementId(), getSessionId().c_str() );

            // end triggered successfully, allow the time to complete
            m_terminateTimeLimit += maximumWaitTime;
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT);
        }
        catch ( const TA_IRS_Bus::TrainAccessDeniedException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT);
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& )
        {
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
        }
        catch( ... )
        {
            // Unexpected exception...
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
            // operation not allowed - abort and send back to the PA Manager
        }

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
    {
        FUNCTION_ENTRY( "processRuntimeNotification" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            if (m_theSpecifics.trains[effectiveCoverageIndices[indexIndex]] == event.trainId)
            {
                // Found
                switch(event.updateType)
                {

                case TA_IRS_Bus::PaTypes::LiveAnnouncementReady:

                    // Prevent multiple train ready messages resetting state
                    if ( m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]) == TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING )
                    {
                        m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY );
                    }
                    break;

                case TA_IRS_Bus::PaTypes::LiveAnnouncementContinueRequired:

                    // continue required
                    if ( TA_IRS_Bus::PaTypes::PaProceeding == event.commandResult )
                    {
                        m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED );
                    }
                    else // TA_IRS_Bus::PaTypes::PaReset
                    {
                        // any continue timeout should be reset
                        if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT == m_progressReportSentry->getLocalState( effectiveCoverageIndices[indexIndex] ) )
                        {
                            m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], m_nextState );
                        }
                    }
                    break;

                case TA_IRS_Bus::PaTypes::AnnouncementReset:

                    if ( TA_IRS_Bus::PaTypes::MpuChangeoverError == event.commandResult )
                    {
                        m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER);
                    }
					else if(TA_IRS_Bus::PaTypes::PaFailed == event.commandResult)
					{
                        m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DUE_TO_NO_AUDIO);
					}
                    else
                    {
                        m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET);
                    }

                    if (areAllTrainsReset())
                    {
                        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
                        setHasProgressReportChanged( true );
                    }

                    break;

                case TA_IRS_Bus::PaTypes::CompletedAnnouncement:

                    // Ignored as only relevant to DVA messages
                    break;

                case TA_IRS_Bus::PaTypes::LiveAnnouncementContinueTimeout:

                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT);
                    break;

                case TA_IRS_Bus::PaTypes::PaCommandResult:
                    // This is the result of the setup command
                    handleCommandResult(effectiveCoverageIndices, indexIndex, event);
                    break;

                default:
                    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Unexpected train PANotificationType");
                }

                FUNCTION_EXIT;
                return;

            }
        }

        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugWarn, "TrainId not matched.");

        FUNCTION_EXIT;
    }


    void TrainLiveBroadcast::processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event )
    {
        FUNCTION_ENTRY( "processRuntimeNotification" );

        ThreadGuard guard( m_lock );

        // This means the call has been dropped
        if (event.updateType == TA_IRS_Bus::PaTypes::AudioCallLost)
        {
            // the broadcast must be set to the failed state
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_RADIO_CALL_RESET);
        }

        FUNCTION_EXIT;
    }


    bool TrainLiveBroadcast::areAllTrainsReset()
    {
        FUNCTION_ENTRY( "areAllTrainsReset" );

        bool allTrainsReset = true;

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET !=
                m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]) )
            {
                allTrainsReset = false;
            }
        }

        FUNCTION_EXIT;
        return allTrainsReset;
    }


    bool TrainLiveBroadcast::areAllTrainsFinished()
    {
        FUNCTION_ENTRY( "areAllTrainsFinished" );

        bool allTrainsFinished = true;

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            TA_Base_Bus::IPAAgentCorbaDef::ELocalState eCurState = m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]);
            if (( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING  == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT == eCurState ))
            {
                allTrainsFinished = false;
                break;
            }
        }

        FUNCTION_EXIT;
        return allTrainsFinished;
    }

    void TrainLiveBroadcast::aborting()
    {
        if ( 0 != getAnnouncementId() )
        {
            if ( 0 == m_terminateTimeLimit )
            {
                // send the disconnect
                disconnect();
            }
        }

        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    }

    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage TrainLiveBroadcast::getTrainCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_theSpecifics.trains;
    }

    void TrainLiveBroadcast::setHasOverrideOption( bool override )
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        m_theSpecifics.hasOverrideOption = override;
    }

    void TrainLiveBroadcast::prepare()
    {
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
    }

} // namespace TA_IRS_App

