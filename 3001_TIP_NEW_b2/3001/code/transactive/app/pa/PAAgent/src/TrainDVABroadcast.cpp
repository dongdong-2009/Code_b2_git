/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/TrainDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #8 $
  * Last modification : $DateTime: 2013/04/05 14:57:31 $
  * Last modified by : $Author: qi.huang $
  *
  *
  */
#include <time.h>

#include "core/utilities/src/DebugUtil.h"

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCorbaProxy/src/PaTypesConversion.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"




using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

    TrainDVABroadcast::TrainDVABroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                         const std::string& sessionId)
     : AbstractTrainBroadcast( broadcastConfigP, sessionId ),
       m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theTrainDvaConfig() )
    {
        FUNCTION_ENTRY( "TrainDVABroadcast" );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = 
            calculateEffectiveCoverageIndices(m_theSpecifics.trains, m_theSpecifics.trains);

        // Set the initial effective coverage to be the entire list of trains
        // The false flag prevents the sending of the synchronization message
        setEffectiveCoverageIndices( effectiveCoverageIndices, false );

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex],
                TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING);
        }

        FUNCTION_EXIT;
    }


    TrainDVABroadcast::~TrainDVABroadcast()
    {
        FUNCTION_ENTRY( "~TrainDVABroadcast" );
        FUNCTION_EXIT;
    }


    bool TrainDVABroadcast::isCyclic()
    {
        FUNCTION_ENTRY( "isCyclic" );

        FUNCTION_EXIT;
        return !( ( 0 == m_theSpecifics.stCyclicParams.periodInSecs ) &&
                  ( 0 == m_theSpecifics.stCyclicParams.durationInSecs ) );
    }


    bool TrainDVABroadcast::getIsPartOfBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId)
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


    void TrainDVABroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "validateBroadcastConfig" );

        ThreadGuard guard( m_lock );
        int i = 0;

        // At least one train must be specified
        if (m_theSpecifics.trains.length() == 0)
        {
            FUNCTION_EXIT;
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "No trains requested" );
        }

        // Test validity of message key in database
        try
        {
            TA_IRS_Bus::TrainDvaMessageRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getTrainDvaMessageRecordFromKey(m_theSpecifics.messageKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            FUNCTION_EXIT;
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested message does not exist in the database" );
        }

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::execute()
    {
        FUNCTION_ENTRY( "execute" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage effectiveTrains = getEffectiveTrainCoverage(m_theSpecifics.trains);

        // package up the list of trains for TrainAgent
        TA_IRS_Bus::CommonTypes::TrainIdList trainList;

        //trainList.length(effectiveTrains.length());
        for (unsigned int i = 0; i < effectiveTrains.length(); ++i)
        {
            trainList.insert( static_cast< unsigned char > (effectiveTrains[i]) );
        }

        TA_IRS_Bus::TrainDvaMessageRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getTrainDvaMessageRecordFromKey(m_theSpecifics.messageKey);
        //unsigned long messageIdAkaTableOffset = record.m_pKeyOrId;
        unsigned short messageIdAkaTableOffset = static_cast<unsigned short>(record.m_pKeyOrId);

        // Initialize with a new announcementId
        try
        {
            setAnnouncementId(getNextAvailableTrainAnnouncementId());
        }
        catch(TA_Base_Core::NoFreeAnnounceIdException&)
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);
            // Process next train if there are any left for execution
            FUNCTION_EXIT;
            return;
        }


        try
        {
            if (isCyclic())
            {
                TA_IRS_Bus::PaTypes::MessageSchedule stPeriod = { m_theSpecifics.stCyclicParams.periodInSecs, m_theSpecifics.stCyclicParams.durationInSecs };

                m_trainPaCorbaProxy->cyclicAnnouncement( trainList,
                                                         getAnnouncementId(),
                                                         messageIdAkaTableOffset,
                                                         m_theSpecifics.libraryVersion,
                                                         stPeriod,
                                                         m_theSpecifics.hasOverrideOption,
                                                         getSessionId().c_str());
            }
            else
            {
                m_trainPaCorbaProxy->preRecordedAnnouncement( trainList,
                                                              getAnnouncementId(),
                                                              messageIdAkaTableOffset,
                                                              m_theSpecifics.libraryVersion,
                                                              m_theSpecifics.hasOverrideOption,
                                                              getSessionId().c_str(),
															  m_theSpecifics.isEmergency);
            }
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
        catch( ... )
        {
            // Unexpected exception...
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);

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


    void TrainDVABroadcast::handleCommandResult(
        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices& effectiveCoverageIndices,
        unsigned int trainIndexIndex,
        const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
    {
        FUNCTION_ENTRY( "handleCommandResult" );

        ThreadGuard guard( m_lock );

        switch (event.commandResult)
        {
        case TA_IRS_Bus::PaTypes::PaProceeding:
            {
                TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState broadcastState = m_progressReportSentry->getBroadcastState();

                switch ( broadcastState )
                {
                    // these states set up the broadcast
                    case TA_Base_Bus::IPAAgentCorbaDef::Creation:
                    case TA_Base_Bus::IPAAgentCorbaDef::Execution:
                    case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
                        m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY );
                        break;

                    // these states terminate it (with an abort command)
                    case TA_Base_Bus::IPAAgentCorbaDef::Aborting:
                    case TA_Base_Bus::IPAAgentCorbaDef::Termination:
                    case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Receive Statue3=1(Proceeding) when Train DVA Broadcast terminated." );
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
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED);
            break;

        case TA_IRS_Bus::PaTypes::PaReset:
            //m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET);
            {
                TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState broadcastState = m_progressReportSentry->getBroadcastState();

                switch ( broadcastState )
                {
                    // these states set up the broadcast
                case TA_Base_Bus::IPAAgentCorbaDef::Creation:
                case TA_Base_Bus::IPAAgentCorbaDef::Execution:
                case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET );
                    break;

                    // these states terminate it (with an abort command)
                case TA_Base_Bus::IPAAgentCorbaDef::Termination:
                case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED );
                    break;

                default:
                    break;
                }
            }
            break;

        case TA_IRS_Bus::PaTypes::PaFailed:
            m_progressReportSentry->setLocalState(effectiveCoverageIndices[trainIndexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN);
            break;

        case TA_IRS_Bus::PaTypes:: PaDifferentOrigin:
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

        // check to ensure that there is at least one train that is successful
        checkAllTrainStatus();

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::checkAllTrainStatus()
    {
        FUNCTION_ENTRY( "checkAllTrainStatus" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        // If no trains were successful, we terminate.
        bool foundSuccess = false;

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            TA_Base_Bus::IPAAgentCorbaDef::ELocalState eCurState = m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]);

            if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY == eCurState )
            {
                // Set back to InProgress to indicate the second phase
                m_progressReportSentry->setLocalState(effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS);

                // Found at least one successful train
                foundSuccess = true;
            }
            else if (( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS == eCurState ) || 
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED == eCurState ) || 
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING == eCurState ))
            {
                // Found at least one successful train
                foundSuccess = true;
            }
        }

        if ( !foundSuccess && ( TA_Base_Bus::IPAAgentCorbaDef::Termination != m_progressReportSentry->getBroadcastState() ) )
        {
            setBroadcastState( TA_Base_Bus::IPAAgentCorbaDef::Termination );
        }

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::runtimeSpecifics()
    {
        FUNCTION_ENTRY( "runtimeSpecifics" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        // Cyclic and oneshot broadcasts are both tested in the same manner for termination

        bool bInProgress = false;
        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            switch (m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]))
            {
            case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING:
                // Initialized state
            case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS:
                // Still running - wait for processRuntimeNotification() to flag the
                // remaining trains as completed OR until they fail
            case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED:
                // Still running - wait for processRuntimeNotification() to flag the
                // remaining trains as completed OR until they fail
                bInProgress = true;
                break;
            default:
                bInProgress = false;
                break;
            }

            if ( bInProgress )
            {
                FUNCTION_EXIT;
                return;
            }
        }

        // Got here - no trains in progress anymore - therefore we terminate
        m_terminateTimeLimit = time(NULL);

        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        ThreadGuard guard( m_lock );

        // check if all train results have been received
        // or if the timeout has been exceeded
        if (( 0 == getAnnouncementId() ) || areAllTrainsFinished() || (( 0 != m_terminateTimeLimit ) && ( time( NULL ) > m_terminateTimeLimit )) )
        {
            // Reset the announce id so that it can be used by new broadcasts
            releaseUsingAnnouncementId( m_trainAnnouncementId );

            setAnnouncementId(0);

            updateFinalProgressReport( false );

            setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
        }

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::operatorAbort()
    {
        FUNCTION_ENTRY( "operatorAbort" );

        ThreadGuard guard( m_lock );

        // Normal train DVA broadcast can't be abort as protocol sequence restricted
        // Waiting for hardware response or no response.
        if ( !m_theSpecifics.isEmergency )
        {
            return;
        }

        // Do not call this, it sets everything to aborted regardless of the return status of the train
        //AbstractBroadcast::operatorAbort();
        // manually set to termination phase
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Aborting);

        FUNCTION_EXIT;
    }


    bool TrainDVABroadcast::areAllTrainsFinished()
    {
        FUNCTION_ENTRY( "areAllTrainsFinished" );

        bool allTrainsFinished = true;

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            TA_Base_Bus::IPAAgentCorbaDef::ELocalState eCurState = m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]);
            if (( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY == eCurState ) ||
                ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED == eCurState ))
            {
                allTrainsFinished = false;
                break;
            }
        }

        FUNCTION_EXIT;
        return allTrainsFinished;
    }


    void TrainDVABroadcast::disconnect()
    {
        FUNCTION_ENTRY( "disconnect" );

        ThreadGuard guard( m_lock );

        // but we are trying to minimize interface changes
        TA_IRS_Bus::CommonTypes::TrainIdList trainIdList;

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();
        for (unsigned int indexIndex=0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex)
        {
            TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState = m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]);

            if (( localState == TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS ) ||
                ( localState == TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY ) ||
                ( localState == TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED ))
            {
                trainIdList.insert( m_theSpecifics.trains[effectiveCoverageIndices[indexIndex]] );
            }
        }

        if ( trainIdList.empty() )
        {
            m_progressReportSentry->flagNonCompletedWithErrorsAndAborted( false );

            FUNCTION_EXIT;
            return;
        }

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
            // Note that for the DVA cyclic, we are using the endLiveAnnouncement() CORBA method to
            // send the PaReset command.  Really we should generalize the method name or provide another
            m_trainPaCorbaProxy->endBroadcast( trainIdList, getAnnouncementId(), getSessionId().c_str() );

            // end triggered successfully, allow the time to complete
            m_terminateTimeLimit += maximumWaitTime;
        }
        catch ( const TA_IRS_Bus::AgentCommunicationException& )
        {
            // Unexpected timeout condition
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT);
            // terminating anyway
        }
        catch ( const TA_IRS_Bus::TrainAccessDeniedException& )
        {
            // Unexpected timeout condition
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT);
            // terminating anyway
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& )
        {
            // Unexpected timeout condition
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
            // terminating anyway
        }
        catch( ... )
        {
            // Unexpected exception...
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT);
            // operation not allowed - abort and send back to the PA Manager
        }

        FUNCTION_EXIT;
    }


    void TrainDVABroadcast::processRuntimeNotification( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
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
                case TA_IRS_Bus::PaTypes::LiveAnnouncementContinueRequired:
                case TA_IRS_Bus::PaTypes::LiveAnnouncementContinueTimeout:

                    // Ignored as these are only for Live broadcasts
                    break;

                case TA_IRS_Bus::PaTypes::AnnouncementReset:
                    {
                        if ( TA_IRS_Bus::PaTypes::MpuChangeoverError == event.commandResult )
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER );
                        }
                    }
                    break;

                case TA_IRS_Bus::PaTypes::CompletedAnnouncement:
                    if ( TA_IRS_Bus::ProtocolPaTypes::Successful == event.completionStatus )
                    {
                        m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED );
                    }
                    else
                    {
                        m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INTERRUPTED );
                    }
                    break;

                case TA_IRS_Bus::PaTypes::PaCommandResult:
                    // This is the result of the execute command
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

    void TrainDVABroadcast::aborting()
    {
        FUNCTION_ENTRY( "processRuntimeNotification" );

        if ( 0 != getAnnouncementId() )
        {
            if ( 0 == m_terminateTimeLimit )
            {
                // send the disconnect
                disconnect();
            }
        }
        else
        {
            // Abort without execution
            // DTL-340 modify start
            //m_progressReportSentry->flagNonCompletedWithErrorsAndAborted();
            m_progressReportSentry->flagNonCompletedWithErrorsAndAborted( false );
            // DTL-340 modify end
        }

        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);

        FUNCTION_EXIT;
    }

    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage TrainDVABroadcast::getTrainCoverage()
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        return m_theSpecifics.trains;
    }

    void TrainDVABroadcast::setHasOverrideOption( bool override )
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        m_theSpecifics.hasOverrideOption = override;
    }

    void TrainDVABroadcast::prepare()
    {
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
    }

} // namespace TA_IRS_App

