/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/DVARecordingBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #9 $
  * Last modification : $DateTime: 2016/06/16 18:45:36 $
  * Last modified by : $Author: hongzhi.zhang $
  *
  *
  */

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/StartDvaRecording.h"
#include "app/pa/PAAgent/src/StopDvaRecording.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasZonesState.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
//#include "app/pa/PAAgent/src/BroadcastManager.h"


namespace TA_IRS_App
{

DVARecordingBroadcast::DVARecordingBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics(),
m_isConnected( false ),
m_ulConsoleUpdateTime( 0u ),
m_ulRecordingCounting( 0u )
{
    PasHelpers::getInstance()->corbaConfigToStdConfig( 
        broadcastConfigP->theBroadcastConfigSpecifics.theStationRecordingConfig(), m_theSpecifics );

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices seqCoverage;
    seqCoverage.length( 1 );
    seqCoverage[0] = 0;
    setEffectiveCoverageIndices( seqCoverage, false );
}

DVARecordingBroadcast::~DVARecordingBroadcast() 
{
    PasTransactionManager::getInstance().abandonTransaction( *this );
}

void DVARecordingBroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) 
{
    TA_THREADGUARD( m_lock );

    // Source Id needs to be from a paging console or a spare:
    if ( !PasHelpers::getInstance()->isSourcePagingConsole( m_theSpecifics.usSourceId, getSessionId() ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The specified source is not a paging console");
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified source is not a paging console" );
    }

    if ( !PasHelpers::getInstance()->isAdHocDvaMsg( m_theSpecifics.usMsgType ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The requested message is not a valid Ad-Hoc DVA message");
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The requested message is not a valid Ad-Hoc DVA message" );
    }

    // Should not send the blank ad-hoc type to PAS, fix it to normal ad-hoc type
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == m_theSpecifics.usMsgType )
    {
        m_theSpecifics.usMsgType = TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL;
    }

    // Message can only be from this location 
    try
    {
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageLocationKeyFromKey(m_theSpecifics.ulMsgKey);
        if (locationKey != CachedConfig::getInstance()->getAgentLocationKey())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The requested message is not within this station's scope");
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The requested message is not within this station's scope" );
        }
    }
    catch( TA_Base_Core::CachedMappingNotFoundException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The specified message is not an ad-hoc message");
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The requested message does not exist in the database" );
    }

    // Message id only be in the range of adhoc messages
    if ( (m_theSpecifics.usMsgId < TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_AD_HOC_DVA_ID) || (m_theSpecifics.usMsgId > TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_AD_HOC_DVA_ID) )
    {
        std::stringstream errStr;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Message ID is invalid: %ld", m_theSpecifics.usMsgId);
        errStr << "The specified message's id is not in range of " << TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_AD_HOC_DVA_ID
               << " to " << TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_AD_HOC_DVA_ID;
        PasHelpers::getInstance()->throwPaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, errStr.str() );
    }
}

void DVARecordingBroadcast::execute() 
{
	FUNCTION_ENTRY( "execute" );

	if ( TRANSACTION_NOT_EXECUTED == m_nTransactionResult )
	{
		executeSendStartCommand();
	}
	else
	{
		executeWaitStartAcknoledge();
	}

	FUNCTION_EXIT;
}

void DVARecordingBroadcast::runtimeSpecifics() 
{
    TA_THREADGUARD( m_lock );

    // Check if it's time to terminate
    if ( !PasTransactionManager::getInstance().isConnectionAvailable() )
    {
        determineWhetherToRetryOrAbort( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );
        return;
    }

    bool bRealPttActive = true;
    
    if ( 0u == m_ulConsoleUpdateTime )
    {
        // Not started in Monitor mode, should start now in Control
        m_ulConsoleUpdateTime = PasZonesState::getInstance().notifyReadPagingConsoleState( true );
        m_ulRecordingCounting = 0u;
    }
    else if ( PasZonesState::getInstance().isPagingConsoleUpdated( m_ulConsoleUpdateTime ))
    {
        bool bPttActive = PasZonesState::getInstance().isConsoleRecording( m_theSpecifics.usSourceId );

        if ( m_isConnected && !bPttActive )
        {
            // Ad-Hoc recording activated and paging console is free again. implying Ad-Hoc recording stop.
            bRealPttActive = false;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ad-Hoc recording is now inactive" );
        }
        else if ( !m_isConnected && bPttActive )
        {
            // Ad-Hoc recording not activated and paging console is busy now. implying Ad-Hoc recording start.
            setIsConnected( true );
            m_ulRecordingCounting = ACE_OS::gettimeofday().msec();

            if ( NULL != m_progressReportSentry )
            {
                m_progressReportSentry->setLocalState( 0u, TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ad-Hoc recording is now active" );
        }
        else if ( !m_isConnected && !bPttActive )
        {
            // Ad-Hoc not start recording, wait for a timeout and turn into terminate/aborting state to avoid runtime forever 
            // caused by no detection of PTT immediately changed from active to inactive (abnormal) or 
            // operator not press PTT for quite a long time (normal) [can not distinguish]

            // The timeout is being asked from PA team.
            const unsigned long ulTimePassed = ACE_OS::gettimeofday().msec() - m_ulConsoleUpdateTime;

            if ( DEFAULT_NO_PTT_STATUS_TIMEOUT < ulTimePassed )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PTT state is not detected, aborting" );
                setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Aborting);
                return;
            }
        }
        else
        {
            // Real PTT active state, leave it to the next logic
            const unsigned long ulTimeCounting = ACE_OS::gettimeofday().msec() - m_ulRecordingCounting;

            // PTT active for 30 seconds, should terminate the ad-hoc recording
            if ( CachedConfig::getInstance()->getPasDvaRecordingMaxDurationInSecond() * 1000 <= ulTimeCounting )
            {
                bRealPttActive = false;
            }
        }
    }
    else
    {
        // Check timeout for no update from PAS, should turn into aborting state
        const unsigned long ulTimePassed = ACE_OS::gettimeofday().msec() - m_ulConsoleUpdateTime;

        if ( DEFAULT_NO_PAS_STATUS_TIMEOUT < ulTimePassed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not read console status from PAS, aborting" );
            setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Aborting);
            return;
        }
    }

    if ( bRealPttActive )
    {
        // Operator is still talking so we cannot terminate yet.
        return;
    }

    // The operator has released the PTT button so we move to the termination state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    // terminate will be called by the BroadcastWorker().
}


void DVARecordingBroadcast::terminate() 
{
    TA_THREADGUARD( m_lock );

    // Reset execution state for retry function
    m_nTransactionResult = TRANSACTION_NOT_EXECUTED;

    // Reset the connected flag so a new execution will send a new request to PAS
    setIsConnected( false );

    m_ulRecordingCounting = 0u;

    if ( 0u < m_ulConsoleUpdateTime )
    {
        PasZonesState::getInstance().notifyReadPagingConsoleState( false );
        m_ulConsoleUpdateTime = 0u;
    }

    updateFinalProgressReport();

    // All went well so we move to the pending removal state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
}

bool DVARecordingBroadcast::shouldSendStopCommand()
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

void DVARecordingBroadcast::aborting()
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

int DVARecordingBroadcast::writeStartToPas()
{
    StartDvaRecording* pStartMasterDVARecording = new StartDvaRecording( CachedConfig::getInstance()->getIsMaster(), this );

	if ( NULL == pStartMasterDVARecording )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterDvaBroadcast" );
		return TRANSACTION_FAILED;
	}
	pStartMasterDVARecording->setMsgType( m_theSpecifics.usMsgType );
	pStartMasterDVARecording->setSourceId( m_theSpecifics.usSourceId );
	pStartMasterDVARecording->setDvaMessageId( m_theSpecifics.usMsgId );

	return PasTransactionManager::getInstance().addTransaction( pStartMasterDVARecording );
}

int DVARecordingBroadcast::writeStopToPas()
{
	StopDvaRecording* pStopMasterDVARecording = new StopDvaRecording( CachedConfig::getInstance()->getIsMaster(), this );

	if ( NULL == pStopMasterDVARecording )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterDvaBroadcast" );
		return TRANSACTION_FAILED;
	}
	pStopMasterDVARecording->setMsgType( m_theSpecifics.usMsgType );
	pStopMasterDVARecording->setSourceId( m_theSpecifics.usSourceId );
	pStopMasterDVARecording->setDvaMessageId( m_theSpecifics.usMsgId );

	return PasTransactionManager::getInstance().addTransaction( pStopMasterDVARecording );
}

void DVARecordingBroadcast::executeSendStartCommand()
{
	FUNCTION_ENTRY( "executeSendStartCommand" );

	TA_THREADGUARD( m_lock );

	setWasExecutedSuccessfully(false);

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

void DVARecordingBroadcast::executeSendStopCommand()
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

void DVARecordingBroadcast::executeWaitStartAcknoledge()
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

void DVARecordingBroadcast::executeWaitStopAcknoledge()
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

bool DVARecordingBroadcast::waitAcknowledge()
{
	bool bContinue = false;

	switch ( m_nTransactionResult )
	{
	case TRANSACTION_FAILED:
		abnormalTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT );
		break;
    case TRANSACTION_CONNECTION_FAILURE:
        linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT );
		break;
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

void DVARecordingBroadcast::transactionExecuted( int nResult )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "transaction executed with result %d", nResult );
	m_nTransactionResult = nResult;
}

void DVARecordingBroadcast::prepare()
{
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
}

} // namespace TA_IRS_App

