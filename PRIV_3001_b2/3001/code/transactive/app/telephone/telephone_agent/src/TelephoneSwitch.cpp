/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/TelephoneSwitch.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the implementation for the TelephoneSwitch class
  */

#include "TSAPINotificationManager.h"
#include "TelephoneSwitch.h"
#include "HiddenDialog.h"
#include "core/Corba/src/CorbaUtil.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"

namespace TA_App
{
	/**
	  * TelephoneSwitch
	  */
	TelephoneSwitch::TelephoneSwitch(std::string switchName)
	{
		//connect to the switch
		m_retCode = acsOpenStream (&m_acsHandle, LIB_GEN_ID, 0, ST_CSTA2,
					(ServerID_t FAR*)"ALCATEL#xm000000#CSTA#renameme",
					(LoginID_t FAR*)"mtcl", (Passwd_t FAR*)"mtcl",
					(AppName_t FAR*)switchName.c_str(), ACS_LEVEL1,
					(Version_t *)"TS2", 0, 0, 0, 0, NULL);
		//get the event that confirmes the correct connection to the stream
		getEventFromSwitchBlocking();
		//create a notification manager to automatically receive the events
		m_notificationManager = new TSAPINotificationManager(this);
		//register the notificationmanager with the TSAPI server
		m_retCode = acsEventNotify (m_acsHandle, m_notificationManager->getWindowHandle(), WM_ACSEVENT, TRUE);
		//add telephones
		std::vector<TA_Core::IEntityData*> telephones = TA_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(switchName);
		for (unsigned int i(0); i < telephones.size(); i++)
		{
			Telephone* telephone = new Telephone(m_acsHandle,telephones[i]->getName(),telephones[i]->getAddress(),50);
			m_telephones.push_back(telephone);
		}
	}

	/**
	  * ~TelephoneSwitch
	  */
	TelephoneSwitch::~TelephoneSwitch()
	{
		//clean up
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//delete each telephone
			delete m_telephones[i];
		}
	}

	/**
	  * getSnapShotOfSwitch
	  */
	void TelephoneSwitch::getSnapShotOfSwitch() 
	{
		//get snapshots for all telephones on this switch
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//create a device ID
			DeviceID_t deviceID;
			//copy the
			strcpy(deviceID, m_telephones[i]->getNumber().c_str());
//			m_nRetCode = cstaSnapshotDeviceReq (m_acsHandle, 0, &deviceID, NULL);
			break;
		}
	}

	/**
	  * getEventFromSwitchBlocking
	  */
	int TelephoneSwitch::getEventFromSwitchBlocking () 
	{
		unsigned short  cstaEventSize;
		unsigned short  numEvents;
		CString eventString;

		cstaEventSize = sizeof (m_cstaEvent);
		m_retCode = acsGetEventBlock (m_acsHandle,
				(void FAR *)&m_cstaEvent,
				(unsigned short FAR *)&cstaEventSize,
				NULL,
				(unsigned short FAR *)&numEvents);
//TEMPORARY
		switch (m_cstaEvent.eventHeader.eventClass) 
		{
		case ACSREQUEST :
			eventString = "ACS REQUEST";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case ACS_OPEN_STREAM :
			  eventString += "Open Stream";
			  break;
			case ACS_CLOSE_STREAM :
			  eventString += "Close Stream";
			  break;
			case ACS_ABORT_STREAM :
			  eventString += "Abort Stream";
			  break;
			case ACS_UNIVERSAL_FAILURE :
			  eventString += "Universal Failure";
			  break;
			case ACS_KEY_REQUEST :
			  eventString += "Key Request";
			  break;
			case ACS_KEY_REPLY :
			  eventString += "Key Reply";
			  break;
			default :
			  eventString += "Unknown type";
			  break;
		  }
		  break;
		case ACSUNSOLICITED :
		  eventString = "Unsollicited";
		  break;
		case ACSCONFIRMATION :
		  eventString = "ACS CONFIRM ";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case ACS_OPEN_STREAM :
			  eventString += "Open Stream";
			  break;
			case ACS_OPEN_STREAM_CONF :
				ACSOpenStreamConfEvent_t *pEvent;
				eventString = "CSTA Application. Connected to ";
				eventString += "ALCATEL";
				pEvent = &m_cstaEvent.event.acsConfirmation.u.acsopen;
			  eventString += "\nACS CONFIRM Open Stream";
			  eventString += "\n           API Version: ";
			  eventString += pEvent->apiVer;
			  eventString += "\n           Library Version: ";
			  eventString += pEvent->libVer;
			  eventString += "\n           TSERVER Version: ";
			  eventString += pEvent->tsrvVer;
			  eventString += "\n           Driver Version: ";
			  eventString += pEvent->drvrVer;
			  break;
			case ACS_CLOSE_STREAM :
			  eventString += "Close Stream";
			  break;
			case ACS_CLOSE_STREAM_CONF :
			  eventString += "Close Stream Confirmed";
			  break;
			case ACS_ABORT_STREAM :
			  eventString += "Abort Stream";
			  break;
			case ACS_UNIVERSAL_FAILURE_CONF :
			  eventString += "Universal Failure Confirmed";
			  break;
			case ACS_UNIVERSAL_FAILURE :
			  eventString += "Universal Failure";
			  break;
			case ACS_KEY_REQUEST :
			  eventString += "Key Request";
			  break;
			case ACS_KEY_REPLY :
			  eventString += "Key Reply";
			  break;
			default :
			  eventString += "Unknown Failure Type";
			  break;
		  }
		  break;
		case CSTAREQUEST :
		  eventString = "CSTA REQUEST ";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case CSTA_ESCAPE_SVC_REQ :
			  eventString += "Escape Service Request";
			  break;
			case CSTA_SYS_STAT_REQ :
			  eventString += "System State Request";
			  break;
			case CSTA_ROUTE_REQUEST_EXT :
			  eventString += "ROute Request";
			  break;
			case CSTA_RE_ROUTE_REQUEST :
			  eventString += "Re-Route request";
			  break;
			default :
			  eventString += "Unknown request";
			  break;
		  }
		  break;
		case CSTAUNSOLICITED :
		  eventString = "CSTA UNSOLICITED EVENT ";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case CSTA_CALL_CLEARED :
			  eventString += "Call cleared";
			  break;
			case CSTA_CONFERENCED :
			  eventString += "Conferenced";
			  break;
			case CSTA_CONNECTION_CLEARED :
			  eventString += "Connection cleared";
			  break;
			case CSTA_DELIVERED :
			  eventString += "Delivered";
			  break;
			case CSTA_DIVERTED :
			  eventString += "Diverted";
			  break;
			case CSTA_ESTABLISHED :
			  eventString += "Established";
			  break;
			case CSTA_FAILED :
			  eventString += "Failed";
			  break;
			case CSTA_HELD :
			  eventString += "Held";
			  break;
			case CSTA_NETWORK_REACHED :
			  eventString += "Network reached";
			  break;
			case CSTA_ORIGINATED :
			  eventString += "Originated";
			  break;
			case CSTA_QUEUED :
			  eventString += "Queued";
			  break;
			case CSTA_RETRIEVED :
			  eventString += "Retrieved";
			  break;
			case CSTA_SERVICE_INITIATED :
			  eventString += "Service Initiated";
			  break;
			case CSTA_TRANSFERRED :
			  eventString += "Transferred";
			  break;
			case CSTA_CALL_INFORMATION :
			  eventString += "Call information";
			  break;
			case CSTA_DO_NOT_DISTURB :
			  eventString += "Do Not disturb";
			  break;
			case CSTA_FORWARDING :
			  eventString += "Forwarding";
			  break;
			case CSTA_MESSAGE_WAITING :
			  eventString += "MessageWaiting";
			  break;
			case CSTA_LOGGED_ON :
			  eventString += "Logged On";
			  break;
			case CSTA_LOGGED_OFF :
			  eventString += "Logged Off";
			  break;
			case CSTA_NOT_READY :
			  eventString += "Not Ready";
			  break;
			case CSTA_READY :
			  eventString += "Ready";
			  break;
			case CSTA_WORK_NOT_READY :
			  eventString += "Work Not ready";
			  break;
			case CSTA_WORK_READY :
			  eventString += "Work Ready";
			  break;
			case CSTA_PRIVATE_STATUS :
			  eventString += "Private Status";
			  break;
			case CSTA_BACK_IN_SERVICE :
			  eventString += "Back in Service";
			  break;
			case CSTA_OUT_OF_SERVICE :
			  eventString += "Out Of Service";
			  break;
			case CSTA_MONITOR_ENDED :
			  break;
			case CSTA_RECORD_EVENT :
			  eventString += "Record Event";
			  break;
			case CSTA_STOP_EVENT :
			  eventString += "Stop Event";
			  break;
			default :
			  eventString += "unknown type";
			  break;
		  }
		  break;
		case CSTACONFIRMATION :
		  eventString = "CSTA CONFIRM ";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case CSTA_ALTERNATE_CALL_CONF :
			  eventString += "Alternate Call";
			  break;
			case CSTA_ASSOCIATE_DATA_CONF:
			  eventString += "Associate Data Confirmed";
			  break;
			case CSTA_ANSWER_CALL_CONF :
			  eventString += "Answer Call Confirmed";
			  break;
			case CSTA_CALL_COMPLETION_CONF : 
			  eventString += "Call Completion Confirmed";
			  break;
			case CSTA_CLEAR_CALL_CONF :
			  eventString += "Clear Call Confirmed";
			  break;
			case CSTA_CLEAR_CONNECTION_CONF :
			  eventString += "Clear Connection Confirmed";
			  break;
			case CSTA_CONFERENCE_CALL_CONF :
			  eventString += "Conference Call Confirmed";
			  break;
			case CSTA_CONSULTATION_CALL_CONF :
			  eventString += "Consultation Call Confirmed";
			  break;
			case CSTA_DEFLECT_CALL_CONF :
			  eventString += "Deflect Call Confirmed";
			  break;
			case CSTA_PARK_CALL_CONF :
			  eventString += "Park Call Confirmed";
			  break;
			case CSTA_PICKUP_CALL_CONF :
			  eventString += "Pickup Call Confirmed";
			  break;
			case CSTA_GROUP_PICKUP_CALL_CONF :
			  eventString += "Group Call Confirmed";
			  break;
			case CSTA_HOLD_CALL_CONF :
			  eventString += "Hold Call Confirmed";
			  break;
			case CSTA_MAKE_CALL_CONF :
			  eventString += "Make Call Confirmed";
			  break;
			case CSTA_MAKE_PREDICTIVE_CALL_CONF :
			  eventString += "Predictive Call Confirmed";
			  break;
			case CSTA_QUERY_MWI_CONF :
			  eventString += "Query Message Waiting ind Confirmed";
			  break;
			case CSTA_QUERY_DND_CONF :
			  eventString += "Query DND Confirmed";
			  break;
			case CSTA_QUERY_FWD_CONF :
			  eventString += "Query Forwarding Confirmed";
			  break;
			case CSTA_QUERY_AGENT_STATE_CONF :
			  eventString += "Query agent State Confirmed";
			  break;
			case CSTA_QUERY_LAST_NUMBER_CONF :
			  eventString += "Query last Number Confirmed";
			  break;
			case CSTA_QUERY_DEVICE_INFO_CONF :
			  eventString += "Query Device Info Confirmed";
			  break;
			case CSTA_RECONNECT_CALL_CONF :
			  eventString += "Reconnection Call Confirmed";
			  break;
			case CSTA_RETRIEVE_CALL_CONF :
			  eventString += "Retrieve Call Confirmed";
			  break;
			case CSTA_SET_MWI_CONF :
			  eventString += "Set Msg Waiting Ind Confirmed";
			  break;
			case CSTA_SET_DND_CONF :
			  eventString += "Set DND Confirmed";
			  break;
			case CSTA_SET_FWD_CONF :
			  break;
			case CSTA_SET_AGENT_STATE_CONF :
			  eventString += "Set Agent State Confirmed";
			  break;
			case CSTA_TRANSFER_CALL_CONF :
			  eventString += "Transfer Call Confirmed";
			  break;
			case CSTA_UNIVERSAL_FAILURE_CONF :
			  eventString += "Universal Failure Confirmed";
			  break;
			case CSTA_ROUTE_REGISTER_REQ :
			  eventString += "Route Register Req";
			  break;
			case CSTA_ROUTE_REGISTER_REQ_CONF :
			  eventString += "Route Register Request Confirmed";
			  break;
			case CSTA_ROUTE_REGISTER_CANCEL :
			  eventString += "Route Register Cancel";
			  break;
			case CSTA_ROUTE_REGISTER_CANCEL_CONF :
			  eventString += "Route Register Cancel Confirmed";
			  break;
			case CSTA_ROUTE_REGISTER_ABORT :
			  eventString += "Route Register Abort";
			  break;
			case CSTA_ROUTE_REQUEST :
			  eventString += "Route Request";
			  break;
			case CSTA_ROUTE_SELECT_REQUEST :
			  eventString += "Route Select Request";
			  break;
			case CSTA_RE_ROUTE_REQUEST :
			  eventString += "Re Route Request";
			  break;
			case CSTA_ROUTE_USED :
			  eventString += "Route Used";
			  break;
			case CSTA_ROUTE_END :
			  eventString += "Route End";
			  break;
			case CSTA_ROUTE_END_REQUEST :
			  eventString += "Route End Request";
			  break;
			case CSTA_SEND_DTMF_TONES_CONF :
			  eventString += "Send DTMF TOnes Confirmed";
			  break;
			case CSTA_SINGLE_STEP_CONFERENCE_CALL_CONF :
			  eventString += "Single Step Conference Confirmed";
			  break;
			case CSTA_RECORD_MESSAGE_CALL_CONF :
			  eventString += "Record Message Call Confirmed";
			  break;
			case CSTA_STOP_CALL_CONF :
			  eventString += "Stop Call Confirmed";
			  break;
			case CSTA_ESCAPE_SVC_CONF :
			  eventString += "Escape SVC Confirmed";
			  break;
			case CSTA_IVR_STATUS_CONF:
			  eventString += "IVR Status Confirmed";
			  break;
			case CSTA_SYS_STAT_REQ_CONF :
			  eventString += "Sys Status Request Confirmed";
			  break;
			case CSTA_SYS_STAT_START_CONF :
			  eventString += "Sys Status Start Confirmed";
			  break;
			case CSTA_SYS_STAT_STOP_CONF :
			  eventString += "Sys STatus Stop Confirmed";
			  break;
			case CSTA_CHANGE_SYS_STAT_FILTER_CONF :
			  eventString += "Change Sys Status Filter Confirmed";
			  break;
			case CSTA_MONITOR_CONF :
			  eventString += "Monitor Confirmed";
			  break;
			case CSTA_CHANGE_MONITOR_FILTER_CONF :
			  eventString += "Change Monitor Filter Confirmed";
			  break;
			case CSTA_MONITOR_STOP_CONF :
			  eventString += "Monitor Stop Confirmed";
			  break;
			case CSTA_SNAPSHOT_CALL_CONF :
			  eventString += "Snapshot Call Confirmed";
			  break;
			case CSTA_SNAPSHOT_DEVICE_CONF :
			  eventString += "Snapshot Device Confirmed";
			  break;
			case CSTA_GETAPI_CAPS_CONF :
			  eventString += "GetAPI Caps Confirmed";
			  break;
			case CSTA_GET_DEVICE_LIST :
			  eventString += "Get Device List";
			  break;
			case CSTA_GET_DEVICE_LIST_CONF :
			  eventString += "Get Device List Confirmed";
			  break;
			case CSTA_QUERY_CALL_MONITOR_CONF :
			  eventString += "Query Call Monitor Confirmed";
			  break;
			default :
			  eventString += "Unknown Type";
			  break;
		  }
		  break;
		case CSTAEVENTREPORT :
		  eventString = "CSTA REPORT ";
		  switch (m_cstaEvent.eventHeader.eventType) 
		  {
			case CSTA_SYS_STAT :
			  eventString += "System Status";
			  break;
			case CSTA_SYS_STAT_ENDED :
			  eventString += "System Status Ended";
			  break;
			case CSTA_ROUTE_REGISTER_ABORT :
			  eventString += "Route Register Abort";
			  break;
			case CSTA_ROUTE_USED :
			  eventString += "Route Used";
			  break;
			case CSTA_ROUTE_END :
			  eventString += "Route End";
			  break;
			case CSTA_PRIVATE :
			  eventString += "Private";
			  break;
			default :
			  eventString += "Unknown Type";
			  break;
		  }
		  break;
		default :
			eventString = "Unknown Class";
		  break;
		}
		std::cout << (LPCSTR)eventString << "\n";
//TEMPORARY - END
		return (0);
	}
	
	/**
	  * getTelephoneByNumber
	  */
	ITelephoneCorbaDef_ptr TelephoneSwitch::getTelephoneByNumber(std::string number)
	{
		//for each telephone
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//compare the number
			if (!m_telephones[i]->getNumber().compare(number))
			{
				//this is the one
				//create a CORBA object for this telephone
				CORBA::Object* obj = TA_Core::CorbaUtil::getInstance().getPOA()->servant_to_reference(m_telephones[i]);
				//narrow and return the object as a TA_App::ITelephoneCorbaDef
				return TA_App::ITelephoneCorbaDef::_narrow(obj);
			}
		}
		//we didn't find a telephone with this number
		return NULL;
	}

	/**
	  * getAllTelephones
	  */
	ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneSwitch::getAllTelephones()
	{
		//prepare a return sequence to contain all the telephones
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* telephoneSequence = new ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence;
		//set the length of the sequence
		telephoneSequence->length(m_telephones.size());
		//for each telephone in this switch
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//create a CORBA object for this telephone
			CORBA::Object* obj = TA_Core::CorbaUtil::getInstance().getPOA()->servant_to_reference(m_telephones[i]);
			//narrow and add it to the sequence as a TA_App::ITelephoneCorbaDef
			(*telephoneSequence)[i] = TA_App::ITelephoneCorbaDef::_narrow(obj);
		}
		//return the sequence with all the telephones for this switch
		return telephoneSequence;
	}

	/**
	  * getTelephonesByLocation
	  */
	ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneSwitch::getTelephonesByLocation(unsigned long locationKey)
	{
		//prepare a return sequence to contain all the telephones for the specified location
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* telephoneSequence = new ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence;
		//initiate a count to keep count of the telephones to add
		int count(0);
		//go through the list of telephones
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//check the location key
			if (m_telephones[i]->getLocationKey() == locationKey)
			{
				//we've found one
				//reate a CORBA object for this telephone
				CORBA::Object* obj = TA_Core::CorbaUtil::getInstance().getPOA()->servant_to_reference(m_telephones[i]);
				//increment the length of the return sequence
				telephoneSequence->length(count + 1);
				//add the new object to the return sequence as a TA_App::ITelephoneCorbaDef
				(*telephoneSequence)[count] = TA_App::ITelephoneCorbaDef::_narrow(obj);
				//increment the count
				count++;
			}
		}
		//return the sequence of telephones for the specified location
		return telephoneSequence;
	}

	/**
	  * getTelephonesByName
	  */
	ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneSwitch::getTelephonesByName(std::string name)
	{
		//prepare a return sequence to contain all telephones that have this name as a substring
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* telephoneSequence = new ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence;
		//initiate the count to keep count of the added telephones
		int count(0);
		//go through the list of telephones
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//look for the substring
			if (m_telephones[i]->getName().find(name) != std::string::npos) // this is still case sensitive
			{
				//we've found one
				//create a CORBA object for this telephone
				CORBA::Object* obj = TA_Core::CorbaUtil::getInstance().getPOA()->servant_to_reference(m_telephones[i]);
				//incrment the length of the return sequence
				telephoneSequence->length(count + 1);
				//add the new object to the return sequence as a TA_App::ITelephoneCorbaDef
				(*telephoneSequence)[count] = TA_App::ITelephoneCorbaDef::_narrow(obj);
				//increment the count
				count++;
			}
		}
		//return the sequence
		return telephoneSequence;
	}

	/**
	  * hasThisTelephone
	  */
	bool TelephoneSwitch::hasThisTelephone(std::string number)
	{
		//go through the list of telephones
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//check the number
			if (!m_telephones[i]->getNumber().compare(number))
			{
				//we've found it, the telephone is part of this switch
				//return a true to indicate
				return true;
			}
		}
		//we didn't find a telephone with that number, return false
		return false;
	}

	/**
	  * servesThisLocation
	  */
	bool TelephoneSwitch::servesThisLocation(unsigned long locationKey)
	{
		//go through the list of telephones
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			// check the location key
			if (!m_telephones[i]->getLocationKey() == locationKey)
			{
				//we've found at least one telephone that is in the specified location
				//return true
				return true;
			}
		}
		//we haven't found any telephone for this location, return false
		return false;
	}

	/**
	  * getNumberOfTelephones
	  */
	int TelephoneSwitch::getNumberOfTelephones()
	{
		//return the total number of telephones for this switch
		return m_telephones.size();
	}

	/**
	  * hasTelephonesByName
	  */
	bool TelephoneSwitch::hasTelephonesByName(std::string name)
	{
		//go through the list of telephones
		for (unsigned int i(0); i < m_telephones.size(); i++)
		{
			//look for the substring
			if (m_telephones[i]->getName().find(name) != std::string::npos) // this is still case sensitive
			{
				// we've found at least one that has the name as substring
				//return true
				return true;
			}
		}
		//we didn't find any telephone with this name as a substring, return false
		return false;
	}

};
