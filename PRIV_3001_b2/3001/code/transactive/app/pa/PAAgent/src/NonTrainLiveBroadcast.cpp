/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/NonTrainLiveBroadcast.cpp $
* @author  HuangQi
* @version $Revision: #1 $
* Last modification : $DateTime: 2018/03/14 13:51:20 $
* Last modified by : $Author: lim.cy $
*
*
*/

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasZonesState.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/NonTrainLiveBroadcast.h"


namespace TA_IRS_App
{

NonTrainLiveBroadcast::NonTrainLiveBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics(),
m_bConnected( false ),
m_ulConsoleUpdateTime( 0u ),
m_ulZoneStateUpdateTime( 0u ),
m_mapLastTransactionState()
{
    FUNCTION_ENTRY( "NonTrainLiveBroadcast" );

    TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig& refConfig = 
        broadcastConfigP->theBroadcastConfigSpecifics.theStationLiveConfig();

    // Set the initial effective coverage to be the entire list of zones
    // The false flag prevents the sending of the synchronization message
    setEffectiveCoverageIndices( calculateEffectiveCoverageIndices( refConfig.zones, refConfig.zones), false );

    PasHelpers::getInstance()->corbaConfigToStdConfig( refConfig, m_theSpecifics );

    FUNCTION_EXIT;
}


NonTrainLiveBroadcast::~NonTrainLiveBroadcast()
{
    FUNCTION_ENTRY( "~NonTrainLiveBroadcast" );

    PasTransactionManager::getInstance().abandonTransaction( *this );

    FUNCTION_EXIT;
}


void NonTrainLiveBroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "validateBroadcastConfig" );

    TA_THREADGUARD( m_lock );

    // Source Id needs to be from a paging console or a spare:
    if ( !PasHelpers::getInstance()->isSourcePagingConsole( m_theSpecifics.usSourceId, getSessionId() ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The specified source is not a paging console");
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified source is not a paging console" );
    }

    // At least one zone must be specified
    if (m_theSpecifics.vecZoneKeys.size() == 0)
    {
        // log it and throw the exception
        // the calling method should catch this and log an audit message in one point
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station DVA broadcast failed - No zones requested" );
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "No zones requested" );
    }

    validateBroadcastConfigSpecifics();

    FUNCTION_EXIT;
}


void NonTrainLiveBroadcast::execute()
{
    FUNCTION_ENTRY( "execute" );

    if ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult )
    {
        if ( !PasTransactionManager::getInstance().isConnectionAvailable() )
        {
            linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );
            return;
        }

        // Check and apply transaction id
        if ( checkAndApplyTransactionId() )
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


void NonTrainLiveBroadcast::executeSendStartCommand()
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


void NonTrainLiveBroadcast::executeSendStopCommand()
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


bool NonTrainLiveBroadcast::waitAcknowledge()
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


void NonTrainLiveBroadcast::executeWaitStartAcknoledge()
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

void NonTrainLiveBroadcast::executeWaitStopAcknoledge()
{
    FUNCTION_ENTRY( "executeWaitStopAcknoledge" );

    if ( !waitAcknowledge() )
    {
        FUNCTION_EXIT;
        return;
    }

    // All went well so we move to the runtime state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);

    FUNCTION_EXIT;
}

void NonTrainLiveBroadcast::runtimeSpecifics()
{
    FUNCTION_ENTRY( "runtimeSpecifics" );

    TA_THREADGUARD( m_lock );

    // Reset execution state for retry function
    m_nTransactionResult = TRANSACTION_NOT_EXECUTED;

    // Check if it's time to terminate
    if ( !PasTransactionManager::getInstance().isConnectionAvailable() )
    {
        determineWhetherToRetryOrAbort( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );

        FUNCTION_EXIT;
        return;
    }

    if (!isPttActive())
    {
        return;
    }

    // Check whether zone state is update to requested state
    if ( m_ulZoneStateUpdateTime == PasZonesState::getInstance().getZoneUpdateTime() )
    {
        FUNCTION_EXIT;
        return;
    }

    updateCurrentlyPlayingZones();

    // Check zone broadcast is finished
    bool bAllZoneFinished = PasZonesState::getInstance().areZonesFreeOfTransaction(m_theSpecifics.vecZoneKeys,m_announceId);

    if ( bAllZoneFinished )
    {
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    }

    FUNCTION_EXIT;
}


void NonTrainLiveBroadcast::updateCurrentlyPlayingZones()
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
    if( !mapDiffTransactionState.empty() )
    {
        // update progress report structure for each zone
        for (unsigned int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
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
                    TA_Base_Bus::IPAAgentCorbaDef::ELocalState eState = m_progressReportSentry->getLocalState( effectiveCoverageIndices[indexIndex] );
                    bool isBroadcasting = (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_BROADCASTING == eState);
                    if(isBroadcasting)
                    {
                        if(PasZonesState::getInstance().isZoneFree(itDiffTransactionState->second))
                        {
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED );
                        }
                        else if(PasZonesState::getInstance().isZoneFreeOfTransaction(itDiffTransactionState->second,m_announceId))
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
                        if(!PasZonesState::getInstance().isZoneFreeOfTransaction(itDiffTransactionState->second,m_announceId))
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

bool NonTrainLiveBroadcast::shouldSendStopCommand()
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

void NonTrainLiveBroadcast::aborting()
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

void NonTrainLiveBroadcast::terminate()
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

    setIsConnected(false);

    if ( 0u < m_ulConsoleUpdateTime )
    {
        PasZonesState::getInstance().notifyReadPagingConsoleState( false );
        m_ulConsoleUpdateTime = 0u;
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

void NonTrainLiveBroadcast::transactionExecuted( int nResult )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "transaction executed with result %d", nResult );
    m_nTransactionResult = nResult;
}

void NonTrainLiveBroadcast::updateEffectiveZones( const std::vector<unsigned long>& vecNewZoneKeys )
{
    m_theSpecifics.vecZoneKeys = vecNewZoneKeys;
}

bool NonTrainLiveBroadcast::isBusy()
{
    bool bBusy = AbstractStationBroadcast::isBusy();

    bBusy = ( bBusy && ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult ));

    return bBusy;
}

//added by lixiaoxia
bool NonTrainLiveBroadcast::isTransactionIdValid()
{
    return ( PAS_FREE_TRANSACTION != m_announceId );
}

bool NonTrainLiveBroadcast::checkAndApplyTransactionId()
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

void NonTrainLiveBroadcast::prepare()
{
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
}

bool NonTrainLiveBroadcast::isPttActive()
{
    if (m_bConnected)
    {
        return true;
    }

    bool bRealPttActive = false;

    if ( 0u == m_ulConsoleUpdateTime )
    {
        // Not started in Monitor mode, should start now in Control
        m_ulConsoleUpdateTime = PasZonesState::getInstance().notifyReadPagingConsoleState( true );
    }
    else if ( PasZonesState::getInstance().isPagingConsoleUpdated( m_ulConsoleUpdateTime ) )
    {
        if ( PasZonesState::getInstance().isConsoleLiving(m_theSpecifics.usSourceId) )
        {
            setIsConnected(true);
            bRealPttActive = true;
            PasZonesState::getInstance().notifyReadPagingConsoleState( false );
            m_ulConsoleUpdateTime = 0u;
        }
    }

    return bRealPttActive;
}

inline bool NonTrainLiveBroadcast::getIsConnected()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_bConnected;
}

inline void NonTrainLiveBroadcast::setIsConnected(bool bValue, bool sendSynchronisation/*=true*/)
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    if ( m_bConnected == bValue )
    {
        return;
    }

    m_bConnected = bValue;

    if (sendSynchronisation)
    {
        CORBA::Any data;
        data <<= CORBA::Any::from_boolean(bValue);

        PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
            (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED, data, getBroadcastId().c_str());
    }
}

TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* NonTrainLiveBroadcast::getBroadcastParametersCopy()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters = 
        AbstractStationBroadcast::getBroadcastParametersCopy();

    int originalLength = parameters->length();
    parameters->length(originalLength+1);

    CORBA::Any data1;
    data1 <<= CORBA::Any::from_boolean(getIsConnected());
    (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
    (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED;
    (*parameters)[originalLength].data        = data1;

    return parameters;
}

void NonTrainLiveBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
{
    if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_IS_CONNECTED)
    {
        bool updateData = false;

        if ( 0 != (parameter.data >>= CORBA::Any::to_boolean(updateData) ))
        {
            setIsConnected(updateData, false);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                "BroadcastParameter update received: IsConnected=%s", updateData?"true":"false");
        }
    }
    else
    {
        AbstractStationBroadcast::setBroadcastParameter(parameter);
    }
}

} // namespace TA_IRS_App

