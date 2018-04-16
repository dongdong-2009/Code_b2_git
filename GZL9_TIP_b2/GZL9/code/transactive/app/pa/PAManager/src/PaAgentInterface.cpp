#include "app/pa/PAManager/src/stdafx.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaAgentInterface.h"

TA_Base_Core::NamedObject<TA_Base_Bus::IStationPaMethod, TA_Base_Bus::IStationPaMethod_ptr, TA_Base_Bus::IStationPaMethod_var>* PaAgentInterface::s_pPaAgentNamedObject = NULL;

void PaAgentInterface::createCorbaObject( const std::string& strEntityName )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        s_pPaAgentNamedObject = new TA_Base_Core::NamedObject<TA_Base_Bus::IStationPaMethod, TA_Base_Bus::IStationPaMethod_ptr, TA_Base_Bus::IStationPaMethod_var>();
        s_pPaAgentNamedObject->setEntityName( strEntityName, true );
    }
}

void PaAgentInterface::removeCorbaObject()
{
    if ( NULL != s_pPaAgentNamedObject )
    {
        delete s_pPaAgentNamedObject;
        s_pPaAgentNamedObject = NULL;
    }
}

int PaAgentInterface::startDVABroadcast( std::string& strBroadcasstId, 
										std::vector<unsigned long> vecZoneKeys, 
										std::vector<unsigned long> ulDVAMsgKeys, 
										int nCylicTimes,
										int nDwellTime,
										time_t tmStartTimeInDay )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    if ( 0 == vecZoneKeys.size() )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, WARNING_ZONE_NOT_SELECTED );
        return WARNING_ZONE_NOT_SELECTED;
    }

	/*
    if ( INVALID_DVA_MSG_KEY == ulDVAMsgKey )
    {
        // The DVA message should be select, otherwise the broadcast button is disable
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
        return ERROR_UNEXPECTED_LOGIC;
    }*/

	//Setting the values of the messages.
    TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq messages;
    messages.length( ulDVAMsgKeys.size() );
	
	for (unsigned int unLoop = 0u; unLoop < ulDVAMsgKeys.size(); ++unLoop )
	{
		messages[unLoop] = ulDVAMsgKeys[unLoop];
	}
	
	//Setting the values of the zones.
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for (unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }
    
	CORBA::UShort usDwellTime = static_cast<CORBA::UShort>(nDwellTime);
    try
    {
        if (( nCylicTimes > 0u ) || ( tmStartTimeInDay > 0u ))
        {
            TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams stSchedule;
			stSchedule.startTime = tmStartTimeInDay;
			stSchedule.stopTime = 0u;
            stSchedule.usCyclicTimes = static_cast<CORBA::UShort>(nCylicTimes);

            CORBA_CALL_RETURN( strBroadcasstId, (*s_pPaAgentNamedObject), broadcastStationScheduleDva,
                (messages, zones, usDwellTime, false, false, "", stSchedule, CachedConfig::getInstance()->getSessionId().c_str()) );
        }
        else
        {
            CORBA_CALL_RETURN( strBroadcasstId, (*s_pPaAgentNamedObject), broadcastStationDva,
                (messages, zones, usDwellTime, false, false, "", CachedConfig::getInstance()->getSessionId().c_str()) );
        }
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_START_DVA_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_START_DVA_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
        	break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_START_DVA_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startDVABroadcast success" );
    return SUCCESS_NO_ERROR;
}

//begin-added by lixiaoxia
int PaAgentInterface::startBroadcastMonitor( std::vector<unsigned long> vecZoneKeys)
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    if ( 0 == vecZoneKeys.size() )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, WARNING_ZONE_NOT_SELECTED );
        return WARNING_ZONE_NOT_SELECTED;
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }

    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), enableZoneMonitoring, 
            ( true, zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_START_MONITOR_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_START_MONITOR_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_START_MONITOR_BROADCAST;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_START_MONITOR_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startMonitorDVABroadcast success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::stopBroadcastMonitor( std::vector<unsigned long> vecZoneKeys )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }

    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), enableZoneMonitoring, 
            ( false, zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_STOP_MONITOR_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_STOP_MONITOR_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_STOP_MONITOR_BROADCAST;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_STOP_MONITOR_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopDVABroadcastMonitor success" );
    return SUCCESS_NO_ERROR;
}
//end-added by lixiaoxia

int PaAgentInterface::stopDVABroadcast( std::vector<unsigned long> vecZoneKeys )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }

    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), stopBroadcastStationDva, 
            ( zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_STOP_DVA_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_STOP_DVA_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_STOP_DVA_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopDVABroadcast success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::startDVAMsgPlayback( std::string& strBroadcasstId, unsigned long ulDVAMsgKey )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    if ( INVALID_DVA_MSG_KEY == ulDVAMsgKey )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
        return ERROR_UNEXPECTED_LOGIC;
    }

    CORBA::ULong ulCorbaMsgKey = static_cast<CORBA::ULong>(ulDVAMsgKey);

    try
    {
        CORBA_CALL_RETURN( strBroadcasstId, (*s_pPaAgentNamedObject), playbackStationDva, 
            ( ulCorbaMsgKey, 0u, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_START_DVA_MSG_PLAYBACK;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startDVAMsgPlayback success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::stopDVAMsgPlayback()
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), stopPlaybackStationDva, ( CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_STOP_DVA_MSG_PLAYBACK;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopDVAMsgPlayback success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::startLiveBroadcast( std::string& strBroadcasstId, std::vector<unsigned long> vecZoneKeys )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }

    try
    {
        CORBA_CALL_RETURN( strBroadcasstId, (*s_pPaAgentNamedObject), broadcastStationLive, 
            ( false, 0, zones, "", CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_START_LIVE_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_START_LIVE_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_START_LIVE_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startLiveBroadcast success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::stopLiveBroadcast( std::vector<unsigned long> vecZoneKeys )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length( vecZoneKeys.size() );
    for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
    {
        zones[unLoop] = vecZoneKeys[unLoop];
    }

    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), stopBroadcastStationLive, 
            ( zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_STOP_LIVE_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_STOP_LIVE_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_STOP_LIVE_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopLiveBroadcast success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::startBackgroundMusicBroadcast( std::vector<unsigned long> vecZoneKeys )
{
	if ( NULL == s_pPaAgentNamedObject )
	{
		PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
		return ERROR_UNEXPECTED_NULL_POINTER;
	}

	TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
	zones.length( vecZoneKeys.size() );
	for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
	{
		zones[unLoop] = vecZoneKeys[unLoop];
	}

	TA_Base_Bus::IPAAgentCorbaDef::EMusicType musicType = CachedConfig::getInstance()->isAtOcc() ? TA_Base_Bus::IPAAgentCorbaDef::OccMusic : TA_Base_Bus::IPAAgentCorbaDef::LocalMusic;
	
	try
	{
		CORBA_CALL( (*s_pPaAgentNamedObject), broadcastStationMusic, 
			( musicType, zones, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.reason );
        return ERROR_FAILED_TO_START_BACKGROUND_MUSIC;
    }
	catch ( TA_Base_Core::BadParameterException& expWhat )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.reason );
		return ERROR_FAILED_TO_START_BACKGROUND_MUSIC;
	}
	catch ( CORBA::SystemException& expCorbaException )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
		return ERROR_FAILED_TO_START_BACKGROUND_MUSIC;
	}
	catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
		return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
	catch( ... )
	{
		PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
		return ERROR_FAILED_TO_START_BACKGROUND_MUSIC;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startBackgroundMusicBroadcast success" );
	return SUCCESS_NO_ERROR;
}

int PaAgentInterface::stopBackgroundMusicBroadcast( std::vector<unsigned long> vecZoneKeys )
{
	if ( NULL == s_pPaAgentNamedObject )
	{
		PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
		return ERROR_UNEXPECTED_NULL_POINTER;
	}

	TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
	zones.length( vecZoneKeys.size() );
	for ( unsigned int unLoop = 0u; unLoop < vecZoneKeys.size(); ++unLoop )
	{
		zones[unLoop] = vecZoneKeys[unLoop];
	}

	try
	{
		CORBA_CALL( (*s_pPaAgentNamedObject), broadcastStationMusic, 
            (TA_Base_Bus::IPAAgentCorbaDef::NoMusic, zones, CachedConfig::getInstance( )->getSessionId( ).c_str( )) );
	}
	catch ( TA_Base_Core::OperationModeException& expOpModeException )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
		return ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC;
	}
	catch ( CORBA::SystemException& expCorbaException )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
		return ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC;
	}
	catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
	{
		PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
		return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure:
            nResult = ERROR_FAILED_TO_CONNECT_TO_HARDWARE;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_PA_FEP_CONFIG_NOT_FOUND;
            break;
        }
        return nResult;
    }
	catch( ... )
	{
		PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
		return ERROR_FAILED_TO_STOP_BACKGROUND_MUSIC;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopBackgroundMusicBroadcast success" );
	return SUCCESS_NO_ERROR;
}

int PaAgentInterface::getBackupKeyStatus( const std::string& strBackupName, int& nBackupKey )
{
    nBackupKey = BACKUP_KEY_UNKNOWN_STATUS;

    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    TA_Base_Bus::IPAAgentCorbaDef::EBackupKeyState eBackupKeyStatus = TA_Base_Bus::IPAAgentCorbaDef::BackupUnknow;
    try
    {
        CORBA_CALL_RETURN( eBackupKeyStatus, (*s_pPaAgentNamedObject), getBackupKeyStatus, ( strBackupName.c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& eOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, eOpModeException.reason );
        return ERROR_FAILED_TO_GET_BACKUP_KEY;
    }
    catch ( CORBA::SystemException& eCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, eCorbaException._name() );
        return ERROR_FAILED_TO_GET_BACKUP_KEY;
    }
    catch ( TA_Base_Core::ObjectResolutionException& eObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, eObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_GET_BACKUP_KEY;
    }

    switch( eBackupKeyStatus )
    {
        case TA_Base_Bus::IPAAgentCorbaDef::BackupUnknow:
        {
            nBackupKey = BACKUP_KEY_UNKNOWN_STATUS;
            break;
        }
        case TA_Base_Bus::IPAAgentCorbaDef::BackupLock:
        {
            nBackupKey = BACKUP_KEY_LOCK_MODE;
            break;
        }
        case TA_Base_Bus::IPAAgentCorbaDef::BackupUnlock:
        {
            nBackupKey = BACKUP_KEY_UNLOCK_MODE;
            break;
        }
        default:
        {
            break;
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getBackupKeyStatus success" );
    return SUCCESS_NO_ERROR;
}

int PaAgentInterface::removeScheduleDVABroadcast( unsigned long ulScheduleKey )
{
    if ( NULL == s_pPaAgentNamedObject )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return ERROR_UNEXPECTED_NULL_POINTER;
    }

    CORBA::ULong ulCorbaScheduleKey = static_cast<CORBA::ULong>(ulScheduleKey);
    try
    {
        CORBA_CALL( (*s_pPaAgentNamedObject), removeScheduleBroadcast, ( ulCorbaScheduleKey, CachedConfig::getInstance()->getSessionId().c_str() ) );
    }
    catch ( TA_Base_Core::OperationModeException& expOpModeException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expOpModeException.reason );
        return ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST;
    }
    catch ( CORBA::SystemException& expCorbaException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expCorbaException._name() );
        return ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST;
    }
    catch ( TA_Base_Core::ObjectResolutionException& expObjectResolution )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expObjectResolution.what() );
        return ERROR_FAILED_TO_SOLVE_AGENT_OBJECT;
    }
    catch ( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.strWhat.in() );

        int nResult = 0u;
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            nResult = ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            nResult = ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST;
            break;
        }
        return nResult;
    }
    catch( ... )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        return ERROR_FAILED_TO_REMOVE_SCHEDULE_DVA_BROADCAST;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "removeScheduleDVABroadcast success" );
    return SUCCESS_NO_ERROR;
}