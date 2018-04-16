/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainMisc.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

///////////////////////////////////////////////////////////
//  TrainMisc.cpp
//  Implementation of the Class TrainMisc
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#include <sstream>

#include "TrainMisc.h"
#include "ITrainManager.h"
#include "ITrainManagerTrainEventFacade.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/TrainTimer.h"
#include "app/trains/train_agent/src/TrainManager.h"
#include "app/trains/train_agent/src/TrainTIS.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscChangeAreaCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscChangeAreaReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscChangeoverStatusEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscTestCallEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscTestCallResultCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscRadioModeCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/misc0A1TableEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paAudioSWCabToCabEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paAudioSWEventReader.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/message/types/TrainAgentAlarms_MessageTypes.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_IRS_App;

TrainMisc::TrainMisc(ITrainManager& callback,
                     ITrainManagerTrainEventFacade& trainEventHandler,
                     ITrainNotificationFacade& notifier,
                     TrainTypes::TrainAgentLocationType agentLocationType,
                     unsigned long subsystemKey /* used to raise alarms */ ) 
: 
m_trainManager(callback),
m_trainEventHandler(trainEventHandler),
m_notifier(notifier),
m_agentLocationType(agentLocationType),
//TD18093,jianghp
//m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper()),
//TD18093,jianghp
m_locationName(""),
m_subsystem(subsystemKey)
{
    m_trainManager.addDelegate(this);
    m_trainManager.addListener(this);

    // Get Timer values (from TrainAgent entity data)
    m_timerRadioMode = m_trainManager.getGlobalTrainAgentEntity()->getTimerRadioMode();

    try
    {
        TA_Base_Core::ILocation* location = NULL;
	    location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(
            m_trainManager.getLocalTrainAgentEntity()->getLocation() );
        if (location != NULL)
        {
            m_locationName = location->getName();
			m_locationKey = location->getKey();	//hongzhi, for alarm
            delete location;
        }
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "...", "Retrieving location name");
    }


    // if this agent is starting in control, close any open radio mode alarms
    if (TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE)== RPARAM_CONTROL)
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation( TA_Base_Core::TrainAgentAlarms::RadioModeTimerAlarm, m_locationKey, 
			                     m_trainManager.getLocalTrainAgentEntity()->getKey() );

    }
}



TrainMisc::~TrainMisc()
{
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
}

void TrainMisc::receive(TrainEvent& event)
{
    // a train event has been received

    unsigned char trainId = event.getTrainID();

    // close the degraded radio mode alarm if it is open
    closeAlarm(trainId);
}


void TrainMisc::removeTrain(unsigned int trainId)
{
    // train has gone away
    // close the degraded radio mode alarm if it is open
    closeAlarm(trainId);
}


//ITrainEventReaderDelegator::delegatorCallback TrainMisc::delegate(TrainEvent& event)
bool TrainMisc::delegate(TrainEvent& event)
{
    TrainEventReader* r = event.getReader();
    bool consumed = true;
    if (dynamic_cast<paAudioSWCabToCabEventReader*>(r) != 0)
    {
        onPAAudioSWCabToCab(event, dynamic_cast<paAudioSWCabToCabEventReader*>(r));
    }
    else if (dynamic_cast<paAudioSWEventReader*>(r) != 0)
    {
        onPAAudioSW(event, dynamic_cast<paAudioSWEventReader*>(r));
    }
    else if (dynamic_cast<miscTestCallEventReader*>(r) != 0)
    {
        onTestCall(event, dynamic_cast<miscTestCallEventReader*>(r));
    }
    else if (dynamic_cast<miscChangeoverstatusEventReader*>(r) != 0)
    {
        onChangeOverStatus(event, dynamic_cast<miscChangeoverstatusEventReader*>(r));
    }
    else 
    {
        consumed = false;
    }
    return consumed;
}

void TrainMisc::degradedRadioMode(unsigned char trainID, unsigned char radioMode, std::string sessionID)
{
	try
	{
		// Send command to train
		miscRadioModeCommand command; 
		
		switch (radioMode) 
		{ 
		case TrainMisc::Radio_Normal: 
			command.setRadioModeStatus(miscRadioModeCommand::NORMAL); 
			break;
		case TrainMisc::Radio_Degraded:
			command.setRadioModeStatus(miscRadioModeCommand::DEGRADED); 
			break;
		}
		
		TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerRadioMode);
	
		/** reinsert this event back into stream so it can be processed
		* It could be the response for anything, so we cannot create a reader from it.
		* It could also be an OA1 or ATC table
		*/
		m_trainEventHandler.onTrainEvent(*event);

	}
    catch (TrainException & e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

        // an error occurred, raise a degraded radio mode alarm if none exist
        submitAlarm(trainID);

        TA_RETHROW( e );
    }
}

void TrainMisc::wakeupChangeArea(unsigned char trainID, unsigned char status, std::string sessionID)
{
    try
    {
        // Send command to train
        miscChangeAreaCommand command; 
        
        switch (status) 
        { 
        case miscChangeAreaCommand::MAINLINE: 
            command.setAreaStatus(miscChangeAreaCommand::MAINLINE); 
            break;
        case miscChangeAreaCommand::DEPOT: 
            command.setAreaStatus(miscChangeAreaCommand::DEPOT); 
            break;
        }
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command);
        
        // Check event reply
        miscChangeAreaReceivedEventReader* reader = dynamic_cast<miscChangeAreaReceivedEventReader*>(event->getReader());
        if (0 == reader)
        {
            TA_THROW( TrainGeneralFailureException() );
        }
        
        switch (reader->getAreaStatus())
        {
        case miscChangeAreaReceivedEventReader::MAINLINE:
        case miscChangeAreaReceivedEventReader::DEPOT:
            ;
        }

    }
    catch (TrainException & e)
    {
        TA_RETHROW( e );
    }
}


void TrainMisc::onPAAudioSWCabToCab(TrainEvent &event, paAudioSWCabToCabEventReader * reader)
{
    // No need to send any notifications, as noone is listening for them. 
    // Also, no TrainStatusInformation to record
}


void TrainMisc::onPAAudioSW(TrainEvent &event, paAudioSWEventReader * reader)
{
    // No need to send any notifications, as noone is listening for them. 
    // Also, no TrainStatusInformation to record
}



void TrainMisc::onTestCall(TrainEvent &event, miscTestCallEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver tsuoLibVers(m_trainManager.getTrainStatusManager(), trainID, syncLibraryVersions);

	unsigned char whichMPU;
	// do this explicitly, in case origin values change
	switch (event.getOrigin()) 
	{
	case 0: 
		whichMPU = 0; // main
		break;
	case 1:
		whichMPU = 1; // standby
		break;
	default:
		// error, invalid origin, get out of here...
		std::ostringstream s;
		s << "Invalid Origin detected in TestCall Message [" << static_cast<unsigned int>(event.getOrigin()) << "]" << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
		return;
	}

    // need to add this train to the info table...

	/// @note assuming a test call is generated when train wakes up from depot, until ats sends a notification to say otherwise
	// This if block looks weird but the logic may change later...
    if(m_trainManager.getTrainStatusManager().getTrainLocation(trainID) == 0)
    {
        m_trainManager.getTrainStatusManager().setTrainLocation(trainID, 0);
    }
    m_trainManager.getTrainStatusManager().setTTISLibraryVersion(trainID, reader->getTISLibraryVersion());
    m_trainManager.getTrainStatusManager().setTTISScheduleVersion(trainID, reader->getMessageScheduleLibraryVersion());
    m_trainManager.getTrainStatusManager().setPALibraryVersion(trainID, reader->getPreRecordedLibraryVersion());
    

    // send reply
    miscTestCallResultCommand command; 
    /// @todo Get proper status from somewhere - ie. ATS/TrainStatusManager

	unsigned long location = m_trainManager.getTrainStatusManager().getTrainLocation(trainID);
	if (location == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey() )
	{
	    command.setCallResultStatus(miscTestCallResultCommand::DEPOT);
    } 
	else 
	{
		command.setCallResultStatus(miscTestCallResultCommand::MAINLINE);
    }

	// only send reply if in our area, and our responsibility
    m_trainManager.sendTrainCommandNoReply(trainID, command);

	// Inform TIS of changes (note: success and sessionID fields meaningless for this message)
    m_notifier.notifyTIS(TrainTIS::TIS_LibraryVersion, trainID, false, "", reader->getTISLibraryVersion(), reader->getMessageScheduleLibraryVersion() ); 
    m_notifier.notifyPublicAddress(trainID, reader->getPreRecordedLibraryVersion()); 
}


void TrainMisc::onChangeOverStatus(TrainEvent &event, miscChangeoverstatusEventReader * reader)
{
    // unsigned int trainID = event.getTrainID();
	//TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);

//    std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID);   

    reader->getCompletionStatus(); // always returns OK
    reader->getMPUSelectionStatus();

	/// @todo enable this...
	//m_trainStatusManager.setTrainTSI(trainId, subscriberId);

    // noone to notify
    // m_notifier.notifyMisc(0, trainID, sessionID); 
}


void TrainMisc::submitAlarm(unsigned int trainId)
{
	FUNCTION_ENTRY("submitAlarm");

	// Only need to raise an alarm if one doesn't already exist.

    std::string alarmId = "";
    try
    {
        alarmId = m_trainManager.getTrainStatusManager().getRadioModeAlarmId(trainId);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Could not retrieve radio mode alarm id for train %d", trainId);

		FUNCTION_EXIT;
		return;
	}

	if (alarmId != "")
	{
        // alarm already raised
		FUNCTION_EXIT;
		return;
	}

 
	TA_Base_Core::DescriptionParameters descriptionParameters;

    std::ostringstream trainIdStream;
	trainIdStream << static_cast<int>(trainId) << std::ends;
	TA_Base_Core::NameValuePair pair1 ( "PV", trainIdStream.str().c_str());
	descriptionParameters.push_back ( &pair1 );

    std::ostringstream serviceStream;
    serviceStream << m_trainManager.getTrainStatusManager().getTrainServiceNumber(trainId) << std::ends;
    TA_Base_Core::NameValuePair pair2 ( "SERVICE", serviceStream.str().c_str());
	descriptionParameters.push_back ( &pair2 );

	TA_Base_Core::NameValuePair pair3 ( "LOCATION", m_locationName.c_str());
	descriptionParameters.push_back ( &pair3 );


	alarmId = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(TA_Base_Core::TrainAgentAlarms::RadioModeTimerAlarm, 
		m_trainManager.getLocalTrainAgentEntity()->getKey(),
		m_trainManager.getLocalTrainAgentEntity()->getTypeKey(),
		descriptionParameters, 
		m_trainManager.getLocalTrainAgentEntity()->getName(),
		m_trainManager.getLocalTrainAgentEntity()->getLocation(),
		m_subsystem,
		m_trainManager.getLocalTrainAgentEntity()->getAssetName(),
		TA_Base_Core::AlarmConstants::defaultTime );

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Submitted radio mode alarm id %s for train %d",
		alarmId.c_str(), trainId);

    TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainId, syncAlarmIDs);

	try
	{
        m_trainManager.getTrainStatusManager().setRadioModeAlarmId(trainId, alarmId);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Could not set radio mode alarm id for train %d", trainId);
	}

	FUNCTION_EXIT;
}

void TrainMisc::closeAlarm(unsigned int trainId)
{
	FUNCTION_ENTRY("closeAlarm");		

	// Only need to close an alarm if one exists.
	std::string alarmId = "";
    try
    {
        alarmId = m_trainManager.getTrainStatusManager().getRadioModeAlarmId(trainId);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Could not retrieve radio mode alarm id for train %d", trainId);

		FUNCTION_EXIT;
		return;
	}

	if (alarmId == "")
	{
		FUNCTION_EXIT;
		return;
	}

	TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmId, m_locationKey);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
		TA_Base_Core::DebugUtil::DebugInfo, "Closed radio mode alarm id %s for train %d",
		alarmId.c_str(), trainId);

    TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainId, syncAlarmIDs);

	try
	{
        m_trainManager.getTrainStatusManager().resetRadioModeAlarmId(trainId);
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Could not set radio mode alarm id for train %d", trainId);
	}

	FUNCTION_EXIT;
}

std::string TrainMiscChangeModeCommand::info()
{
	std::ostringstream s;
	s << "TrainMiscChangeModeCommand [Train=" << static_cast<int>(m_trainId);
	s << ", ChangeArea=" << (m_changeArea?"True":"False") << "]";


	return s.str();
}

int TrainMiscChangeModeCommand::call()
{
	FUNCTION_ENTRY("TrainMiscChangeModeCommand::call()");

	try
	{
		m_trainMisc.degradedRadioMode(m_trainId, m_radioMode, m_session);

		if (m_changeArea)
		{
			m_trainMisc.wakeupChangeArea(m_trainId, m_status, m_session);
		}
	}
	catch (TrainException &e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in Misc worker thread");
	}
	
	FUNCTION_EXIT;
	return 0;
	
}