/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasHelpers.cpp $
  * @author:  Ripple
  * @version: $Revision: #38 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h" // TD19669
#include "core/exceptions/src/PasExceptions.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/PAAgentAlarms_MessageTypes.h"
#include "core/message/types/PAAgentAudit_MessageTypes.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/types/PAAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdate_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

#include <ace/OS.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>


using namespace TA_Base_Core;
using namespace TA_IRS_Bus;


namespace TA_IRS_App
{

AuditMessageSender* PasHelpers::m_auditSender = NULL;
CommsMessageSender*                 PasHelpers::m_paAgentCommsSender = NULL;
CachedConfig*                       PasHelpers::m_cachedConfigInstance = NULL;
TA_Base_Core::StateUpdateMessageSender*	PasHelpers::m_stateUpdateMessageSender = NULL;
NonReEntrantThreadLockable          PasHelpers::m_lock;
PasHelpers*                         PasHelpers::m_me = NULL;

PasHelpers::PasHelpers()
{
    FUNCTION_ENTRY( "PasHelpers" );

    if (!m_auditSender)
    {
        m_auditSender = 
            MessagePublicationManager::getInstance().getAuditMessageSender( PAAgentAudit::Context );
    }
   
    if (!m_stateUpdateMessageSender)
    {
        m_stateUpdateMessageSender = 
            MessagePublicationManager::getInstance().getStateUpdateMessageSender(PAAgentStateUpdate::Context);
    }

    if (!m_paAgentCommsSender)
    {
        m_paAgentCommsSender = 
            MessagePublicationManager::getInstance().getCommsMessageSender( PAAgentComms::Context );
    }

    if (!m_cachedConfigInstance)
    {
        m_cachedConfigInstance = CachedConfig::getInstance();
    }

//TD18607
	m_pasAlarm = "";
//TD18607

    FUNCTION_EXIT;
}


PasHelpers::~PasHelpers()
{
    FUNCTION_ENTRY( "~PasHelpers" );

    // liuyu++ TD4657
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    // ++liuyu TD4657
//TD18607
	m_pasAlarm = "";
//TD18607

    if ( m_stateUpdateMessageSender != NULL )
    {
	    delete m_stateUpdateMessageSender;
	    m_stateUpdateMessageSender = NULL;
    }

    if (m_auditSender)
    {
        delete m_auditSender;
        m_auditSender = NULL;
    }

    
    if (m_paAgentCommsSender)
    {
        delete m_paAgentCommsSender;
        m_paAgentCommsSender = NULL;
    }

    FUNCTION_EXIT;
}


PasHelpers* PasHelpers::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

		ThreadGuard guard( m_lock );

		if ( 0 == m_me )
		{
			m_me = new PasHelpers();
    	}
	}

    FUNCTION_EXIT;
	return m_me;
}


void PasHelpers::removeInstance( )
{
    FUNCTION_ENTRY( "removeInstance" );

	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lock );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}

    FUNCTION_EXIT;
}


std::string PasHelpers::getProgressLogStr( AbstractBroadcast& broadcast)
{
	std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::ProgressReport> prgressReport(broadcast.getProgressReportCopy());
	if(prgressReport.get() == NULL)
	{
		return "prgressReport is NULL";
	}
	std::ostringstream ss;
	ss << "BroadcastID = " << prgressReport->broadcastId.in() << " BroadcastState = " << prgressReport->globalState << " LocalState = ";
	for(int i = 0, sz = prgressReport->localProgresses.length(); i < sz; ++i)
	{
		ss << prgressReport->localProgresses[i].zoneKeyOrTrainId << ":" << prgressReport->localProgresses[i].localState << " ";
	}
	return ss.str();
}

void PasHelpers::zoneKeysToCoverage(const Keys& keys, TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage)
{
    FUNCTION_ENTRY( "zoneKeysToCoverage" );

    coverage.length(keys.size());
    int i=0;
    for (Keys::const_iterator it=keys.begin(); it!=keys.end(); ++it)
    {
        coverage[i++]=*it;
    }

    FUNCTION_EXIT;
}


void PasHelpers::coverageToZoneKeys(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& coverage, Keys& keys )
{
    FUNCTION_ENTRY( "coverageToZoneKeys" );

    keys.clear();

    for (int i=0; i< coverage.length(); ++i)
    {
        keys.push_back(coverage[i]);   
    }

    FUNCTION_EXIT;
}


bool PasHelpers::isSourcePagingConsole(unsigned short sourceId, const std::string& session)
{
    FUNCTION_ENTRY( "isSourcePagingConsole" );

    try
    {
		IConsole* localConsole = ConsoleAccessFactory::getInstance().getConsoleFromSession(session, false);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Console=%s ",
            localConsole->getName().c_str() );

        unsigned long ulPagingConsole = localConsole->getPagingConsoleSourceId();

        delete localConsole;
        localConsole = NULL;

		if (sourceId == ulPagingConsole)
		{
            std::map<unsigned short, std::string> pagingConsoles = CachedConfig::getInstance()->getPagingConsoles();

			std::map<unsigned short, std::string>::iterator findIter = pagingConsoles.find( sourceId );

            FUNCTION_EXIT;
			return ( findIter != pagingConsoles.end() );
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The specified source [%d] does not match with this console's sourceId.",sourceId);
		}
    }
    catch (...)
    {
        // invalid zone - it will be validated as part of the broadcast
    }

    FUNCTION_EXIT;
    return false;
}


void PasHelpers::throwBroadcastIdNotFoundException(const std::string& what, const std::string& broadcastId) 
//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwBroadcastIdNotFoundException" );

    std::ostringstream msg;
    msg << what << " " << broadcastId;

    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound, msg.str().c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
    LOGOUT(msg.str());

    FUNCTION_EXIT;
    throw exception;

    FUNCTION_EXIT;
}


void PasHelpers::throwFepcCommsFailureException(const std::string& what, const std::string& broadcastId) 
//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwFepcCommsFailureException" );

    // Audit failure message for tracking failed requests 
    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure, what.c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.c_str());
    LOGOUT(what);

    FUNCTION_EXIT;
    throw exception;

    FUNCTION_EXIT;
}


void PasHelpers::throwBroadcastLimitReachedException() 
//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwBroadcastLimitReachedException" );

    std::ostringstream msg;
    msg << "Request failed due to broadcast limit (" << CachedConfig::getInstance()->getMaxBroadcasts() << ")";

    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached, msg.str().c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

     FUNCTION_EXIT;
     throw exception;

    FUNCTION_EXIT;
}


void PasHelpers::throwTrainAgentCommsFailureException(const std::string& what, const std::string& broadcastId) 
    //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwTrainAgentCommsFailureException" );

    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(TA_Base_Bus::IPAAgentCorbaDef::eTrainAgentCommsFailure, what.c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.c_str());

    FUNCTION_EXIT;
    throw exception;

    FUNCTION_EXIT;
}


void PasHelpers::throwAccessDeniedException(const std::string& reason, const std::string& broadcastId) 
    //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwAccessDeniedException" );

    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied, reason.c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, reason.c_str());
    LOGOUT(reason);

    FUNCTION_EXIT;
    throw exception;

    FUNCTION_EXIT;
}

void PasHelpers::throwPaErrorException( const int eError, const std::string& what )
//throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
{
    FUNCTION_ENTRY( "throwPaErrorException" );

    TA_Base_Bus::IPAAgentCorbaDef::PaErrorException exception(static_cast<TA_Base_Bus::IPAAgentCorbaDef::EPaErrorId>(eError), what.c_str());
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.c_str());

    FUNCTION_EXIT;
    throw exception;
}



////////////////////////////////////////////////////////
//
//            Audit message senders
//
////////////////////////////////////////////////////////


void PasHelpers::submitPaBroadcastStationDvaRequestAuditMessage( bool hasChime, 
    const TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams* pStationCyclicParams, 
    const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
    const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastStationDvaRequestAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    // HasChime
    NameValuePair chimeParam ( "HasChime", (hasChime ? "True" : "False") );
    parameters.push_back ( &chimeParam );

    static const char* const ParamNone = "N/A";
    // Start time
    NameValuePair startParam ( "Start", ParamNone );
    // Stop time
    NameValuePair stopParam ( "Stop", ParamNone );
    // Interval
    NameValuePair periodParam ( "Interval", ParamNone );
    // Day of week
    NameValuePair dayOfWeek ( "Dayofweek", ParamNone );

    if ( NULL != pStationCyclicParams) // Non scheduled Dva broadcast
    {
        std::stringstream stmDaysOfWeek;
        stmDaysOfWeek << ( pStationCyclicParams->bCycSun ? "Sun, " : "" ) << ( pStationCyclicParams->bCycMon ? "Mon, " : "" )
            << ( pStationCyclicParams->bCycTue ? "Tue, " : "" ) << ( pStationCyclicParams->bCycWed ? "Wed, " : "" )
            << ( pStationCyclicParams->bCycThu ? "Thu, " : "" ) << ( pStationCyclicParams->bCycFri ? "Fri, " : "" )
            << ( pStationCyclicParams->bCycSat ? "Sat, " : "" );

        startParam.value = formatTimeToHourMinute(pStationCyclicParams->startTime);
        stopParam.value = formatTimeToHourMinute(pStationCyclicParams->stopTime);
        periodParam.value = formatTimeToMinute(pStationCyclicParams->period);
        dayOfWeek.value = stmDaysOfWeek.str().c_str();
    }

    parameters.push_back ( &startParam );
    parameters.push_back ( &stopParam );
    parameters.push_back ( &periodParam );
    parameters.push_back ( &dayOfWeek );

    // Zones list
    NameValuePair zoneParam ( "ListOfLocationsAndZones", formatZones(zones) );
    parameters.push_back ( &zoneParam );

    // Message label
    NameValuePair messageLabelsParam ( "MsgLabel", formatDvaMsgNamesForAudit( messages ));
    parameters.push_back ( &messageLabelsParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastStationDvaRequest, parameters, sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastStationLiveRequestAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastStationLiveRequestAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    NameValuePair zoneParam ( "ListOfLocationsAndZones", formatZones(zones) );
    parameters.push_back ( &zoneParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastStationLiveRequest,
                        parameters,
                        sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastStationMusicRequestAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::EMusicType& musicType, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastStationMusicRequestAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    std::string musicTypeStr;
    switch (musicType)
	{
    case TA_Base_Bus::IPAAgentCorbaDef::NoMusic:
        musicTypeStr = "None";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::LocalMusic:
		musicTypeStr = "Local";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::OccMusic:
        musicTypeStr = "Occ";
        break;
	}

    NameValuePair musicSourceParam ( "NoneOrLocalOrOcc", musicTypeStr );
    parameters.push_back ( &musicSourceParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastStationMusicRequest,
                        parameters,
                        sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaRecordAdhocRequestAuditMessage( unsigned short sourceId,
    unsigned long ulMsgKey, unsigned short usMsgType, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaRecordAdhocRequestAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    std::map<unsigned short, std::string> mapPagingConsoles = CachedConfig::getInstance()->getPagingConsoles();

    std::map<unsigned short, std::string>::iterator itFound = mapPagingConsoles.find( sourceId );
    NameValuePair sourceIdParam ( "Source", "Unknown Console" );
    if ( mapPagingConsoles.end() != itFound )
    {
        sourceIdParam.value = itFound->second.c_str();
    }
    parameters.push_back ( &sourceIdParam );

	std::ostringstream messageLabel;
    messageLabel << convertTypeString( usMsgType ) << "_MSG0";
 	try
	{  
		StationDvaMessageRecord record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( ulMsgKey );
		messageLabel << record.usMsgId;
	}
	catch(TA_Base_Core::CachedMappingNotFoundException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

        FUNCTION_EXIT;
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The specified message does not exist in the database" );
    }
    NameValuePair messageLabelsParam ( "MsgLabel", messageLabel.str() );
    parameters.push_back ( &messageLabelsParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaRecordAdhocRequest,
                        parameters,
                        sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaRetryStationBroadcastRequestAuditMessage( const int broadcastType,
                                                                   const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, 
                                                                   const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaRetryStationBroadcastRequestAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    std::string typeStr;
    switch (broadcastType)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::StationMusic: typeStr = "Music"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationLive: typeStr = "Live"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationDva: typeStr = "DVA"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationRecording: typeStr = "Recording"; break;
    }

    NameValuePair broadcastTypeParam ( "BroadcastType", typeStr );
    parameters.push_back ( &broadcastTypeParam );

    NameValuePair zoneParam ( "ListOfLocationsAndZones", formatZones(zones) );
    parameters.push_back ( &zoneParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaRetryStationBroadcastRequest,
                        parameters,
                        sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage( const std::string& broadcastId,
                                                                               unsigned long messageKey, 
                                                                               const std::string& messageVersionNumber,
                                                                               bool hasChime, 
                                                                               bool isSynchronisedWithTis, 
                                                                               unsigned long durationInSecs, 
                                                                               unsigned short periodInSecs, 
                                                                               const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                                                                               const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   NameValuePair versionParam ( "Version", messageVersionNumber );
 //   parameters.push_back ( &versionParam );

 //   std::ostringstream chime;
 //   chime << hasChime;
 //   NameValuePair chimeParam ( "HasChime", chime.str() );
 //   parameters.push_back ( &chimeParam );

 //   std::ostringstream temp;
 //   temp << isSynchronisedWithTis;
 //   NameValuePair syncParam ( "IsTisSynchronised", temp.str() );
 //   parameters.push_back ( &syncParam );

 //   temp.str("");
 //   temp << durationInSecs;
 //   NameValuePair durationParam ( "Duration", temp.str() );
 //   parameters.push_back ( &durationParam );

 //   temp.str("");
 //   temp << periodInSecs;
 //   NameValuePair periodParam ( "Period", temp.str() );
 //   parameters.push_back ( &periodParam );

 //   NameValuePair zoneParam ( "ListOfLocationsAndZones", formatZones(zones) );
 //   parameters.push_back ( &zoneParam );

	//// jeffrey++ TD10365, TD10393
	//std::ostringstream messageLabel;
 //   try
	//{  
	//	StationDvaMessageRecord record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
	//	messageLabel << checkLabel(record.strMsgName);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }
 //   NameValuePair messageLabelsParam ( "MsgLabel", messageLabel.str() );
 //   parameters.push_back ( &messageLabelsParam );
	//// ++jeffrey TD10365

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastEventTriggeredStationDvaRequest,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage( const std::string& broadcastId,
                                                                                         unsigned long messageKey,
                                                                                         const std::string& messageVersionNumber,
                                                                                         bool hasChime,
                                                                                         bool isSynchronisedWithTis,
                                                                                         unsigned long durationInSecs,
                                                                                         unsigned short periodInSecs,
                                                                                         unsigned long stationKey,
                                                                                         const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   NameValuePair versionParam ( "Version", messageVersionNumber );
 //   parameters.push_back ( &versionParam );

 //   std::ostringstream temp;
 //   temp << hasChime;
 //   NameValuePair chimeParam ( "HasChime", temp.str() );
 //   parameters.push_back ( &chimeParam );

 //   temp.str("");
 //   temp << isSynchronisedWithTis;
 //   NameValuePair syncParam ( "IsTisSynchronised", temp.str() );
 //   parameters.push_back ( &syncParam );

 //   temp.str("");
 //   temp << durationInSecs;
 //   NameValuePair durationParam ( "Duration", temp.str() );
 //   parameters.push_back ( &durationParam );

 //   temp.str("");
 //   temp << periodInSecs;
 //   NameValuePair periodParam ( "Period", temp.str() );
 //   parameters.push_back ( &periodParam );

 //   std::string stationName;
 //   try 
 //   {
 //       stationName = CachedMaps::getInstance()->getLocationNameFromKey(stationKey);
 //   }
 //   catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair stationParam ( "Station", stationName );
 //   parameters.push_back ( &stationParam );

	//// jeffrey++ TD10365, TD10393
	//std::ostringstream messageLabel;
 //   try
	//{  
	//	StationDvaMessageRecord record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
	//	messageLabel << checkLabel(record.strMsgName);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }
 //   NameValuePair messageLabelsParam ( "MsgLabel", messageLabel.str() );
 //   parameters.push_back ( &messageLabelsParam );
	//// ++jeffrey TD10365

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastEventTriggeredStationDvaForStationRequest,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage( const std::string& broadcastId,
                                                                                      unsigned long messageKey,
                                                                                      const std::string& messageVersionNumber,
                                                                                      bool hasChime,
                                                                                      bool isSynchronisedWithTis,
                                                                                      unsigned long durationInSecs,
                                                                                      unsigned short periodInSecs,
                                                                                      const std::string& zoneArea,
                                                                                      const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   NameValuePair versionParam ( "Version", messageVersionNumber );
 //   parameters.push_back ( &versionParam );

 //   std::ostringstream temp;
 //   temp << hasChime;
 //   NameValuePair chimeParam ( "HasChime", temp.str() );
 //   parameters.push_back ( &chimeParam );

 //   temp.str("");
 //   temp << isSynchronisedWithTis;
 //   NameValuePair syncParam ( "IsTisSynchronised", temp.str() );
 //   parameters.push_back ( &syncParam );

 //   temp.str("");
 //   temp << durationInSecs;
 //   NameValuePair durationParam ( "Duration", temp.str() );
 //   parameters.push_back ( &durationParam );

 //   temp << periodInSecs;
 //   NameValuePair periodParam ( "Period", temp.str() );
 //   parameters.push_back ( &periodParam );

 //   NameValuePair areaParam ( "Area", zoneArea );
 //   parameters.push_back ( &areaParam );

	//// jeffrey++ TD10365, TD10393
	//std::ostringstream messageLabel;
 //   try
	//{  
	//	StationDvaMessageRecord record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
	//	messageLabel << checkLabel(record.strMsgName);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }
 //   NameValuePair messageLabelsParam ( "MsgLabel", messageLabel.str() );
 //   parameters.push_back ( &messageLabelsParam );
	//// ++jeffrey TD10365

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastEventTriggeredStationDvaForAreaRequest,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage( const std::string& broadcastId,
                                                                                       unsigned long messageKey,
                                                                                       const std::string& messageVersionNumber,
                                                                                       bool hasChime,
                                                                                       bool isSynchronisedWithTis,
                                                                                       unsigned long durationInSecs,
                                                                                       unsigned short periodInSecs,
                                                                                       const std::string& zoneGroupLabel,
                                                                                       const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   NameValuePair versionParam ( "Version", messageVersionNumber );
 //   parameters.push_back ( &versionParam );

 //   std::ostringstream temp;
 //   temp << hasChime;
 //   NameValuePair chimeParam ( "HasChime", temp.str() );
 //   parameters.push_back ( &chimeParam );

 //   temp.str("");
 //   temp << isSynchronisedWithTis;
 //   NameValuePair syncParam ( "IsTisSynchronised", temp.str() );
 //   parameters.push_back ( &syncParam );

 //   temp.str("");
 //   temp << durationInSecs;
 //   NameValuePair durationParam ( "Duration", temp.str() );
 //   parameters.push_back ( &durationParam );

 //   temp << periodInSecs;
 //   NameValuePair periodParam ( "Period", temp.str() );
 //   parameters.push_back ( &periodParam );

 //   NameValuePair groupParam ( "Group", zoneGroupLabel );
 //   parameters.push_back ( &groupParam );

	//// jeffrey++ TD10365, TD10393
	//std::ostringstream messageLabel;
 //   try
	//{  
	//	StationDvaMessageRecord record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
	//	messageLabel << checkLabel(record.strMsgName);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }
 //   NameValuePair messageLabelsParam ( "MsgLabel", messageLabel.str() );
 //   parameters.push_back ( &messageLabelsParam );
	//// ++jeffrey TD10365

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaBroadcastEventTriggeredStationDvaForGroupRequest,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaStationBroadcastResultAuditMessage( const std::string& broadcastId,
                                                             const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& failedZones,
                                                             bool broadcastFailed,
                                                             std::map<int, std::string> failedReasons,//TD10320
                                                             const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaStationBroadcastResultAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::string failedZoneStr;
	//std::string failedReasonStr;//TD10320
 //   if ( failedZones.length() > 0 )
 //   {
 //       failedZoneStr = formatZones(failedZones);
	//	failedReasonStr = formatReasons(failedReasons, failedZones);//TD10320
 //   }
 //   else
 //   {
 //       if (broadcastFailed)
 //       {
 //           failedZoneStr = "All";
 //       }
 //       else
 //       {
 //           failedZoneStr = "None";
 //       }
 //   }
 //   NameValuePair zoneParam ( "ListOfZoneFailuresIfAny", failedZoneStr );
 //   parameters.push_back ( &zoneParam );
	//NameValuePair failedReason ( "ListOfReasonFailuresIfAny", failedReasonStr );//TD10320
 //   parameters.push_back ( &failedReason );//TD10320

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaStationBroadcastResult,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaTrainBroadcastResultAuditMessage( const std::string& broadcastId,
                                                           const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& failedTrains,
                                                           bool broadcastFailed,
                                                           const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaTrainBroadcastResultAuditMessage");

  //  // build the parameters
  //  DescriptionParameters parameters;

  //  NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
  //  parameters.push_back ( &broadcastIdParam );

  //  std::ostringstream failedTrainStr;
  //  if ( failedTrains.length() > 0 )
  //  {
  //      for (int i = 0; i < failedTrains.length(); ++i)
		//{
  //          failedTrainStr << failedTrains[i];

  //          if (i < (failedTrains.length()-1))
  //          {
  //              failedTrainStr << ",";
  //          }
		//}
  //  }
  //  else
  //  {
  //      if (broadcastFailed)
  //      {
  //          failedTrainStr << "All";
  //      }
  //      else
  //      {
  //          failedTrainStr << "None";
  //      }
  //  }

  //  NameValuePair zoneParam ( "ListOfZoneFailuresIfAny", failedTrainStr.str() );
  //  parameters.push_back ( &zoneParam );

  //  // send the message
  //  submitAuditMessage( TA_Base_Core::PAAgentAudit::PaStationBroadcastResult,
  //                      parameters,
  //                      sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaTerminateBroadcastAuditMessage( 
    const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaTerminateBroadcastAuditMessage");

    // build the parameters
    DescriptionParameters parameters;
    NameValuePair messageLabelsParam ( "MsgLabel", formatDvaMsgNamesForAudit( messages ));
    parameters.push_back ( &messageLabelsParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaTerminateBroadcast, parameters, sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaRemoveBroadcastAuditMessage( const std::string& broadcastId,
                                                      const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaRemoveBroadcastAuditMessage");

    //// build the parameters
    //DescriptionParameters parameters;

    //NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
    //parameters.push_back ( &broadcastIdParam );

    //// send the message
    //submitAuditMessage( TA_Base_Core::PAAgentAudit::PaRemoveBroadcast,
    //                    parameters,
    //                    sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaSetAdhocTypeAuditMessage( unsigned short oldType, 
    unsigned short newType, unsigned long messageId, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaSetAdhocTypeAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    std::ostringstream temp;
	temp << convertTypeString(oldType);
    NameValuePair oldTypeParam ( "OldType", temp.str() );
    parameters.push_back ( &oldTypeParam );

    temp.str("");
	temp << convertTypeString(newType);
    NameValuePair newTypeParam ( "NewType", temp.str() );
    parameters.push_back ( &newTypeParam );

    NameValuePair messageLabelsParam ( "MsgLabel", formatDvaMsgNameForAudit( messageId ));
    parameters.push_back ( &messageLabelsParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaSetAdhocType, parameters, sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaSetAdhocLabelAuditMessage( const std::string& oldLabel,
                                                    const std::string& newLabel,
                                                    unsigned long messageKey,
                                                    const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaSetAdhocLabelAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

	//// jeffrey++ TD10393
 //   //NameValuePair oldLabelParam ( "OldLabel", oldLabel );
 //   NameValuePair oldLabelParam ( "OldLabel", checkLabel(oldLabel));
 //   parameters.push_back ( &oldLabelParam );

 //   //NameValuePair newLabelParam ( "NewLabel", newLabel );
 //   NameValuePair newLabelParam ( "NewLabel", checkLabel(newLabel));
 //   parameters.push_back ( &newLabelParam );
	//// ++jeffrey TD10393

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaSetAdhocLabel,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaChangeBroadcastIdAuditMessage( const std::string& oldBroadcastId,
                                                        const std::string& newBroadcastId,
                                                        const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaChangeBroadcastIdAuditMessage");

    // build the parameters
    //DescriptionParameters parameters;

    //NameValuePair oldIdParam ( "OldId", oldBroadcastId );
    //parameters.push_back ( &oldIdParam );

    //NameValuePair newIdParam ( "NewId", newBroadcastId );
    //parameters.push_back ( &newIdParam );

    //// send the message
    //submitAuditMessage( TA_Base_Core::PAAgentAudit::PaChangeBroadcastId,
    //                    parameters,
    //                    sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaAgentBroadcastStateAuditMessage( const std::string& broadcastId, const int broadcastState )
{
    FUNCTION_ENTRY("submitPaAgentBroadcastStateAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::string stateStr = "Unknown State";
 //   switch (broadcastState)
	//{
 //       case TA_Base_Bus::IPAAgentCorbaDef::Creation: stateStr = "Creation"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::Execution: stateStr = "Execution"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::Runtime: stateStr = "Runtime"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::Termination: stateStr = "Termination"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval: stateStr = "PendingRemoval"; break;
	//}

 //   NameValuePair broadcastStateParam ( "BroadcastState", stateStr );
 //   parameters.push_back ( &broadcastStateParam );

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaAgentBroadcastState,
 //                       parameters );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaAgentBroadcastStateFailureAuditMessage( const std::string& broadcastId, const int globalState )
{
    FUNCTION_ENTRY("submitPaAgentBroadcastStateFailureAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   NameValuePair broadcastIdParam ( "BroadcastId", broadcastId );
 //   parameters.push_back ( &broadcastIdParam );

 //   std::string stateStr = "Unknown reason";
 //   switch (globalState)
	//{
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE: return;   // not a failure
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID: stateStr = "No free announce ID"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_MESSAGE_SEQ_ID: stateStr = "No free message sequence ID"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_DVA_CHANNEL: stateStr = "No free DVA channel"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_RECORDING_SOURCE_BUSY: stateStr = "Recording source busy"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_MESSAGE_BUSY_RECORDING: stateStr = "Message busy recording"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_MESSAGE_BUSY_BROADCASTING: stateStr = "Message busy broadcasting"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TRAIN_AGENT: stateStr = "Train Agent comms failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_TIS_AGENT: stateStr = "TIS Agent comms failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_COMMS_FAILURE_AUTHENTICATION_AGENT: stateStr = "Authentication Agent comms failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_TRAIN_AGENT: stateStr = "Train Agent hardware failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_TIS_AGENT: stateStr = "TIS Agent hardware failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT: stateStr = "PA Agent hardware failure"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TRAIN_AGENT: stateStr = "No permission from Train Agent"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_PERMISSION_FAILURE_TIS_AGENT: stateStr = "No permission from TIS Agent"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TRAIN_AGENT: stateStr = "Invalid protocol from Train Agent"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_TIS_AGENT: stateStr = "Invalid protocol from TIS Agent"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT: stateStr = "Invalid protocol from PA Agent"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_PUBLIC_DVA_SET_VERSION_CONFLICT: stateStr = "DVA set A version conflict"; break;
 //       case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_RADIO_CALL_RESET: stateStr = "Audio Call Dropped"; break;
	//}

 //   
 //   NameValuePair broadcastStateFailureParam ( "BroadcastStateFailure", stateStr );
 //   parameters.push_back ( &broadcastStateFailureParam );

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaAgentBroadcastStateFailure,
 //                       parameters );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaAgentDVAMessageUpdateAuditMessage( const unsigned short oldType,
                                                            const unsigned short newType,
                                                            const std::string& oldLabel,
                                                            const std::string& newLabel,
                                                            const unsigned long messageKey )
{
    FUNCTION_ENTRY("submitPaAgentDVAMessageUpdateAuditMessage");
    
 //   // build the parameters
 //   DescriptionParameters parameters;

 //   std::ostringstream messageId;
 //   try
 //   {
 //       messageId << CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messageKey);
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       FUNCTION_EXIT;
 //       throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
 //           "The specified message does not exist in the database" );
 //   }

 //   NameValuePair messageIdParam ( "MessageId", messageId.str() );
 //   parameters.push_back ( &messageIdParam );

 //   std::ostringstream temp;
 //   temp << oldType;
 //   NameValuePair oldTypeParam ( "OldType", temp.str() );
 //   parameters.push_back ( &oldTypeParam );

 //   temp.str("");
 //   temp << newType;
 //   NameValuePair newTypeParam ( "NewType", temp.str() );
 //   parameters.push_back ( &newTypeParam );

	//// jeffrey++ TD10393
 //   //NameValuePair oldLabelParam ( "OldLabel", oldLabel );
 //   NameValuePair oldLabelParam ( "OldLabel", checkLabel(oldLabel));
 //   parameters.push_back ( &oldLabelParam );

 //   //NameValuePair newLabelParam ( "NewLabel", newLabel );
 //   NameValuePair newLabelParam ( "NewLabel", checkLabel(newLabel));
 //   parameters.push_back ( &newLabelParam );


 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaAgentDVAMessageUpdate,
 //                       parameters );

    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPAPrerecordedAuditMessage( const std::string& broadcastId,
                                                       const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                                       unsigned long messageKey, 
                                                       unsigned long libraryVersion, 
                                                       const bool bEmergency,
                                                       const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTrainPAPrerecordedAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    NameValuePair broadcastIdParam ( "ID", formatDvaMsgIdForAudit( messageKey ));
    parameters.push_back ( &broadcastIdParam );

    //NameValuePair trainParam ( "TrainID", formatTrainIdForAudit( trains ));
    NameValuePair trainParam ( "TrainID", "" );
    parameters.push_back ( &trainParam );

    NameValuePair libraryParam ( "Library", formatDvaVersionForAudit( libraryVersion ));
    parameters.push_back ( &libraryParam );

    NameValuePair priorityParam( "Priority", formatDvaPriorityForAudit( bEmergency ));
    parameters.push_back ( &priorityParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::TrainPAPrerecorded,
                            parameters,
                            sessionId );
    }


    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPAPrerecordedAbortAuditMessage( const std::string& broadcastId, const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTrainPrerecordedAbortAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    //NameValuePair trainParam ( "TrainID", formatTrainIdForAudit( trains ));
    NameValuePair trainParam ( "TrainID", "" );
    parameters.push_back ( &trainParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::PaTrainPrerecordAborted,
                            parameters,
                            sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPACyclicAuditMessage( const std::string& broadcastId,
                                                  const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, 
                                                  unsigned long messageKey, 
                                                  unsigned long libraryVersion, 
                                                  const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams,
                                                  const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTrainPACyclicAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    NameValuePair broadcastIdParam ( "ID", broadcastId );
    parameters.push_back ( &broadcastIdParam );

    NameValuePair trainParam ( "Train", "" );
    parameters.push_back ( &trainParam );

    std::ostringstream temp;
    temp << messageKey;
    NameValuePair messageKeyParam ( "MessageKey", temp.str() );
    parameters.push_back ( &messageKeyParam );

    temp.str("");
    temp << libraryVersion;
    NameValuePair libraryParam ( "Library", temp.str() );
    parameters.push_back ( &libraryParam );

    NameValuePair intervalParam ( "Interval", formatInterval(stCyclicParams) );
    parameters.push_back ( &intervalParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::TrainPACyclic,
                            parameters,
                            sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPASetupLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                                 const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                                 const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTrainPASetupLiveAnnouncementAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    //NameValuePair broadcastIdParam ( "ID", broadcastId );
    //parameters.push_back ( &broadcastIdParam );

    //NameValuePair trainParam ( "TrainID", formatTrainIdForAudit(trains) );
    NameValuePair trainParam ( "TrainID", "" );
    parameters.push_back ( &trainParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::TrainPASetupLiveAnnouncement,
                            parameters,
                            sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPAContinueLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                                    const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
																	const std::string& sessionId)
{
    FUNCTION_ENTRY("submitTrainPAContinueLiveAnnouncementAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    NameValuePair broadcastIdParam ( "ID", broadcastId );
    parameters.push_back ( &broadcastIdParam );

    //NameValuePair trainParam ( "Train", formatTrainIdForAudit(trains) );
    NameValuePair trainParam ( "Train", "" );
    parameters.push_back ( &trainParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::TrainPAContinueLiveAnnouncement, parameters, sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitTrainPAEndLiveAnnouncementAuditMessage( const std::string& broadcastId,
                                                               const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
															   const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTrainPAEndLiveAnnouncementAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    NameValuePair broadcastIdParam ( "ID", broadcastId );
    parameters.push_back ( &broadcastIdParam );

    //NameValuePair trainParam ( "Train", formatTrainIdForAudit( trains ));
    NameValuePair trainParam ( "Train", "" );
    parameters.push_back ( &trainParam );

    // for each train
    for( int i = 0; i < trains.length(); i++ )
    {
        // set the train Id Param
        std::ostringstream tempStr;
        tempStr << trains[i];
        trainParam.value = tempStr.str();

        // send the message
        submitAuditMessage( TA_Base_Core::PAAgentAudit::TrainPAEndLiveAnnouncement, parameters, sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitPaZoneBroadcastFailureAuditMessage( unsigned long failedZone,
                                                           const std::string& reason,
                                                           const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaZoneBroadcastFailureAuditMessage");

 //   // build the parameters
 //   DescriptionParameters parameters;

 //   // get the zone Id from the key
 //   std::ostringstream tempZone;
 //   std::ostringstream tempLocation;
 //   try
	//{
 //       tempZone << CachedMaps::getInstance()->getPaZoneIdFromKey(failedZone);
 //       tempLocation << CachedMaps::getInstance()->getLocationNameFromKey(
 //                       CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(failedZone) );
	//}
	//catch(TA_Base_Core::CachedMappingNotFoundException& e)
 //   {
 //       LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

 //       // what if the broadcast failed due to an invalid zone?
 //       // just add the key as the zone Id so the audit message is generated as this
 //       // might be the reason the broadcast failed.
 //       if (tempZone.str().size() < 1)
 //       {
 //           tempZone << failedZone;
 //       }

 //       // this location
 //       try
 //       {
 //           // try to look up this location name
 //           tempLocation << CachedMaps::getInstance()->getLocationNameFromKey(
 //                           CachedConfig::getInstance()->getAgentLocationKey() );
 //       }
 //       catch (...)
 //       {
 //           // if it fails just use the key
 //           tempLocation << CachedConfig::getInstance()->getAgentLocationKey();
 //       }
 //   }

 //   NameValuePair zoneIdParam ( "ZoneId", tempZone.str() );
 //   parameters.push_back ( &zoneIdParam );

 //   NameValuePair locationParam ( "Location", tempLocation.str() );
 //   parameters.push_back ( &locationParam );

 //   NameValuePair reasonParam ( "Reason", reason );
 //   parameters.push_back ( &reasonParam );

 //   // send the message
 //   submitAuditMessage( TA_Base_Core::PAAgentAudit::PaZoneBroadcastFailure,
 //                       parameters,
 //                       sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaTrainBroadcastFailureAuditMessage( unsigned long trainId,
                                                            unsigned long location,
                                                            const std::string& reason,
                                                            const std::string& sessionId )
{
    FUNCTION_ENTRY("submitPaTrainBroadcastFailureAuditMessage");
    
    // build the parameters
    DescriptionParameters parameters;

    std::ostringstream temp;
    temp << trainId;
    NameValuePair trainParam ( "TrainId", temp.str() );
    parameters.push_back ( &trainParam );

    temp.str("");
    temp << location;
    NameValuePair locationParam ( "Location", temp.str() );
    parameters.push_back ( &locationParam );

    NameValuePair reasonParam ( "Reason", reason );
    parameters.push_back ( &reasonParam );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaTrainBroadcastFailure,
                        parameters,
                        sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPaTrainBroadcastFailureAuditMessages( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                                             unsigned long location,
                                                             const std::string& reason,
                                                             const std::string& sessionId )
{
    FUNCTION_ENTRY( "submitPaTrainBroadcastFailureAuditMessages" );

    for (int i = 0; i < trains.length(); i++)
    {
        submitPaTrainBroadcastFailureAuditMessage(
            trains[i],
            location,
            reason,
            sessionId );
    }

    FUNCTION_EXIT;
}


void PasHelpers::submitTerminateFireCountdownAuditMessage( const unsigned int usStationId, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitTerminateFireCountdownAuditMessage");

    // build the parameters
    DescriptionParameters parameters;

    unsigned long ulLocationkey = 0u;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        ulLocationkey = CachedMaps::getInstance()->getLocationKeyFromPasStationId( usStationId );
    }
    else
    {
        ulLocationkey = CachedConfig::getInstance()->getAgentLocationKey();
    }

    std::string locationName = CachedMaps::getInstance()->getLocationNameFromKey( ulLocationkey );
    NameValuePair stationName ( "Station", locationName );
    parameters.push_back ( &stationName );

    // send the message
    submitAuditMessage( TA_Base_Core::PAAgentAudit::PaTerminateFireCountdown, parameters, sessionId );

    FUNCTION_EXIT;
}


void PasHelpers::submitPAAgentOnlineUpdateAuditMessage( const std::string& parameter )
{
    FUNCTION_ENTRY("submitPAAgentOnlineUpdateAuditMessage");

    //// build the parameters
    //DescriptionParameters parameters;

    //NameValuePair parameterParam ( "Parameter", parameter );
    //parameters.push_back ( &parameterParam );

    //// send the message
    //submitAuditMessage( TA_Base_Core::PAAgentAudit::PAAgentOnlineUpdate,
    //                    parameters );

    FUNCTION_EXIT;
}

// TD 15128
void PasHelpers::submitPaPrioritySchemeUpdateAuditMessage()
{
 	FUNCTION_ENTRY("submitPaPrioritySchemeUpdateAuditMessage");

 //   if (CachedConfig::getInstance()->getAgentKey() == 0)
 //   {
 //       // The agent has not been initialised completely, we can only log this error 
 //       // below

 //       FUNCTION_EXIT;
 //       throw;
 //   }
 //       
 //   // construct the DescriptionParameters list
	//DescriptionParameters parameters;
 //       
 //   std::string locationName = CachedMaps::getInstance()->getLocationNameFromKey( 
 //       CachedConfig::getInstance()->getAgentLocationKey() );

 //   NameValuePair pair1 ( "LocationName", locationName );
	//parameters.push_back ( &pair1 );

	//// send the message
	//submitAuditMessage( TA_Base_Core::PAAgentAudit::PaPrioritySchemeUpdate,
	//	                parameters );

    FUNCTION_EXIT;
}

// TD 15128




// ExceptionChecked
// Threadsafe between:
// - external corba request threads
// - class child threads
void PasHelpers::submitAuditMessage( const MessageType& messageType,
    DescriptionParameters& parameters, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitAuditMessage");
	if (NULL == m_auditSender)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_auditSender is NULL" );
		return;
	}

    bool submitted = false;
    std::string reason = "";

    // Add a empty parameter to the first position to let event value empty
    NameValuePair pirNullParam( "EmptyValueString", " " );
    parameters.insert( parameters.begin(), &pirNullParam );
    
    // If the agent has not been initialised, we can only log this message
    if (CachedConfig::getInstance()->getAgentKey() != 0)
    {
        try
        {            
            m_auditSender->sendAuditMessageWithAsset(
                messageType,                                 // Message Type
                //CachedConfig::getInstance()->getAgentKey(),  // Entity key
				//TD17452
				CachedConfig::getInstance()->getMonitorProcessKey(),
                parameters,                       // Description
                "",                                          // Additional details
                sessionId,                                   // SessionID if applicable
                "",                                          // AlarmID if an alarm associated with event
                "",                                          // IncidentKey if incident associated with event
                "",                                          // EventID of a parent event, used to link events
			    CachedConfig::getInstance()->getAgentAssetName());  //Asset Name

            submitted = true;

        } // try block
        catch( TA_Base_Core::TransactiveException& ex )
        {
            reason = ex.what();
        }
        catch( ... )
        {
            reason = "Unknown reason.";
        }
    }
    else
    {
        // log the fact that the message cant be sent
        // also log the details of the message
        reason = "Agent not initialised.";
    }
    
    // failed to submit - make sure all the details are logged
    if (!submitted)
    {
        std::ostringstream msg;
        msg << "Failed to submit audit message " << messageType.getTypeKey() << ". " << reason << std::endl
            << "Parameters are: ";

        for (DescriptionParameters::const_iterator iter = parameters.begin();
        iter != parameters.end(); iter++ )
        {
            msg << (*iter)->name << "=" << (*iter)->value << ", ";
        }
        msg << "SessionId=" << sessionId;

        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, msg.str().c_str());
    }

    FUNCTION_EXIT;
}


std::string PasHelpers::formatTime(time_t time)
{
    FUNCTION_ENTRY( "formatTime" );

    // Many places set time to 0 where the time is unused or unavailable.
    // However, this function will blindly return "01-Jan-1970 08:00:00" in 
    // these cases. In order to avoid misunderstanding we don't print the 
    // timestamp if the time is 0
    std::ostringstream timeStr;
    if ( time == 0 )
    {
        timeStr << "N/A";

        FUNCTION_EXIT;
        return timeStr.str();
    }

    struct tm timeStruct;
    ACE_OS::localtime_r(&time, &timeStruct);

    // DD-Mon-YYYY HH:MM:SS
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_mday << "-"; // DD-

    // Mon-
    switch (timeStruct.tm_mon )
    {
        case 0: timeStr << "Jan-"; break;
        case 1: timeStr << "Feb-"; break;
        case 2: timeStr << "Mar-"; break;
        case 3: timeStr << "Apr-"; break;
        case 4: timeStr << "May-"; break;
        case 5: timeStr << "Jun-"; break;
        case 6: timeStr << "Jul-"; break;
        case 7: timeStr << "Aug-"; break;
        case 8: timeStr << "Sep-"; break;
        case 9: timeStr << "Oct-"; break;
        case 10: timeStr << "Nov-"; break;
        case 11: timeStr << "Dec-"; break;
    }

    timeStr << ( timeStruct.tm_year + 1900 ) << " "; // YYYY

    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_hour << ":"; // HH:
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_min << ":"; // MM:
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_sec; // SS
    FUNCTION_EXIT;
    return timeStr.str();
}

std::string PasHelpers::formatTimeToHourMinute(time_t time)
{
    FUNCTION_ENTRY( "formatTimeToHourMinute" );

    std::ostringstream timeStr;
    if ( time == 0 )
    {
        timeStr << "N/A";

        FUNCTION_EXIT;
        return timeStr.str();
    }

    struct tm timeStruct;
    ACE_OS::localtime_r(&time, &timeStruct);

    // HH:MM
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_hour << ":"; // HH:
    timeStr << std::setw(2) << std::setfill('0') << timeStruct.tm_min; // MM:

    FUNCTION_EXIT;
    return timeStr.str();
}

std::string PasHelpers::formatTimeToMinute( time_t time )
{
    FUNCTION_ENTRY( "formatTimeToHourMinute" );

    std::ostringstream timeStr;
    if ( time == 0 )
    {
        timeStr << "N/A";

        FUNCTION_EXIT;
        return timeStr.str();
    }

    struct tm timeStruct;
    ACE_OS::localtime_r(&time, &timeStruct);

    timeStr << timeStruct.tm_min;

    FUNCTION_EXIT;
    return timeStr.str();
}

std::string PasHelpers::formatZones(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)
{
    FUNCTION_ENTRY( "formatZones" );

    // build a map of location keys to lists of zones for those location keys
    // change zoneId from unsigned long to string to map with actual zoneId
    // which can be obtained in the stn/occ equipment after "_" e.g. BBS.PA.PUBLIC.PZON_02A => 02A
    // unsigned long zoneId = CachedMaps::getInstance()->getPaZoneIdFromKey(zones[i]);
    typedef std::set<std::string> ZoneList;
    typedef std::map<unsigned long, ZoneList> LocationKeyToZoneListMap;

    LocationKeyToZoneListMap locationKeyToZoneListMap;

    std::string strOutput = "";
    const std::string strZonePre = "PAZONE_";
    const std::string strZoneEnd = "0123456789";

    try
	{
        // build the map
		for (int i = 0; i < zones.length(); ++i)
		{
            TA_IRS_Bus::PaZoneRecord record = CachedMaps::getInstance()->getPaZoneRecordFromKey(zones[i]);

            std::string equipmentName = record.m_occEquipmentEntityName.empty() ? record.m_stnEquipmentEntityName : record.m_occEquipmentEntityName;
            std::vector<std::string> tokens;

            std::string::size_type unPre = equipmentName.find( strZonePre );

            if ( std::string::npos == unPre )
            {
                continue;
            }

            unPre += strZonePre.length();

            std::string::size_type unPost = equipmentName.find_first_not_of( strZoneEnd, unPre );
            if ( std::string::npos == unPost )
            {
                continue;
            }

            std::string zoneId = equipmentName.substr( unPre, unPost - unPre );

           	unsigned long locationKey = CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zones[i]);
            locationKeyToZoneListMap[locationKey].insert(zoneId);
            tokens.clear();
		}

        // then print the list out
        // DBGZ1, DBGZ1, DBGZ1 ...
        const std::string strDelimiter = ", ";
        std::ostringstream destinationList;

        for (LocationKeyToZoneListMap::iterator locIter = locationKeyToZoneListMap.begin();
        locIter != locationKeyToZoneListMap.end(); ++locIter)
        {
            for (ZoneList::iterator zoneIter = locIter->second.begin(); zoneIter != locIter->second.end(); ++zoneIter)
            {
                destinationList << CachedMaps::getInstance()->getLocationNameFromKey(locIter->first);
                destinationList << "Z";
                destinationList << (*zoneIter);

                destinationList << strDelimiter;
            }
        }

        strOutput = destinationList.str().c_str();
        if ( strOutput.length() > strDelimiter.length() )
        {
            strOutput = strOutput.substr( 0, strOutput.length() - strDelimiter.length() );
        }
	}
	catch(TA_Base_Core::CachedMappingNotFoundException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

        FUNCTION_EXIT;
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The specified zone does not exist in the database" );
    }

    FUNCTION_EXIT;
    return strOutput;
}


std::string PasHelpers::formatZone(int zoneID)
{
    FUNCTION_ENTRY( "formatZone" );

	std::string zoneStr("");

    try
	{
        // build the map
		TA_IRS_Bus::PaZoneRecord record = CachedMaps::getInstance()->getPaZoneRecordFromKey(zoneID);

		std::string equipmentName = record.m_occEquipmentEntityName.empty() ? record.m_stnEquipmentEntityName : record.m_occEquipmentEntityName;
		std::vector<std::string> tokens;

		PasHelpers::getInstance()->tokenise(equipmentName, tokens, "PAZONE_");
		std::stringstream zoneNum;
		if (tokens.size()!=2)
		{
			zoneNum<<zoneID;
			zoneStr = zoneNum.str();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No match for PAZONE_ , the equipmentName = %s, zoneID = %ld ",equipmentName.c_str(),zoneID);
		}
		else
		{
			zoneStr = tokens[1];
		}

		tokens.clear();
	}
	catch(TA_Base_Core::CachedMappingNotFoundException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

        FUNCTION_EXIT;
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The specified zone does not exist in the database" );
    }

    FUNCTION_EXIT;
    return zoneStr;
}


std::string PasHelpers::formatReasons(const std::map<int, std::string>& reasons, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)//TD10320
{
    FUNCTION_ENTRY( "formatReasons" );

    std::ostringstream reasonList;
    int i;
    std::map<int, std::string>::const_iterator resIter;

    for ( resIter = reasons.begin(), i = 0;
          resIter != reasons.end(); ++i/*resIter incremented below */)
    {
        // location name

		std::string zoneStr = formatZone(zones[i]);

        reasonList << zoneStr << "(";
		reasonList << resIter->second ;
            	
        // avoid adding semicolon for the last item
        if ( ++resIter != reasons.end() )
        {
            reasonList << ");";
        }
        else
        {
            reasonList << ")";
        }		
    }

    FUNCTION_EXIT;
    return reasonList.str();
}


std::string PasHelpers::formatInterval( const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams )
{
    FUNCTION_ENTRY( "formatInterval" );

    std::ostringstream formatStr;
    formatStr << std::setw(2) << std::setfill('0') << stCyclicParams.periodInSecs << ":"
              << std::setw(2) << std::setfill('0') << stCyclicParams.durationInSecs;
    FUNCTION_EXIT;
    return formatStr.str();
}


////////////////////////////////////////////////////////
//
//            Comms message senders
//
////////////////////////////////////////////////////////


void PasHelpers::sendCurrentBroadcastsUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData)
{
    FUNCTION_ENTRY( "sendCurrentBroadcastsUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending CurrentBroadcastsUpdate message to subscribers");

    CORBA::Any data;

    data <<= updateData;

    m_paAgentCommsSender->sendCommsMessage(
        PAAgentComms::CurrentBroadcastsUpdate,                  // Context::Type
        CachedConfig::getInstance()->getAgentKey(),             // EntityKey
        data,                                                   // Data
        CachedConfig::getInstance()->getAgentSubsystemKey(),    // Subsystem
		CachedConfig::getInstance()->getAgentLocationKey(),     // LocationKey
        NULL);

    FUNCTION_EXIT;
}


void PasHelpers::sendDvaMessagesUpdate(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData)
{
    FUNCTION_ENTRY( "sendDvaMessagesUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending DvaMessagesUpdate message to subscribers");
    
    CORBA::Any data;

    try
    {
        data <<= updateData;

        m_paAgentCommsSender->sendCommsMessage(
            PAAgentComms::DvaMessagesUpdate,                        // Context::Type
            CachedConfig::getInstance()->getAgentKey(),             // EntityKey
            data,                                                   // Data
            CachedConfig::getInstance()->getAgentSubsystemKey(),    // Subsystem
            CachedConfig::getInstance()->getAgentLocationKey(),     // LocationKey
            NULL);
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception when calling CommsMessageSender::sendCommsMessage" );
    }

    FUNCTION_EXIT;
}

void PasHelpers::sendDvaMsgVersionUpdate( const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData )
{
    FUNCTION_ENTRY( "sendDvaMsgVersionUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending sendDvaMsgVersionUpdate message to subscribers");

    CORBA::Any data;

    try
    {
        data <<= updateData;
        m_paAgentCommsSender->sendBroadcastCommsMessage( TA_Base_Core::PAAgentBroadcastComms::DvaVersionsUpdate,CachedConfig::getInstance()->getAgentKey(), data, NULL );

        //if ( !CachedConfig::getInstance()->getIsMaster() )
        //{
        //    m_paAgentCommsSender->sendCommsMessage(
        //        PAAgentComms::DvaVersionsUpdate,                        // Context::Type
        //        CachedConfig::getInstance()->getAgentKey(),             // EntityKey
        //        data,                                                   // Data
        //        CachedConfig::getInstance()->getAgentSubsystemKey(),    // Subsystem
        //        CachedConfig::getInstance()->getOccLocationKey(),
        //        NULL);
        //}
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception when calling CommsMessageSender::sendCommsMessage" );
    }

    FUNCTION_EXIT;
}


////////////////////////////////////////////////////////
//
//            State update message senders
//
////////////////////////////////////////////////////////


void PasHelpers::sendPAAgentBroadcastParameterUpdate( const int parameter, const CORBA::Any& parameterData, const std::string& broadcastId )
{
    FUNCTION_ENTRY( "sendPAAgentBroadcastParameterUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
        "Sending PAAgentBroadcastParameterUpdate message to subscribers");

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter disposableUpdateData = 
        { broadcastId.c_str(), static_cast<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastParameter>(parameter), parameterData };

    CORBA::Any packetData;

    packetData <<= disposableUpdateData;

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentBroadcastParameterUpdate,
		0, CachedConfig::getInstance()->getAgentName(), packetData );

    FUNCTION_EXIT;
}


void PasHelpers::sendPAAgentRemovedBroadcastUpdate(const std::string& updateData)
{
    FUNCTION_ENTRY( "sendPAAgentRemovedBroadcastUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
        "Sending PAAgentRemovedBroadcastUpdate message to subscribers");

    CORBA::Any data;

    data <<= CORBA::Any::from_string(updateData.c_str(),0);

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentRemovedBroadcastUpdate,
		0, CachedConfig::getInstance()->getAgentName(), data );

    FUNCTION_EXIT;
}


void PasHelpers::sendPAAgentNewBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& updateData)
{
    FUNCTION_ENTRY( "sendPAAgentNewBroadcastUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending PAAgentNewBroadcastUpdate message to subscribers");

    CORBA::Any data;

    data <<= updateData;

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentNewBroadcastUpdate,
		0, CachedConfig::getInstance()->getAgentName(), data );

    FUNCTION_EXIT;
}


void PasHelpers::sendPAAgentRenamedBroadcastUpdate(const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData)
{
    FUNCTION_ENTRY( "sendPAAgentRenamedBroadcastUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending PAAgentRenamedBroadcastUpdate message to subscribers");

    CORBA::Any data;

    data <<= updateData;

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentRenamedBroadcastUpdate,
		0, CachedConfig::getInstance()->getAgentName(), data );

    FUNCTION_EXIT;
}


void PasHelpers::sendPAAgentPaSysParamUpdate( const TA_Base_Bus::IPAAgentCorbaDef::PaSysParameter& updateData )
{
    FUNCTION_ENTRY( "sendPAAgentPaSysParamUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending sendPAAgentPaSysParamUpdate message to subscribers");

    CORBA::Any data;

    data <<= updateData;

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentPasParamUpdate,
        0, CachedConfig::getInstance()->getAgentName(), data );

    FUNCTION_EXIT;
}


void PasHelpers::sendPAAgentPasScheduleDvaIdUpdate( const std::set<unsigned short>& setIds )
{
    FUNCTION_ENTRY( "sendPAAgentPasScheduleDvaIdUpdate" );

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Sending sendPAAgentPasScheduleDvaIdUpdate message to subscribers");

    TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds seqScheduleIds;
    seqScheduleIds.length( setIds.size() );

    int nIndex = 0;
    for ( std::set<unsigned short>::const_iterator itLoop = setIds.begin(); setIds.end() != itLoop; ++itLoop )
    {
        seqScheduleIds[nIndex++] = *itLoop;
    }

    CORBA::Any data;

    data <<= seqScheduleIds;

    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentAvailableScheduleDvaIdUpdate,
        0, CachedConfig::getInstance()->getAgentName(), data );

    FUNCTION_EXIT;
}


void PasHelpers::tokenise(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    FUNCTION_ENTRY( "tokenise" );

	std::string::size_type firstPos = str.find(delimiters, 0);
    if (std::string::npos != firstPos && firstPos > 0 )
    {
		firstPos = 0;
    }
    // Find first "non-delimiter".
    std::string::size_type secondpos = str.find(delimiters, firstPos);
	
    while ( std::string::npos != secondpos)
    {
		tokens.push_back(str.substr(firstPos, secondpos - firstPos));
		
        // Found a token, add it to the vector.
		
        // Skip delimiters.  Note the "not_of"
		firstPos = secondpos + delimiters.size();
        secondpos = str.find(delimiters, firstPos);
		if (std::string::npos != secondpos)
		{
			tokens.push_back(str.substr(firstPos,secondpos - firstPos));
			firstPos = secondpos + delimiters.size();
			secondpos = str.find(delimiters, firstPos);
			if (secondpos == std::string::npos)
			{
				tokens.push_back(str.substr(firstPos,str.size() - firstPos));
			}
		}
		else
		{
			tokens.push_back(str.substr(firstPos,str.size() - firstPos));
		}
		
    }

    FUNCTION_EXIT;
}


////////////////////////////////////////////////////////
//
//            Alarm message senders
//
////////////////////////////////////////////////////////


// ExceptionChecked
void PasHelpers::raiseInvalidPAConfigurationAlarm( const std::string& detail )
{
 	FUNCTION_ENTRY("raiseInvalidPAConfigurationAlarm");

    try
    {   
        if (CachedConfig::getInstance()->getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below

            FUNCTION_EXIT;
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Invalid agent key"));
        }
        
        // construct the DescriptionParameters list
	    DescriptionParameters parameters;
        
        std::string locationName = CachedMaps::getInstance()->getLocationNameFromKey( 
            CachedConfig::getInstance()->getAgentLocationKey() );

        NameValuePair pair1 ( "LocationName", locationName );
	    parameters.push_back ( &pair1 );
	    NameValuePair pair2 ( "Detail", detail );
	    parameters.push_back ( &pair2 );

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset ( 
            PAAgentAlarms::InvalidPAConfiguration, 
            CachedConfig::getInstance()->getAgentKey(),	        	// entity key
            CachedConfig::getInstance()->getAgentTypeKey(),	    	// entity type key
            parameters,                                  // parameters
		    CachedConfig::getInstance()->getAgentName(),			// entity name
            CachedConfig::getInstance()->getAgentLocationKey(),		// region key
            CachedConfig::getInstance()->getAgentSubsystemKey(),	// subystem key
			CachedConfig::getInstance()->getAgentAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );
    }
    catch(...)
    {
        std::ostringstream msg;
        msg << "Failed to raise the InvalidPAConfiguration alarm.  Detail='"
            << detail << "'";
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, msg.str().c_str());
    }
    FUNCTION_EXIT;
}

// ExceptionChecked
void PasHelpers::closeInvalidPAConfigurationAlarm()
{
 	FUNCTION_ENTRY("closeInvalidPAConfigurationAlarm");

    try
    {   
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            PAAgentAlarms::InvalidPAConfiguration, 
            CachedConfig::getInstance()->getAgentKey(),
            CachedConfig::getInstance()->getAgentLocationKey()
            );
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Failed to close the InvalidPAConfiguration alarm");
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void PasHelpers::raisePASCommsFailureAlarm()
{
 	FUNCTION_ENTRY("raisePASCommsFailureAlarm");

    // Thread safe 

	if(!m_pasAlarm.empty())
	{
		//alarm is already raised
        FUNCTION_EXIT;
		return;
	}

    try
    {   
        if (CachedConfig::getInstance()->getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below

            FUNCTION_EXIT;
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Invalid agent key"));
        }
        
        // construct the DescriptionParameters list
	    DescriptionParameters parameters;
        
        std::string locationName = CachedMaps::getInstance()->getLocationNameFromKey( 
            CachedConfig::getInstance()->getAgentLocationKey() );

        NameValuePair pair1 ( "LocationName", locationName );
	    parameters.push_back ( &pair1 );

        std::string alarmId = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
            PAAgentAlarms::PASCommsFailure, 
            CachedConfig::getInstance()->getAgentKey(),	        	// entity key
            CachedConfig::getInstance()->getAgentTypeKey(),	    	// entity type key
            parameters,                                             // parameters
		    CachedConfig::getInstance()->getAgentName(),			// entity name
            CachedConfig::getInstance()->getAgentLocationKey(),		// region key
            CachedConfig::getInstance()->getAgentSubsystemKey(),	// subystem key
			CachedConfig::getInstance()->getAgentAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );

        m_pasAlarm = alarmId;

    }
    catch(...)
    {
        std::ostringstream msg;
        msg << "Failed to raise the PASCommsFailure alarm";
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, msg.str().c_str());
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void PasHelpers::closePASCommsFailureAlarm()
{
 	FUNCTION_ENTRY("closePASCommsFailureAlarm");

	if(m_pasAlarm.empty())
	{
		//no alarm has been raised, so there is no alarm to close
        FUNCTION_EXIT;
		return;
	}

    try
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(m_pasAlarm, 
												CachedConfig::getInstance()->getAgentLocationKey());
		m_pasAlarm = "";
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Failed to close the PASCommsFailure alarm");
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void PasHelpers::raisePASVersionConflictAlarm( const std::string& stationInConflict )
{
 	FUNCTION_ENTRY("raisePASVersionConflictAlarm");

    try
    {   
        if (CachedConfig::getInstance()->getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below

            FUNCTION_EXIT;
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Invalid agent key"));
        }
        
        // construct the DescriptionParameters list
	    DescriptionParameters parameters;
        
        std::string locationName = CachedMaps::getInstance()->getLocationNameFromKey( 
            CachedConfig::getInstance()->getAgentLocationKey() );

        NameValuePair pair1 ( "LocationName", locationName );
	    parameters.push_back ( &pair1 );
        
        NameValuePair pair2 ( "TargetLocationName", stationInConflict );
	    parameters.push_back ( &pair2 );


        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset ( 
            PAAgentAlarms::PASVersionConflict, 
            CachedConfig::getInstance()->getAgentKey(),	        	// entity key
            CachedConfig::getInstance()->getAgentTypeKey(),	    	// entity type key
            parameters,                                  // parameters
		    CachedConfig::getInstance()->getAgentName(),			// entity name
            CachedConfig::getInstance()->getAgentLocationKey(),		// region key
            CachedConfig::getInstance()->getAgentSubsystemKey(),	// subystem key
			CachedConfig::getInstance()->getAgentAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );
    }
    catch(...)
    {
        std::ostringstream msg;
        msg << "Failed to raise the PASVersionConflict alarm";
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, msg.str().c_str());
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void PasHelpers::closePASVersionConflictAlarm()
{
 	FUNCTION_ENTRY("closePASVersionConflictAlarm");

    try
    {   
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            PAAgentAlarms::PASVersionConflict, 
            CachedConfig::getInstance()->getAgentKey(),
            CachedConfig::getInstance()->getAgentLocationKey()
            );
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Failed to close the PASVersionConflict alarm");
    }
    
    FUNCTION_EXIT;
}


void PasHelpers::raiseTrainLibraryVersionAlarm( int trainID,
                                                int libraryVersion,
                                                int iscsLibraryVersion )
{
    FUNCTION_ENTRY("raiseTrainLibraryVersionAlarm");

    // no need to lock the map - only called by the corba message thread

    TrainIdToAlarmDetailsMap::iterator findIter = m_trainToAlarmIdMap.find(trainID);
    if (findIter != m_trainToAlarmIdMap.end())
    {
        if (findIter->second.trainLibraryVersion == libraryVersion)
        {
            // this alarm already raised
            FUNCTION_EXIT;
            return;
        }
        else
        {
            // close and re-raise the alarm
            closeTrainLibraryVersionAlarm(trainID);
        }
    }

    // Threadsafe 

    try
    {   
        if (CachedConfig::getInstance()->getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below

            FUNCTION_EXIT;
            return;
        }
        
        // construct the DescriptionParameters list
	    DescriptionParameters parameters;
        
        std::stringstream trainIdStr;
        trainIdStr << trainID;
        std::stringstream libraryStr;
        libraryStr << libraryVersion;
        std::stringstream iscsLibraryStr;
        iscsLibraryStr << iscsLibraryVersion;

        NameValuePair pair1 ( "TrainId", trainIdStr.str() );
	    parameters.push_back ( &pair1 );
        NameValuePair pair2 ( "TrainVersion", libraryStr.str() );
	    parameters.push_back ( &pair2 );
        NameValuePair pair3 ( "IscsVersion", iscsLibraryStr.str() );
	    parameters.push_back ( &pair3 );

        TrainAlarm newAlarm;
        newAlarm.trainLibraryVersion = libraryVersion;
        newAlarm.alarmId = "";
        /* TODO uncomment this as part of TD 15292 when the alarm type is added
        newAlarm.alarmId = 
            TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarm (
                PAAgentAlarms::TrainPALibraryInconsistentWithISCS, 
                CachedConfig::getInstance()->getAgentKey(),	        	// entity key
                CachedConfig::getInstance()->getAgentTypeKey(),	    	// entity type key
                parameters,                                  // parameters
		        CachedConfig::getInstance()->getAgentName(),			// entity name
                CachedConfig::getInstance()->getAgentLocationKey(),		// region key
                CachedConfig::getInstance()->getAgentSubsystemKey() );	// subystem key
        */

        m_trainToAlarmIdMap[trainID] = newAlarm;
    }
    catch(...)
    {
        std::ostringstream msg;
        msg << "Failed to raise the TrainPALibraryInconsistentWithISCS alarm";
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, msg.str().c_str());
    }

    FUNCTION_EXIT;
}


void PasHelpers::closeTrainLibraryVersionAlarms()
{
    FUNCTION_ENTRY("closeTrainLibraryVersionAlarms");
    try
    {
        /* TODO uncomment this as part of TD 15292 when the alarm type is added
        m_alarmHelpercloseAlarmAtLocation(
            PAAgentAlarms::TrainPALibraryInconsistentWithISCS, 
            CachedConfig::getInstance()->getAgentKey(),
            CachedConfig::getInstance()->getAgentLocationKey()
            );
        */

        m_trainToAlarmIdMap.clear();
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Failed to close the TrainPALibraryInconsistentWithISCS alarm");

    }


    FUNCTION_EXIT;
}

void PasHelpers::closeTrainLibraryVersionAlarm(int trainID)
{
    FUNCTION_ENTRY("closeTrainLibraryVersionAlarm");

    TrainIdToAlarmDetailsMap::iterator findIter = m_trainToAlarmIdMap.find(trainID);
    if (findIter == m_trainToAlarmIdMap.end())
    {
        FUNCTION_EXIT;
        return;        // not active
    }

    try
    {
		/* TODO uncomment this as part of TD 15292 when the alarm type is added
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(findIter->second.alarmId, 
		CachedConfig::getInstance()->getAgentLocationKey());
        */
        m_trainToAlarmIdMap.erase(findIter);
    }
    catch(...)
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
            "Failed to close the TrainPALibraryInconsistentWithISCS alarm");
    }
    

    FUNCTION_EXIT;
}


void PasHelpers::closeTrainLibraryVersionAlarmOnVersionChange(int newIscsVersion)
{
    FUNCTION_ENTRY("closeTrainLibraryVersionAlarmOnVersionChange");

    TrainIdToAlarmDetailsMap::iterator iter = m_trainToAlarmIdMap.begin();
    while (iter != m_trainToAlarmIdMap.end())
    {
        // close the alarm if the train now matches
        if (iter->second.trainLibraryVersion == newIscsVersion)
        {
            try
            {
//				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(iter->second.alarmId, 
//												CachedConfig::getInstance()->getAgentLocationKey());
            }
            catch(...)
            {
                LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, 
                    "Failed to close the TrainPALibraryInconsistentWithISCS alarm");
            }

            m_trainToAlarmIdMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }

    FUNCTION_EXIT;
}


void PasHelpers::sendBroadcastProgressUpdate( AbstractBroadcast& broadcast, bool bNotifyClient )
{
    FUNCTION_ENTRY( "sendBroadcastProgressUpdate" );

    CORBA::Any data;
    NameValuePair nvp( TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_ID_FILTER_STRING, broadcast.getBroadcastId() );

    std::vector<NameValuePair*> extraFilterableData;
    extraFilterableData.push_back(&nvp);

	TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* progressReport = broadcast.getProgressReportCopy();

	if (NULL == progressReport)
	{
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "getProgressReportCopy() is NULL");
		return;
	}

	LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Send broadcast progress update:%s", getProgressLogStr(broadcast).c_str());

    data <<= broadcast.getProgressReportCopy();

    if ( bNotifyClient )
    {
        // Send Comms message for PA Managers
        m_paAgentCommsSender->sendCommsMessage(
            TA_Base_Core::PAAgentComms::BroadcastProgressUpdate,                  // Context::Type
            m_cachedConfigInstance->getAgentKey(),                  // EntityKey
            data,                                                   // Data
            m_cachedConfigInstance->getAgentSubsystemKey(),         // Subsystem
		    m_cachedConfigInstance->getAgentLocationKey(),          // LocationKey
            &extraFilterableData);                                  // BroadcastId
    }

    // Send State update message for PA Agents
    m_stateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::PAAgentStateUpdate::PAAgentBroadcastProgressUpdate,
        0, m_cachedConfigInstance->getAgentName(), data );

    FUNCTION_EXIT;
}


std::string PasHelpers::convertTypeString( unsigned short InType )
{
    FUNCTION_ENTRY( "convertTypeString" );

	std::string OutType;

    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED == InType || 
         TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == InType )
    {
        OutType = "DELETED";
    }
    else if ((TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY == InType) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == InType ))
    {
        OutType = "EMERGENCY";
    }
    else if ((TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_NORMAL == InType) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == InType ))
    {
        OutType = "NORMAL";
    }
    else if ((TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_SPARE_A == InType) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_SPARE_B == InType ) || 
        ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_SPARE_C == InType ))
    {
        OutType = "SPARE";
    }
    else if (TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_ATS_MSG == InType)
    {
        OutType = "ATS";
    }
    else
    {
        OutType = "NONE";
    }

    FUNCTION_EXIT;
	return OutType;
}

std::string PasHelpers::checkLabel( const std::string& InLabel)
{
    FUNCTION_ENTRY( "checkLabel" );

    std::string OutLabel = InLabel.empty() ? "<empty>" : InLabel.c_str();

    FUNCTION_EXIT;
	return OutLabel.c_str();
}

std::string PasHelpers::formatTrainIdForAudit( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& failedTrains )
{
    if ( 0 >= failedTrains.length() )
    {
        return "";
    }

    std::stringstream stmTrains;
    stmTrains << failedTrains[0];
    for ( int nLoop = 1; nLoop < failedTrains.length(); ++nLoop )
    {
        stmTrains << "," << failedTrains[nLoop];
    }

    return stmTrains.str().c_str();
}

std::string PasHelpers::formatDvaPriorityForAudit( const bool bEmergency )
{
    std::string strPriority = bEmergency ? "emergency" : "normal";

    return strPriority.c_str();
}

std::string PasHelpers::formatDvaVersionForAudit( const unsigned long ulVersion )
{
    std::stringstream strValue;

    strValue << (( ulVersion & 0xFF000000 ) >> 24) << '.' << (( ulVersion & 0x00FF0000 ) >> 16) << '.'
        << (( ulVersion & 0x0000FF00 ) >> 8) << '.' << ( ulVersion & 0x000000FF );

    return strValue.str().c_str();
}

std::string PasHelpers::formatDvaMsgIdForAudit( const unsigned long ulMsgKey )
{
    std::stringstream stmMsgId;

    TA_IRS_Bus::TrainDvaMessageRecord record;

    try
    {
        record = TA_IRS_Bus::CachedMaps::getInstance()->getTrainDvaMessageRecordFromKey( ulMsgKey );
        stmMsgId << record.m_pKeyOrId;
    }
    catch (std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unkonwn", "Caught unknown exception when get Train Dva message from cached map" );
    }

    return stmMsgId.str().c_str();
}

std::string PasHelpers::formatDvaMsgNameForAudit( const unsigned long ulMsgKey )
{
    std::string strMsgName = "";

    try
    {
        TA_IRS_Bus::StationDvaMessageRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( ulMsgKey );
        strMsgName = checkLabel( record.strMsgName );
    }
    catch (std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unkonwn", "Caught unknown exception when get Station Dva message from cached map" );
    }

    return strMsgName.c_str();
}

std::string PasHelpers::formatDvaMsgNamesForAudit( const  TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& seqMessages )
{
    if ( 0u == seqMessages.length() )
    {
        return "";
    }

    std::stringstream stmMsgLabels;

    stmMsgLabels << formatDvaMsgNameForAudit( seqMessages[0u] );

    TA_IRS_Bus::StationDvaMessageRecord record;
    for( unsigned int unLoop = 1u; unLoop < seqMessages.length(); ++unLoop )
    {
        try
        {
            record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( seqMessages[unLoop] );
            stmMsgLabels << "," << checkLabel( record.strMsgName );
        }
        catch (std::exception& expWhat)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unkonwn", "Caught unknown exception when get Station Dva message from cached map" );
        }
    }

    return stmMsgLabels.str().c_str();
}

void PasHelpers::formatPasOccZoneMatrix( 
    const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    unsigned long ulLocationKey = 0;
    unsigned short usZoneId = 0;
    unsigned short usStationId = 0;
    int nDesAddress = 0;
    unsigned short usZoneBit = 0;
    unsigned short usNewValue = 0;

    for ( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); vecZoneKeys.end() != itLoop; ++itLoop )
    {
        try
        {
            ulLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey( *itLoop );
            // Zone id base on PA_MIN_ZONE_ID (1)
            usZoneId = static_cast<unsigned short>(TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey( *itLoop ));
            // Station id base on PA_MIN_STN_ID (1)
            usStationId = static_cast<unsigned short>(TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey( ulLocationKey ));
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone key %d invalid", *itLoop );
            continue;
        }

        if (( TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID > usZoneId ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID < usZoneId ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone id %d is not in range", usZoneId );
            continue;
        }
        if (( TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID > usStationId ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID < usStationId ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station id %d is not in range", usStationId );
            continue;
        }

        // Offset to map modbus address
        usZoneId -= TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID; 
        usStationId -= TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 

        nDesAddress = static_cast<int>(usStationId) * PAS_EACH_STATION_WORD_COUNT + 
            static_cast<int>(usZoneId) / PAS_ADDRESS_LENGTH;
        nDesAddress += dbData.start();
        usZoneBit = 0x0001;
        usZoneBit <<= ( usZoneId % PAS_ADDRESS_LENGTH );
        if ( dbData.isInRange( nDesAddress ))
        {
            usNewValue = dbData[nDesAddress] | usZoneBit;
            dbData.set( nDesAddress, usNewValue );
        }
    }
}

void PasHelpers::formatPasStnZoneMatrix( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
	unsigned long ulLocationKey = 0;
	unsigned short usZoneId = 0;
	int nDesAddress = 0;
	unsigned short usZoneBit = 0;
	unsigned short usNewValue = 0;

	for ( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); vecZoneKeys.end() != itLoop; ++itLoop )
	{
		try
		{
			// Zone id base on PA_MIN_ZONE_ID (1)
			usZoneId = static_cast<unsigned short>(TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey( *itLoop ));
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone key %d invalid", *itLoop );
			continue;
		}

		if (( TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID > usZoneId ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID < usZoneId ))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone id %d is not in range", usZoneId );
			continue;
		}

		usZoneId -= TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID;

		nDesAddress = dbData.start() + static_cast<int>(usZoneId) / PAS_ADDRESS_LENGTH;
		usZoneBit = 0x0001;
		usZoneBit <<= ( usZoneId % PAS_ADDRESS_LENGTH );
		if ( dbData.isInRange( nDesAddress ))
		{
			usNewValue = dbData[nDesAddress] | usZoneBit;
			dbData.set( nDesAddress, usNewValue );
		}
	}
}

void PasHelpers::formatPasOccStationMatrix( 
    const std::vector<unsigned long>& vecZoneKeys, TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    unsigned long ulLocationKey = 0;
    unsigned short usStationId = 0;
    int nDesAddress = 0;
    std::set<unsigned short> setStations;
    const int ADDRESS_OFFSET = 1;

    for ( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); vecZoneKeys.end() != itLoop; ++itLoop )
    {
        try
        {
            ulLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey( *itLoop );
            // Station id base on PA_MIN_STN_ID (1)
            usStationId = static_cast<unsigned short>(TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey( ulLocationKey ));
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone key %d invalid", *itLoop );
            continue;
        }

        if (( TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID > usStationId ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID < usStationId ))
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station id %d is not in range", usStationId );
            continue;
        }

        // Offset to map ModBus address
        usStationId -= TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 

        if ( setStations.end() != setStations.find( usStationId ))
        {
            continue;
        }

        nDesAddress = dbData.start() + static_cast<int>(usStationId) * PAS_EACH_STATION_WORD_COUNT + ADDRESS_OFFSET;
        if ( dbData.isInRange( nDesAddress ))
        {
            dbData.set( nDesAddress, PAS_OCC_MUSIC_STATION_SELECTED );
        }

        setStations.insert( usStationId );
    }

    setStations.clear();
}

//Added by lixiaoxia
void PasHelpers::formatMultiMessagesIds( const std::vector<unsigned short>& vecMsgIds, TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    int nIndex = dbData.start();
	for( std::vector<unsigned short>::const_iterator itLoop = vecMsgIds.begin(); vecMsgIds.end() != itLoop; ++itLoop )
	{
		dbData.set( nIndex, *itLoop );
		++nIndex;

        if ( nIndex > dbData.end() )
        {
            break;
        }
	}

    for ( ; nIndex <= dbData.end(); ++nIndex )
    {
        dbData.set( nIndex, 0u );
    }
}

void PasHelpers::corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refCorbaConfig, StdStationDvaConfig& stToConverted )
{
    for ( int nZoneLoop = 0; nZoneLoop < refCorbaConfig.zones.length(); ++nZoneLoop )
    {
        stToConverted.vecZoneKeys.push_back( refCorbaConfig.zones[nZoneLoop] );
    }

    stToConverted.vecMsgKeys.clear();
    stToConverted.vecMsgIds.clear();
    if ( 0u < refCorbaConfig.messages.length() )
    {
		for (unsigned int unMessageLoop = 0; unMessageLoop < refCorbaConfig.messages.length(); ++unMessageLoop)
		{
            try
            {
				unsigned long ulMsgKey = refCorbaConfig.messages[unMessageLoop];
                // Try to get the correct message id
                unsigned short usDvaMsgId = static_cast<unsigned short>( TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey( ulMsgKey ));
				stToConverted.vecMsgIds.push_back( usDvaMsgId );
				stToConverted.vecMsgKeys.push_back( ulMsgKey );
            }
            catch (...)
            {
                // Do nothing
            }
        }
    }
    else
    {
    }

    stToConverted.dwellInSecs = convertShortToBcdFormat(refCorbaConfig.dwellInSecs);
    stToConverted.hasChime = refCorbaConfig.hasChime;
    stToConverted.isLocalPlayback = refCorbaConfig.isLocalPlayback;

	//Get original start time and end time for tis.If no cyclic,this start time is replaced with current time and end time with starttime + 60s.
	time_t currentTime = time(NULL);
	const unsigned long SECONDS_FOR_ONE_MINUTE = 60;
	unsigned long ulOriginalStartTime = convertTimeToHourMin(static_cast<const time_t>(refCorbaConfig.stCyclicParams.startTime));
	unsigned long ulOriginalEndTime = convertTimeToHourMin(static_cast<const time_t>(refCorbaConfig.stCyclicParams.stopTime));
	stToConverted.ulOriginalStartTime = static_cast<unsigned long>(refCorbaConfig.bCyclic ? ulOriginalStartTime : currentTime);
	stToConverted.ulOriginalEndTime = static_cast<unsigned long>(refCorbaConfig.bCyclic ? ulOriginalEndTime : (currentTime + SECONDS_FOR_ONE_MINUTE));

    convertTimeToBcdFormat( static_cast<const time_t>(refCorbaConfig.stCyclicParams.period), stToConverted.usInterval, stToConverted.usIntervalSecond );
    convertTimeToBcdFormat( static_cast<const time_t>(refCorbaConfig.stCyclicParams.startTime), stToConverted.usStartTime, stToConverted.usStartSecond );
    convertTimeToBcdFormat( static_cast<const time_t>(refCorbaConfig.stCyclicParams.stopTime), stToConverted.usEndTime, stToConverted.usEndSecond );

	stToConverted.isSynchronisedWithTis = refCorbaConfig.isSynchronisedWithTis;
    stToConverted.bScheduled = refCorbaConfig.bCyclic;
    stToConverted.usScheduleId = refCorbaConfig.stCyclicParams.usScheduleId;
    stToConverted.usSourceId = refCorbaConfig.sourceId;
    stToConverted.usDaysInWeek = convertDaysInWeek( refCorbaConfig.stCyclicParams.bCycSun, 
        refCorbaConfig.stCyclicParams.bCycMon, refCorbaConfig.stCyclicParams.bCycTue, 
        refCorbaConfig.stCyclicParams.bCycWed, refCorbaConfig.stCyclicParams.bCycThu, 
        refCorbaConfig.stCyclicParams.bCycFri, refCorbaConfig.stCyclicParams.bCycSat );
}

void PasHelpers::corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig& refCorbaConfig, StdStationLiveConfig& stToConverted )
{
    stToConverted.usSourceId = static_cast<unsigned short>(refCorbaConfig.sourceId);
    stToConverted.bWithChime = refCorbaConfig.hasChime;
    for ( int nZoneLoop = 0; nZoneLoop < refCorbaConfig.zones.length(); ++nZoneLoop )
    {
        stToConverted.vecZoneKeys.push_back( refCorbaConfig.zones[nZoneLoop] );
    }
}

void PasHelpers::corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationRecordingConfig& refCorbaConfig, StdStationRecordingConfig& stToConverted )
{
    stToConverted.usMsgId = 0u;

    try
    {
        StationDvaMessageRecord stDvaMsg = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( refCorbaConfig.messageKey );
        stToConverted.usMsgId = stDvaMsg.usMsgId;
    }
    catch (...)
    {
        // Do nothing
    }

    stToConverted.usMsgType = refCorbaConfig.usMsgType;
    stToConverted.ulMsgKey = refCorbaConfig.messageKey;
    stToConverted.usSourceId = refCorbaConfig.sourceId;
}

void PasHelpers::corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refCorbaConfig, ScheduleDvaConfig& stToConverted )
{
    for ( int nZoneLoop = 0; nZoneLoop < refCorbaConfig.zones.length(); ++nZoneLoop )
    {
        stToConverted.lstZoneKeys.push_back( refCorbaConfig.zones[nZoneLoop] );
    }

    for ( unsigned int unMessageLoop = 0; unMessageLoop < refCorbaConfig.messages.length(); ++unMessageLoop )
	{
        stToConverted.lstMsgKeys.push_back( refCorbaConfig.messages[unMessageLoop] );
	}

    stToConverted.bChime = refCorbaConfig.hasChime;
    stToConverted.ulInterval = refCorbaConfig.stCyclicParams.period;
    stToConverted.ulStartTime = refCorbaConfig.stCyclicParams.startTime;
    stToConverted.ulEndTime = refCorbaConfig.stCyclicParams.stopTime;
    stToConverted.usScheduleId = refCorbaConfig.stCyclicParams.usScheduleId;
    stToConverted.bSun = refCorbaConfig.stCyclicParams.bCycSun;
    stToConverted.bMon = refCorbaConfig.stCyclicParams.bCycMon;
    stToConverted.bTue = refCorbaConfig.stCyclicParams.bCycTue;
    stToConverted.bWed = refCorbaConfig.stCyclicParams.bCycWed;
    stToConverted.bThu = refCorbaConfig.stCyclicParams.bCycThu;
    stToConverted.bFri = refCorbaConfig.stCyclicParams.bCycFri;
    stToConverted.bSat = refCorbaConfig.stCyclicParams.bCycSat;
}

void PasHelpers::corbaConfigToStdConfig( const TA_Base_Bus::IPAAgentCorbaDef::StationMusicConfig& refCorbaConfig, StdStationMusicConfig& stToConverted )
{
    for ( int nZoneLoop = 0; nZoneLoop < refCorbaConfig.zones.length(); ++nZoneLoop )
    {
        stToConverted.vecZoneKeys.push_back( refCorbaConfig.zones[nZoneLoop] );
    }
    stToConverted.usSourceId = refCorbaConfig.sourceId;
    stToConverted.musicType = refCorbaConfig.musicType;
}

bool PasHelpers::isAdHocDvaMsg( const unsigned short usMsgType )
{
    bool bIsAdHocDva = 
        (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == usMsgType ) || 
         ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == usMsgType ) ||
         ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == usMsgType ));

    return bIsAdHocDva;
}

unsigned short PasHelpers::convertDaysInWeek( const bool bSun, const bool bMon, 
    const bool bTue, const bool bWed, const bool bThu, const bool bFri, const bool bSat )
{
    unsigned short usDaysInWeek = 0u;

    usDaysInWeek |= ( bSun ? 0x0001u : 0 );
    usDaysInWeek |= ( bMon ? 0x0002u : 0 );
    usDaysInWeek |= ( bTue ? 0x0004u : 0 );
    usDaysInWeek |= ( bWed ? 0x0008u : 0 );
    usDaysInWeek |= ( bThu ? 0x0010u : 0 );
    usDaysInWeek |= ( bFri ? 0x0020u : 0 );
    usDaysInWeek |= ( bSat ? 0x0040u : 0 );

    usDaysInWeek |= 0x0080u;

    return usDaysInWeek;
}

bool PasHelpers::isLiveBroadcast(const unsigned short usZoneState)
{
    std::map<unsigned short, std::string> mapPagingConsoleKeys = CachedConfig::getInstance()->getPagingConsoles();
    
    bool isLive = mapPagingConsoleKeys.find( usZoneState ) != mapPagingConsoleKeys.end();

    return isLive;
}

void PasHelpers::convertTimeToBcdFormat( const time_t tmTime, unsigned short& usHourMinute, unsigned short& usSecond )
{
    tm stTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ACE_OS::localtime_r( &tmTime, &stTime );
    const unsigned short usHour = static_cast<unsigned short>(stTime.tm_hour);
    const unsigned short usMin = static_cast<unsigned short>(stTime.tm_min);
    const unsigned short usSec = static_cast<unsigned short>(stTime.tm_sec);

    usSecond = 0u;

    // 00S0
    usSecond |= ((usSec / 10) << 4);
    // 00SS
    usSecond |= (usSec % 10);

    usHourMinute = 0u;

    // H000
    usHourMinute |= ((usHour / 10) << 12);
    // HH00
    usHourMinute |= ((usHour % 10) << 8);
    // HHM0
    usHourMinute |= ((usMin / 10) << 4);
    // HHMM
    usHourMinute |= (usMin % 10);
}

unsigned long PasHelpers::convertTimeToHourMin( const time_t tmTime )
{
	tm stTime = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ACE_OS::localtime_r( &tmTime, &stTime );
	stTime.tm_sec = 0;

    return static_cast<unsigned long>( ACE_OS::mktime( &stTime ) );
}

unsigned short PasHelpers::convertShortToBcdFormat( unsigned short usValue)
{
	unsigned short usBcdValue = 0u;

	// X000
	usBcdValue |= ((usValue / 1000) << 12);

	// XX00
	usBcdValue |= (((usValue % 1000) / 100) << 8);

	// XXX0
	usBcdValue |= (((usValue % 100) / 10) << 4);

	// XXXX
	usBcdValue |= (usValue % 10);

	return usBcdValue;
}

std::string PasHelpers::formatFailReasonForAudit( const int localState )
{
    std::string reason = "";

    switch (localState)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS: reason = "In progress"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_OVERRIDDEN: reason = "In progress with overridden"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_DWELL_TIME: reason = "In progress dwell time"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED: reason = "PTT is activated"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING: reason = "Connecting"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY: reason = "Train ready"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED: reason = "Continue live announcement required"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED: reason = "Completed"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED: reason = "Aborted"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CYCLE: reason = "Failed cycle"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE: reason = "Failed zone"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN: reason = "Overridden"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN: reason = "Failed train"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET: reason = "Connection reset"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INTERRUPTED: reason = "Failed interrupted"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_VERSION_CONFLICT: reason = "Version conflict"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST: reason = "Invalid request"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY: reason = "Busy"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY_WITH_OVERRIDE_OPTION: reason = "Busy with override option"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT: reason = "TIMS continue request not received"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_PERMISSION: reason = "No duty for this train at this location"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED: reason = "Train first cycle cancelled"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BAD_COMMAND: reason = "Failed bad command"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_ORIGIN: reason = "Failed different origin"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_VERSION: reason = "Failed different version"; break;
        // DTL-735 add start
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_RESPONSE_FROM_TRAIN: reason = "Failed no response from train"; break;
        // DTL-735 add end
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER: reason = "Failed mpu changeover"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_LINK_LOST: reason = "Link Lost"; break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DUE_TO_NO_AUDIO: reason = "Failed due to no audio"; break;
    default: reason = "Unknown"; break;
    }

    return reason;
}

void PasHelpers::stdConfigToCorbaConfig( const ScheduleDvaConfig& refStConfig, TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& corbaToConverted )
{
    corbaToConverted.zones.length( refStConfig.lstZoneKeys.size() );
    _CORBA_ULong indexZone = 0;
    for( std::list<unsigned long>::const_iterator it = refStConfig.lstZoneKeys.begin(); it != refStConfig.lstZoneKeys.end(); ++it )
    {
        corbaToConverted.zones[indexZone] = *it;
        indexZone++;
    }

    corbaToConverted.messages.length( refStConfig.lstMsgKeys.size() );
    _CORBA_ULong indexMsg = 0;
    for( std::list<unsigned long>::const_iterator it = refStConfig.lstMsgKeys.begin(); it != refStConfig.lstMsgKeys.end(); ++it )
    {
        corbaToConverted.messages[indexMsg] = *it;
        indexMsg++;
    }

    corbaToConverted.sourceId = 0;
    corbaToConverted.dwellInSecs = 0;
    corbaToConverted.isSynchronisedWithTis = false;
    corbaToConverted.isLocalPlayback = false;
    corbaToConverted.bCyclic = true;

    corbaToConverted.hasChime = refStConfig.bChime;
    corbaToConverted.stCyclicParams.period = refStConfig.ulInterval;
    corbaToConverted.stCyclicParams.startTime = refStConfig.ulStartTime;
    corbaToConverted.stCyclicParams.stopTime = refStConfig.ulEndTime;
    corbaToConverted.stCyclicParams.usScheduleId = refStConfig.usScheduleId;
    corbaToConverted.stCyclicParams.bCycSun = refStConfig.bSun;
    corbaToConverted.stCyclicParams.bCycMon = refStConfig.bMon;
    corbaToConverted.stCyclicParams.bCycTue = refStConfig.bTue;
    corbaToConverted.stCyclicParams.bCycWed = refStConfig.bWed;
    corbaToConverted.stCyclicParams.bCycThu = refStConfig.bThu;
    corbaToConverted.stCyclicParams.bCycFri = refStConfig.bFri;
    corbaToConverted.stCyclicParams.bCycSat = refStConfig.bSat;
}

}