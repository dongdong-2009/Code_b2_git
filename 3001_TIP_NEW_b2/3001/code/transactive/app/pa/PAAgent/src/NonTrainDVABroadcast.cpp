/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/NonTrainDVABroadcast.cpp $
* @author  A. Parker
* @version $Revision: #13 $
* Last modification : $DateTime: 2015/02/27 16:39:44 $
* Last modified by : $Author: qi.huang $
*
*
*/

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasZonesState.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/ScheduleDvaIdSupplier.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/StationPaSyncTisProxy.h"
#include "app/pa/PAAgent/src/NonTrainDVABroadcast.h"


namespace TA_IRS_App
{

NonTrainDVABroadcast::NonTrainDVABroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics(),
m_ulZoneStateUpdateTime( 0u ),
m_ulConsoleUpdateTime( 0u ),
m_mapLastTransactionState()
{
    FUNCTION_ENTRY( "NonTrainDVABroadcast" );

    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refConfig = 
        broadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig();

    // Set the initial effective coverage to be the entire list of zones
    // The false flag prevents the sending of the synchronization message
    setEffectiveCoverageIndices( calculateEffectiveCoverageIndices( refConfig.zones, refConfig.zones), false );

    PasHelpers::getInstance()->corbaConfigToStdConfig( refConfig, m_theSpecifics );

    // No audit message needed for local playback
    m_progressReportSentry->setInhibitAuditMessages(m_theSpecifics.isLocalPlayback);

    FUNCTION_EXIT;
}


NonTrainDVABroadcast::~NonTrainDVABroadcast()
{
    FUNCTION_ENTRY( "~NonTrainDVABroadcast" );

    PasTransactionManager::getInstance().abandonTransaction( *this );

    FUNCTION_EXIT;
}


void NonTrainDVABroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "validateBroadcastConfig" );

    TA_THREADGUARD( m_lock );

    // At least one zone must be specified
    if ( !m_theSpecifics.isLocalPlayback && m_theSpecifics.vecZoneKeys.size() == 0)
    {
        // log it and throw the exception
        // the calling method should catch this and log an audit message in one point
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station DVA broadcast failed - No zones requested" );
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "No zones requested" );
    }

    if ( m_theSpecifics.bScheduled && ( INVALID_SCHEDULE_EVENT_ID == m_theSpecifics.usScheduleId ))
    {
        // Apply the event id at execution
    }

    // At least one message must be specified
    if (m_theSpecifics.vecMsgKeys.size() == 0u)
    {
        // log it and throw the exception
        // the calling method should catch this and log an audit message in one point
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Station DVA broadcast failed - No messages requested" );
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "No messages requested" );
    }

    // Test validity of message keys in database
    for (std::vector<unsigned long>::iterator itLoop = m_theSpecifics.vecMsgKeys.begin();
        itLoop != m_theSpecifics.vecMsgKeys.end(); ++itLoop)
    {
        try
        {
            unsigned long messageId = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey( *itLoop );
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Station DVA broadcast failed - A requested message (%d) does not exist in the database",
                *itLoop );
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested message does not exist in the database" );
        }
    }


    if ( !m_theSpecifics.isLocalPlayback)
    {
        // Five possible cases:
        // 
        // 1.  startTime = 0 stopTime = 0
        // start now and only once
        // 
        // 2.  startTime = 0 stopTime !=0
        // start now and end at stopTime
        // 
        // 3.  startTime != 0 stopTime = 0
        // start at startTime and only once
        // 
        // 4.  startTime = stopTime != 0
        // start at startTime and only once
        // 
        // 5.  startTime != 0 stopTime != 0
        // start at startTime and stop at stopTime
        // Ensure that stopTime > startTime

        if ( m_theSpecifics.bScheduled )
        {
            unsigned long ulStartTime = static_cast<unsigned long>(m_theSpecifics.usStartTime) << 16u;
            ulStartTime |= static_cast<unsigned long>(m_theSpecifics.usStartSecond);
            unsigned long ulEndTime = static_cast<unsigned long>(m_theSpecifics.usEndTime) << 16u;
            ulEndTime |= static_cast<unsigned long>(m_theSpecifics.usEndSecond);

            // m_theSpecifics.startTime must be <= m_theSpecifics.stopTime
            if ( ulStartTime > ulEndTime )
            {
                // log it and throw the exception
                // the calling method should catch this and log an audit message in one point
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Station DVA broadcast failed - StartTime is greater than stopTime (%d > %d)",
                    ulStartTime, ulEndTime );
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "StartTime is greater than stopTime" );
            }
        }
    }

    //PIDs parameters verified process
    if(m_theSpecifics.isSynchronisedWithTis)
    {
        try
        {
            StationPaSyncTisProxy::getInstance().verifyTisParamters( m_theSpecifics.vecZoneKeys, m_theSpecifics.vecMsgKeys );
        }
        catch(TA_Base_Core::InvalidPasConfigurationException& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "PA-TIS synchronization configuration is not completed" );
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eZoneNotMapped, "PA-TIS synchronization configuration is not completed" );
        }
    }

    validateBroadcastConfigSpecifics();

    FUNCTION_EXIT;
}


void NonTrainDVABroadcast::execute()
{
    FUNCTION_ENTRY( "execute" );

    if ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult )
    {
        if ( !PasTransactionManager::getInstance().isConnectionAvailable() )
        {
            linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );
            return;
        }

        // Check and apply event id and check and apply transaction id
        if ( checkAndApplyEventId() && checkAndApplyTransactionId() )
        {
            executeSendStartCommand();
        }
    }
    else
    {
        executeWaitStartAcknoledge();
    }

    FUNCTION_EXIT;
}


void NonTrainDVABroadcast::executeSendStartCommand()
{
    FUNCTION_ENTRY( "executeSendStartCommand" );

    TA_THREADGUARD( m_lock );

    setWasExecutedSuccessfully(false);

    m_ulZoneStateUpdateTime = PasZonesState::getInstance().getZoneUpdateTime();
    m_nTransactionResult = writeStartToPas();

    if ( TRANSACTION_IN_MONITOR_MODE == m_nTransactionResult )
    {
        return;
    }
    else if ( TRANSACTION_CONNECTION_FAILURE == m_nTransactionResult )
    {
        // Serious error - terminate
        linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }
    else if ( INVALID_NULL_TRANSACTION == m_nTransactionResult )
    {
        // Serious error - terminate
        noneValidZoneTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }
    else if ( TRANSACTION_QUEUED != m_nTransactionResult )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected transaction result %d", m_nTransactionResult );
    }

    FUNCTION_EXIT;
}


void NonTrainDVABroadcast::executeSendStopCommand()
{
    FUNCTION_ENTRY( "executeSendStopCommand" );

    TA_THREADGUARD( m_lock );

    m_nTransactionResult = writeStopToPas();

    if ( TRANSACTION_IN_MONITOR_MODE == m_nTransactionResult )
    {
        return;
    }
    else if ( TRANSACTION_CONNECTION_FAILURE == m_nTransactionResult )
    {
        // Serious error - terminate
        linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }
    else if ( INVALID_NULL_TRANSACTION == m_nTransactionResult )
    {
        // Serious error - terminate
        abnormalTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }
    else if ( TRANSACTION_QUEUED != m_nTransactionResult )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected transaction result %d", m_nTransactionResult );
    }

    FUNCTION_EXIT;
}


bool NonTrainDVABroadcast::waitAcknowledge()
{
    bool bContinue = false;

    switch ( m_nTransactionResult )
    {
    case TRANSACTION_FAILED:
        {
            if ( TA_Base_Bus::IPAAgentCorbaDef::Execution == getBroadcastState() )
            {
                noneValidZoneTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT );
            }
            else
            {
                abnormalTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT );
            }

            break;
        }
    case TRANSACTION_CONNECTION_FAILURE:
        {
            linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );
            break;
        }
    case TRANSACTION_QUEUED:
    case TRANSACTION_NOT_EXECUTED:
    case TRANSACTION_IN_MONITOR_MODE:
        break;
    case TRANSACTION_SUCCESS:
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Transaction executed successfully" );
            // Only continue when success 
            bContinue = true;
            // Reset transaction result
            m_nTransactionResult = TRANSACTION_NOT_EXECUTED;
            break;
        }
    default:
        break;
    }

    return bContinue;
}


void NonTrainDVABroadcast::executeWaitStartAcknoledge()
{
    FUNCTION_ENTRY( "executeWaitStartAcknoledge" );

    if ( !waitAcknowledge() )
    {
        FUNCTION_EXIT;
        return;
    }

    // Flag to try to cleanup the announceId in terminate
    setWasExecutedSuccessfully(true);

    // All went well so we move to the runtime state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Runtime);

    if ( NULL != m_progressReportSentry )
    {
        m_progressReportSentry->resetGlobalState();
    }

    FUNCTION_EXIT;
}

void NonTrainDVABroadcast::executeWaitStopAcknoledge()
{
    FUNCTION_ENTRY( "executeWaitStopAcknoledge" );

    if ( !waitAcknowledge() )
    {
        FUNCTION_EXIT;
        return;
    }

    // All went well so we move to the terminate state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);

    FUNCTION_EXIT;
}

void NonTrainDVABroadcast::runtimeSpecifics()
{
    FUNCTION_ENTRY( "runtimeSpecifics" );

    TA_THREADGUARD( m_lock );

    // Reset execution state for retry function
    m_nTransactionResult = TRANSACTION_NOT_EXECUTED;

    if ( m_theSpecifics.bScheduled )
    {
        // Scheduled broadcast would not update zone state
        return;
    }

    // Check if it's time to terminate
    if ( !PasTransactionManager::getInstance().isConnectionAvailable() )
    {
        determineWhetherToRetryOrAbort( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }
    
    bool bBroadcastFinished = false;

    if ( m_theSpecifics.isLocalPlayback )
    {
        if ( 0 == m_ulConsoleUpdateTime )
        {
            // Not started in Monitor mode, should start now in Control
            m_ulConsoleUpdateTime = PasZonesState::getInstance().notifyReadPagingConsoleState( true );
        }
        else if ( PasZonesState::getInstance().isPagingConsoleUpdated( m_ulConsoleUpdateTime ))
        {
            static bool bPlaybacking = false;

            bool bPlaybackActive = PasZonesState::getInstance().isConsolePlaybacking( m_theSpecifics.usSourceId );

            if ( bPlaybacking && !bPlaybackActive )
            {
                // Playback activated and paging console is free again. implying Playback stop.
                bBroadcastFinished = true;
                bPlaybacking = false;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Playback is now inactive" );
            }
            else if ( !bPlaybacking && bPlaybackActive )
            {
                // Playback not activated and paging console is busy now. implying Playback start.
                bPlaybacking = true;

                if ( NULL != m_progressReportSentry )
                {
                    m_progressReportSentry->setLocalState( 0u, TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED );
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Playback is now active" );
            }
            else if ( !bPlaybacking && !bPlaybackActive )
            {
                // Playback not started, wait for a timeout and turn into terminate state to avoid runtime forever 
                // caused by no detection of playback state immediately changed from active to inactive

                // The timeout is being asked from PA team.
                const unsigned long lTimePassed = ACE_OS::gettimeofday().msec() - m_ulConsoleUpdateTime;

                if ( DEFAULT_NO_PLAYBACK_STATUS_TIMEOUT < lTimePassed )
                {
                    // Abnormal finish
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Playback state is not detected, terminate" );
                    bBroadcastFinished = true;
                    bPlaybacking = false;
                }
            }
            else
            {
                // Real playback active state, leave it to the next logic
            }
        }
        else
        {
            // Check timeout for no update from PAS, should turn into aborting state
            const unsigned long lTimePassed = ACE_OS::gettimeofday().msec() - m_ulConsoleUpdateTime;

            if ( DEFAULT_NO_PAS_STATUS_TIMEOUT < lTimePassed )
            {
                // Abnormal finish
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not read console status from PAS, terminate" );
                bBroadcastFinished = true;
            }
        }
    }
    else
    {
        // Check whether zone state is update to requested state
        if ( m_ulZoneStateUpdateTime != PasZonesState::getInstance().getZoneUpdateTime() )
        {
            updateCurrentlyPlayingZones();

            // Check zone broadcast is finished
            bBroadcastFinished = PasZonesState::getInstance().areZonesFreeOfTransaction( m_theSpecifics.vecZoneKeys, m_announceId );
        }
    }

    if ( bBroadcastFinished )
    {
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    }

    FUNCTION_EXIT;
}


void NonTrainDVABroadcast::updateCurrentlyPlayingZones()
{
    FUNCTION_ENTRY( "updateCurrentlyPlayingZones" );

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

    MapZoneKeyToTransactionState mapTempTransactionState = m_mapLastTransactionState;

    // Initialize new transaction state map
    if ( m_mapLastTransactionState.empty() )
    {
        for (unsigned int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
        {
            unsigned long ulZoneKey = m_theSpecifics.vecZoneKeys[effectiveCoverageIndices[indexIndex]];
            m_mapLastTransactionState.insert( MapZoneKeyToTransactionState::value_type(ulZoneKey, PAS_FREE_TRANSACTION) );
        }
    }

    // Refresh current playing zones state
    PasZonesState::getInstance().getZoneStatesByZoneKey( m_mapLastTransactionState );

    // Get different transaction states
    MapZoneKeyToTransactionState mapDiffTransactionState;
    for ( MapZoneKeyToTransactionState::iterator itLoop = m_mapLastTransactionState.begin(); m_mapLastTransactionState.end() != itLoop; ++itLoop )
    {
        MapZoneKeyToTransactionState::iterator itFound = mapTempTransactionState.find( itLoop->first );
        if (( mapTempTransactionState.end() == itFound ) || ( itFound->second != itLoop->second ))
        {
            mapDiffTransactionState.insert( MapZoneKeyToTransactionState::value_type(itLoop->first, itLoop->second) );
        }
    }
    // check whether zone state changed
    if( !mapDiffTransactionState.empty())
    {
        for( unsigned int indexIndex = 0; indexIndex < effectiveCoverageIndices.length(); ++indexIndex )
        {
            unsigned long zoneKey = m_theSpecifics.vecZoneKeys[effectiveCoverageIndices[indexIndex]];

            MapZoneKeyToTransactionState::iterator itDiffTransactionState = mapDiffTransactionState.find(zoneKey);
            if ( mapDiffTransactionState.end() != itDiffTransactionState )
            {
                if ( m_progressReportSentry->getErrorOccurred( effectiveCoverageIndices[indexIndex] ))
                {
                    continue;
                }

                if ( PasZonesState::getInstance().isZoneStateFault( itDiffTransactionState->second ))
                {
                    // Faulty zone
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE );
                }
                else
                {
                    TA_Base_Bus::IPAAgentCorbaDef::ELocalState eState = m_progressReportSentry->getLocalState(effectiveCoverageIndices[indexIndex]);
                    bool isBroadcasting = (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_BROADCASTING == eState);
                    if ( isBroadcasting )
                    {
                        if ( PasZonesState::getInstance().isZoneFree( itDiffTransactionState->second ))
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED );
                        }
                        else if (PasZonesState::getInstance().isZoneFreeOfTransaction( itDiffTransactionState->second, m_announceId ))
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN );
                        }
                        else
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_BROADCASTING );
                        }
                    }
                    else
                    {
                        if (!PasZonesState::getInstance().isZoneFreeOfTransaction( itDiffTransactionState->second, m_announceId ))
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_BROADCASTING );
                        }
                        else 
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE );
                        }
                    }
                }
            }
        }
    }

    FUNCTION_EXIT;
}

bool NonTrainDVABroadcast::shouldSendStopCommand()
{
    FUNCTION_ENTRY( "shouldSendStopCommand" );

    // First check whether start command send to PAS successfully
    if ( !getWasExecutedSuccessfully() )
    {
        // Terminate if not send to PAS and not queue in TransactionManager
        if ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult )
        {
            setBroadcastState( TA_Base_Bus::IPAAgentCorbaDef::Termination );
            return false;
        }
        else
        {
            // Remove from TransactionManager's queue once
            static bool bShouldRemove = true;
            if ( bShouldRemove )
            {
                bShouldRemove = false;
                // If not executing or executed, remove it; otherwise wait acknoledge
                if ( PasTransactionManager::getInstance().removeTransaction( *this ))
                {
                    setBroadcastState( TA_Base_Bus::IPAAgentCorbaDef::Termination );
                    return false;
                }
            }

            // Failed to remove from queue, transaction must be executing. Wait for execution ack
            if ( waitAcknowledge() )
            {
                // Reset should remove flag for broadcast retry function
                bShouldRemove = true;

                // Execution acknowledge with successfully
                setWasExecutedSuccessfully( true );
            }
            else
            {
                return false;
            }
        }
    }

    FUNCTION_EXIT;
    return true;
}

void NonTrainDVABroadcast::aborting()
{
    FUNCTION_ENTRY( "aborting" );

    if ( !shouldSendStopCommand() )
    {
        FUNCTION_EXIT;
        return;
    }

    if ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult )
    {
        executeSendStopCommand();
    }
    else
    {
        executeWaitStopAcknoledge();
    }

    FUNCTION_EXIT;
}

void NonTrainDVABroadcast::terminate()
{
    FUNCTION_ENTRY( "terminate" );

    TA_THREADGUARD( m_lock );

    m_mapLastTransactionState.clear();

    // Reset execution state for retry function
    m_nTransactionResult = TRANSACTION_NOT_EXECUTED;
    // Reset the flag without notify
    setWasExecutedSuccessfully( false );

    //added by lixiaoxia
    setAnnouncementId( PAS_FREE_TRANSACTION );

    if ( m_theSpecifics.bScheduled )
    {
        ScheduleDvaIdSupplier::getInstance().refreshEventIdAvaliableState( m_theSpecifics.usScheduleId );
        updateScheduleDvaEventId( INVALID_SCHEDULE_EVENT_ID );
    }
    else if ( m_theSpecifics.isLocalPlayback )
    {
        if ( 0 < m_ulConsoleUpdateTime )
        {
            PasZonesState::getInstance().notifyReadPagingConsoleState( false );
            m_ulConsoleUpdateTime = 0;
        }
    }

    try
    {
        updateFinalProgressReport();
    }
    catch(...)
    {
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason For updateFinalProgressReport" );
    }

    // All went well so we move to the pending removal state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);

    FUNCTION_EXIT;
}

void NonTrainDVABroadcast::transactionExecuted( int nResult )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "transaction executed with result %d", nResult );
    m_nTransactionResult = nResult;
}

void NonTrainDVABroadcast::updateScheduleDvaEventId( const unsigned short usEventId, const bool bSendSyncNotify )
{
    if ( usEventId == m_theSpecifics.usScheduleId )
    {
        return;
    }

    unsigned short usOldEventId = m_theSpecifics.usScheduleId;

    m_theSpecifics.usScheduleId = usEventId;
    m_broadcastConfigV->theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usScheduleId = usEventId;

    if ( bSendSyncNotify )
    {
        CORBA::Any data;
        data <<= usEventId;

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SCHEDULE_DVA_EVENT_ID, data, getBroadcastId().c_str());
    }
    else
    {
        // Updated in Monitor Mode, should update the ID supplier also
        ScheduleDvaIdSupplier::getInstance().changeEventId( usOldEventId, usEventId );
    }
}

bool NonTrainDVABroadcast::isScheduleDvaEventIdValid()
{
    if ( !m_theSpecifics.bScheduled )
    {
        // Normal Dva, no need to get schedule Dva Id
        return true;
    }
    else if ( INVALID_SCHEDULE_EVENT_ID == m_theSpecifics.usScheduleId )
    {
        // Not synchronized with PAS yet, retrying
        return false;
    }

    return true;
}

TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* NonTrainDVABroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = AbstractStationBroadcast::getBroadcastParametersCopy();

    int originalLength = parameters->length();
    parameters->length(originalLength+1);

    CORBA::Any data;
    data <<= m_theSpecifics.usScheduleId;
    (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SCHEDULE_DVA_EVENT_ID;
    (*parameters)[originalLength].data = data;

    return parameters;
}

void NonTrainDVABroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{
    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_SCHEDULE_DVA_EVENT_ID)
    {
        unsigned short updateData = 0u;
        parameter.data >>= updateData;

        updateScheduleDvaEventId( updateData, false );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "BroadcastParameter update received: Schedule DVA event Id=%d", updateData );
    }
    else
    {
        AbstractStationBroadcast::setBroadcastParameter(parameter);
    }
}

void NonTrainDVABroadcast::updateEffectiveZones( const std::vector<unsigned long>& vecNewZoneKeys )
{
    m_theSpecifics.vecZoneKeys = vecNewZoneKeys;
}

bool NonTrainDVABroadcast::isBusy()
{
    bool bBusy = AbstractStationBroadcast::isBusy();

    bBusy = ( bBusy && ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult ));

    return bBusy;
}

bool NonTrainDVABroadcast::isTransactionIdValid()
{
    if ( m_theSpecifics.bScheduled )
    {
        // Schedule Dva, no need to get Transaction Id
        return true;
    }
    else if ( PAS_FREE_TRANSACTION == m_announceId )
    {
        return false;
    }

    return true;
}

bool NonTrainDVABroadcast::checkAndApplyEventId()
{
    if ( !isScheduleDvaEventIdValid() )
    {
        unsigned short usEventId = ScheduleDvaIdSupplier::getInstance().applyAvailableEventId();

        if ( FAILURE_SCHEDULE_EVENT_ID == usEventId )
        {
            // Schedule DVA full, need to end this schedule
            noneValidZoneTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_SCHEDULE_DVA_FULL );
            return false;
        }
        else if ( INVALID_SCHEDULE_EVENT_ID == usEventId )
        {
            // Schedule Id is not ready yet
            return false;
        }

        updateScheduleDvaEventId( usEventId );
    }

    return true;
}

bool NonTrainDVABroadcast::checkAndApplyTransactionId()
{
    if( !isTransactionIdValid() )
    {
        unsigned short usTransationid = PAS_FREE_TRANSACTION;

        try
        {
            usTransationid = getNextAvailableStationAnnouncementId();
        }
        catch(TA_Base_Core::NoFreeAnnounceIdException&)
        {
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);
            return false;
        }

        setAnnouncementId( usTransationid );
    }

    return true;
}

void NonTrainDVABroadcast::prepare()
{
    if( m_theSpecifics.isSynchronisedWithTis )
    {
        int globleState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE;

        StationPaSyncTisProxy::getInstance().syncTisBroadcast( m_theSpecifics.vecZoneKeys, m_theSpecifics.vecMsgKeys, m_theSpecifics.ulOriginalStartTime, m_theSpecifics.ulOriginalEndTime, getSessionId(), globleState );
        if(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE != globleState)
        {
            m_progressReportSentry->setGlobalState( static_cast<TA_Base_Bus::IPAAgentCorbaDef::EGlobalState>(globleState) );
        }
    }

    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
}

} // namespace TA_IRS_App

