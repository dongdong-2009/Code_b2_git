/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StationMusicBroadcast.cpp $
* @author  A. Parker
* @version $Revision: #4 $
* Last modification : $DateTime: 2012/11/29 22:02:45 $
* Last modified by : $Author: qi.huang $
*
*
*/

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/PasExceptions.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/StartBackgroundMusic.h"
#include "app/pa/PAAgent/src/StopBackgroundMusic.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
//#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"



namespace TA_IRS_App
{

StationMusicBroadcast::StationMusicBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics()
{
    TA_Base_Bus::IPAAgentCorbaDef::StationMusicConfig& refConfig = 
        broadcastConfigP->theBroadcastConfigSpecifics.theStationMusicConfig();
    PasHelpers::getInstance()->corbaConfigToStdConfig( refConfig, m_theSpecifics );
}

StationMusicBroadcast::~StationMusicBroadcast() 
{
}

void StationMusicBroadcast::validateBroadcastConfig() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) 
{
    // Source Id has been checked by the broadcast manager
    // Zones is not relevant
}

void StationMusicBroadcast::execute() 
{
    TA_THREADGUARD( m_lock );

    setWasExecutedSuccessfully(false);

    int nExecutionResult = TRANSACTION_NOT_EXECUTED;

    if ( TA_Base_Bus::IPAAgentCorbaDef::NoMusic == m_theSpecifics.musicType )
    {
        // Only the correct type should be terminated.
        // At this moment, all broadcast are station broadcast which are triggered in station/DCC
        StopBackgroundMusic oStopBackgroundMusic( CachedConfig::getInstance()->getIsMaster(), this );

        oStopBackgroundMusic.setSourceNumber(m_theSpecifics.usSourceId);

        nExecutionResult = PasTransactionManager::getInstance().executeTransaction( oStopBackgroundMusic, false );
    }
    else
    {
        // Local or OCC music
        StartBackgroundMusic oStartBackgroundMusic( CachedConfig::getInstance()->getIsMaster(), this );

        // Only play music at station should set source number but can still set it
        oStartBackgroundMusic.setSourceNumber(m_theSpecifics.usSourceId);
        oStartBackgroundMusic.setPasZoneKeys(m_theSpecifics.vecZoneKeys);

        nExecutionResult = PasTransactionManager::getInstance().executeTransaction( oStartBackgroundMusic, false );
    }

    bool bContinue = false;

    switch ( nExecutionResult )
    {
    case TRANSACTION_FAILED:
        PasHelpers::getInstance()->throwPaErrorException(TA_Base_Bus::IPAAgentCorbaDef::eBroadcastStationMusicFailure, "Transaction failed");
        break;
    case TRANSACTION_CONNECTION_FAILURE:
        PasHelpers::getInstance()->throwPaErrorException(TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure, "Transaction connection failed");
        break;
    case TRANSACTION_IN_MONITOR_MODE:
        break;
    case TRANSACTION_SUCCESS:
        bContinue = true;
        break;
    default:
        break;
    }

    // Error occur or in monitor mode, should not turn to Runtime.
    if ( !bContinue )
    {
        return;
    }

    // We can skip to final state as this is a once off request with no runtime functionality
    // but we go with the flow to follow the other broadcast types
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Runtime);

    // Clear any past errors only for the transition from Execution to Runtime
    // In the case of the transition from Execution to Termination to PendingRemoval,
    // we want to retain the GlobalState so that the PA Manager can see the
    // errors encountered.

    m_progressReportSentry->resetGlobalState();

    setWasExecutedSuccessfully(true);
}

void StationMusicBroadcast::terminate() 
{
    // Does not apply
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
}

void StationMusicBroadcast::runtimeSpecifics() 
{
    // Does not apply
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
}

void StationMusicBroadcast::aborting()
{

}

int StationMusicBroadcast::writeStartToPas()
{
    return 0;
}

int StationMusicBroadcast::writeStopToPas()
{
    return 0;
}

void StationMusicBroadcast::transactionExecuted( int nResult )
{
	m_nTransactionResult = nResult;
}

void StationMusicBroadcast::prepare()
{
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
}

} // namespace TA_IRS_App

