/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/Telephone.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the implementation for the Telephone class
  */

#include "Telephone.h"

namespace TA_App
{
	/**
	  * Telephone
	  */
	Telephone::Telephone(ACSHandle_t& handle, std::string name, std::string number, unsigned int maximumHistoryCallStackLength)
		: m_acsHandle(handle)
	{
		//set the name
		m_data.name = name.c_str();
		//set the number
		m_data.number = number.c_str();
		//make sure this device is monitored
		m_retCode = cstaMonitorDevice (m_acsHandle, 0, (DeviceID_t*)CORBA::string_dup(m_data.number), NULL, NULL); 
		//set the maximum length of the history call stacks
		m_historyCallStackMaximumLength = maximumHistoryCallStackLength;
		//activate this telephone to be a CORBA servant
        activateServant();
	}

	/**
	  * ~Telephone
	  */
	Telephone::~Telephone()
	{
		//deactivate this telephone as a CORBA servant
		deactivateServant();
	}

	/**
	  * getTelephoneData
	  */
	TA_App::IDirectoryCorbaDef::DirectoryEntryData* Telephone::getTelephoneData(const char* sessionId)
	{
		//return the requested data
		return &m_data;
	}

	/**
	  * getTelephoneCallStacks
	  */
	TA_App::ITelephoneCorbaDef::TelephoneCallStackData* Telephone::getTelephoneCallStacks(const char* sessionId)
	{
		//return the requested call stacks
		return &m_callStacks;
	}

	/**
	  * answerCall
	  */
	void Telephone::answerCall(const char* sessionId, unsigned long callId)
	{
		//create a connection struct
		ConnectionID_t callToAnswer;
		//set the call ID to the specified one
		callToAnswer.callID = callId;
		//copy the number in the connection struct
		strcpy (callToAnswer.deviceID, CORBA::string_dup(m_data.number));
		//answer the call using the TSAPI method
		m_retCode = cstaAnswerCall (m_acsHandle, (InvokeID_t)0, &callToAnswer, NULL);
	}

	/**
	  * holdCall
	  */
	void Telephone::holdCall(const char* sessionId, unsigned long callId)
	{
		//create a connection struct
		ConnectionID_t callToHold;
		//set the call ID to the specified one
		callToHold.callID = callId;
		//copy the number in the connection struct
		strcpy (callToHold.deviceID, CORBA::string_dup(m_data.number));
		//hold the call using the TSAPI method
		m_retCode = cstaHoldCall (m_acsHandle, (InvokeID_t)0, &callToHold, true, NULL);
	}

	/**
	  * makeCall
	  */
	void Telephone::makeCall(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& destination)
	{
		//make the call using the TSAPI method
		m_retCode = cstaMakeCall (m_acsHandle, (InvokeID_t)0, (DeviceID_t FAR *)CORBA::string_dup(m_data.number), (DeviceID_t FAR *)CORBA::string_dup(destination.number), NULL);
	}

	/**
	  * pickUpCall
	  */
	void Telephone::pickUpCall(const char* sessionId, unsigned long callId)
	{
		//create a connection struct
		ConnectionID_t callToPickUp;
		//set the call ID to the specified one
		callToPickUp.callID = callId;
		//copy the number in the connection struct
		strcpy (callToPickUp.deviceID, CORBA::string_dup(m_data.number));
		//pick up the call using the TSAPI method
		//TODO: This method is probably not supported by ALCATEL
		m_retCode = cstaPickupCall (m_acsHandle, 0, &callToPickUp, (DeviceID_t FAR *)CORBA::string_dup(m_data.number), NULL);
	}

	/**
	  * endCall
	  */
	void Telephone::endCall(const char* sessionId, unsigned long callId)
	{
		//create a connection struct
		ConnectionID_t callToEnd;
		//set the call ID to the specified one
		callToEnd.callID = callId;
		//copy the number in the connection struct
		strcpy (callToEnd.deviceID, CORBA::string_dup(m_data.number));
		m_retCode = cstaClearCall (m_acsHandle, (InvokeID_t)0, &callToEnd, NULL);
	}

	/**
	  * transferCall
	  */
	void Telephone::transferCall(const char* sessionId, unsigned long heldCallId, unsigned long activeCallId)
	{
		//create a connection struct
		ConnectionID_t heldCall;
		//set the call ID to the specified one
		heldCall.callID = heldCallId;
		//copy the number in the connection struct
		strcpy (heldCall.deviceID, CORBA::string_dup(m_data.number));
		//create a connection struct
		ConnectionID_t activeCall;
		//set the call ID to the specified one
		activeCall.callID = activeCallId;
		//copy the number in the connection struct
		strcpy (activeCall.deviceID, CORBA::string_dup(m_data.number));
		//transfer the call
		m_retCode = cstaTransferCall (m_acsHandle, (InvokeID_t)0, (ConnectionID_t FAR *)&heldCall, (ConnectionID_t FAR *)&activeCall, NULL);
	}

	/**
	  * conferenceCall
	  */
	void Telephone::conferenceCall(const char* sessionId, unsigned long heldCallId, unsigned long activeCallId)
	{
		//create a connection struct
		ConnectionID_t heldCall;
		//set the call ID to the specified one
		heldCall.callID = heldCallId;
		//copy the number in the connection struct
		strcpy (heldCall.deviceID, CORBA::string_dup(m_data.number));
		//create a connection struct
		ConnectionID_t activeCall;
		//set the call ID to the specified one
		activeCall.callID = activeCallId;
		//copy the number in the connection struct
		strcpy (activeCall.deviceID, CORBA::string_dup(m_data.number));
		//conference both calls
		m_retCode = cstaConferenceCall (m_acsHandle, (InvokeID_t)0, (ConnectionID_t FAR *)&heldCall, (ConnectionID_t FAR *)&activeCall, NULL);
	}

	/**
	  * getConnectedCallId
	  */
	unsigned long Telephone::getConnectedCallId()
	{
		//go through the call stack to find a connected call
		for (unsigned int i(0); i < m_callStacks.callStack.length(); i++)
		{
			//check the call status
			if (m_callStacks.callStack[i].status == TA_App::ITelephoneCorbaDef::Connected)
			{
				//return this callId because this one is connected
				return m_callStacks.callStack[i].callId;
			}
		}
		//return 0 because we didn't find a connected one
		return 0;
	}

	/**
	  * getNumber
	  */
	std::string Telephone::getNumber()
	{
		//create a string to return
		std::string returnNumber(CORBA::string_dup(m_data.number));
		//return the number
		return returnNumber;
	}

	/**
	  * getLocationKey
	  */
	unsigned long Telephone::getLocationKey()
	{
		//return the location key
		return m_data.location;
	}

	/**
	  * getName
	  */
	std::string Telephone::getName()
	{
		//create a return string
		std::string returnName(CORBA::string_dup(m_data.name));
		//return the name
		return returnName;
	}

	/**
	  * addCall
	  */
	void Telephone::addCall(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall)
	{
		//check if this is an emergency call or not
		if (newCall.type != TA_App::ITelephoneCorbaDef::EmergencyCall)
		{
			//it is not an emergency call
			//find out where it belongs
			if (!strcmp(newCall.origin.number,m_data.number) || !strcmp(newCall.destination.number,m_data.number))
			{
				//it's a local one
				addCallToCallStack(newCall);
				//send an update event
				//TODO
				return;
			}
			else
			{
				//it's a switch one
				addCallToSwitchCallStack(newCall);
				//send an update event
				//TODO
				return;
			}
		}
		else
		{
			//it's an emergency call
			//add to the emergency stack
			addCallToEmergencyCallStack(newCall);
			//send an update event
			//TODO
			return;
		}
	}

	/**
	  * modifyCall
	  */
	void Telephone::modifyCall(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall)
	{
		//check if this is an emergency call or not
		if (modifiedCall.type != TA_App::ITelephoneCorbaDef::EmergencyCall)
		{
			//it is not an emergency call
			//find the call and update its properties
			if (isCallInCallStack(modifiedCall.callId))
			{
				//update the call stack
				modifyCallInCallStack(modifiedCall);
				//send an update event
				//TODO
				return;
			}
			else if (isCallInSwitchCallStack(modifiedCall.callId))
			{
				//update the switch stack
				modifyCallInSwitchCallStack(modifiedCall);
				//send an update event
				//TODO
				return;
			}
			else
			{
				//this call does not exist, add it
				addCall(modifiedCall);
				return;
			}
		}
		else
		{
			//it's an emergency call
			//update the emergency stack
			if (isCallInEmergencyCallStack(modifiedCall.callId))
			{
				modifyCallInEmergencyCallStack(modifiedCall);
				//send an update event
				//TODO
				return;
			}
			else
			{
				//this emergency call does not exist, add it
				addCall(modifiedCall);
				return;
			}
		}
	}

	/**
	  * deleteCall
	  */
	void Telephone::deleteCall(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall)
	{
		//remove from any stack, if it was from this telephone, add to history stack
		//check if this is an emergency call or not
		if (deletedCall.type != TA_App::ITelephoneCorbaDef::EmergencyCall)
		{
			//it is not an emergency call
			//find the call and update its properties
			if (isCallInCallStack(deletedCall.callId))
			{
				//delete from the call stack
				deleteCallFromCallStack(deletedCall);
				//add to the history stack
				addCallToHistoryCallStack(deletedCall);
				//send an update event
				//TODO
				return;
			}
			else if (isCallInSwitchCallStack(deletedCall.callId))
			{
				//delete from the switch stack
				deleteCallFromSwitchCallStack(deletedCall);
				//send an update event
				//TODO
				return;
			}
		}
		else
		{
			//it's an emergency call
			//remove from the emergency stack
			deleteCallFromEmergencyCallStack(deletedCall);
			//send an update event
			//TODO
			return;
		}
	}

	/**
	  * isCallInCallStack
	  */
	bool Telephone::isCallInCallStack(unsigned long callId)
	{
		//run through the call stack
		for (unsigned int i(0); i < m_callStacks.callStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.callStack[i].callId == callId)
			{
				//we've found it, it's part of the call stack
				return true;
			}
		}
		//we didn't find it, it's not part of the call stack
		return false;
	}

	/**
	  * isCallInSwitchCallStack
	  */
	bool Telephone::isCallInSwitchCallStack(unsigned long callId)
	{
		//run through the switch call stack
		for (unsigned int i(0); i < m_callStacks.switchCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.switchCallStack[i].callId == callId)
			{
				//we've found it, it's part of the switch call stack
				return true;
			}
		}
		//we didn't find it, it's not part of the switch call stack
		return false;
	}

	/**
	  * isCallInEmergencyCallStack
	  */
	bool Telephone::isCallInEmergencyCallStack(unsigned long callId)
	{
		//run through the emergency call stack
		for (unsigned int i(0); i < m_callStacks.emergencyCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.emergencyCallStack[i].callId == callId)
			{
				//we've found it, it's part of the emergency call stack
				return true;
			}
		}
		//we didn't find it, it's not part of the emergency call stack
		return false;
	}

	/**
	  * addCallToCallStack
	  */
	void Telephone::addCallToCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall)
	{
		//get the length of the call stack as it is currently
		unsigned int length(m_callStacks.callStack.length());
		//increase the length by 1 
		m_callStacks.callStack.length(length+1);
		//set each member of the new call on the call stack
		m_callStacks.callStack[length].callId			= newCall.callId;
		m_callStacks.callStack[length].connectedTime	= newCall.connectedTime;
		m_callStacks.callStack[length].createTime		= newCall.createTime;
		m_callStacks.callStack[length].endedTime		= newCall.endedTime;
		m_callStacks.callStack[length].status			= newCall.status;
		m_callStacks.callStack[length].type				= newCall.type;
		m_callStacks.callStack[length].origin			= newCall.origin;
		m_callStacks.callStack[length].destination		= newCall.destination;
		return;
	}

	/**
	  * addCallToSwitchCallStack
	  */
	void Telephone::addCallToSwitchCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall)
	{
		//get the length of the switch call stack as it is currently
		unsigned int length(m_callStacks.switchCallStack.length());
		//increase the length by 1 
		m_callStacks.switchCallStack.length(length+1);
		//set each member of the new call on the switch call stack
		m_callStacks.switchCallStack[length].callId			= newCall.callId;
		m_callStacks.switchCallStack[length].connectedTime	= newCall.connectedTime;
		m_callStacks.switchCallStack[length].createTime		= newCall.createTime;
		m_callStacks.switchCallStack[length].endedTime		= newCall.endedTime;
		m_callStacks.switchCallStack[length].status			= newCall.status;
		m_callStacks.switchCallStack[length].type			= newCall.type;
		m_callStacks.switchCallStack[length].origin			= newCall.origin;
		m_callStacks.switchCallStack[length].destination	= newCall.destination;
		return;
	}

	/**
	  * addCallToEmergencyCallStack
	  */
	void Telephone::addCallToEmergencyCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall)
	{
		//get the length of the emergency call stack as it is currently
		unsigned int length(m_callStacks.emergencyCallStack.length());
		//increase the length by 1 
		m_callStacks.emergencyCallStack.length(length+1);
		//set each member of the new call on the emergency call stack
		m_callStacks.emergencyCallStack[length].callId			= newCall.callId;
		m_callStacks.emergencyCallStack[length].connectedTime	= newCall.connectedTime;
		m_callStacks.emergencyCallStack[length].createTime		= newCall.createTime;
		m_callStacks.emergencyCallStack[length].endedTime		= newCall.endedTime;
		m_callStacks.emergencyCallStack[length].status			= newCall.status;
		m_callStacks.emergencyCallStack[length].type			= newCall.type;
		m_callStacks.emergencyCallStack[length].origin			= newCall.origin;
		m_callStacks.emergencyCallStack[length].destination		= newCall.destination;
		return;
	}

	/**
	  * addCallToHistoryCallStack
	  */
	void Telephone::addCallToHistoryCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall)
	{
		//verify the length of the history call stack against the maximum allowed
		if (m_callStacks.historyCallStack.length() == m_historyCallStackMaximumLength)
		{
			// the history call stack is already of length m_historyCallStackMaximumLength
			//remove the first one
			for (unsigned int i(0); i < m_callStacks.historyCallStack.length()-1; i++)
			{
				//shift all the others one forward (FIFO)
				m_callStacks.historyCallStack[i] = m_callStacks.historyCallStack[i+1];
			}
			//make the length one less, this cuts of the last element
			m_callStacks.historyCallStack.length(m_callStacks.historyCallStack.length()-1);
		}
		//now add it
		//get the length of the history call stack as it is currently
		unsigned int length(m_callStacks.historyCallStack.length());
		//increase the length by 1 
		m_callStacks.historyCallStack.length(length+1);
		//set each member of the new call on the history call stack
		m_callStacks.historyCallStack[length].callId		= newCall.callId;
		m_callStacks.historyCallStack[length].connectedTime	= newCall.connectedTime;
		m_callStacks.historyCallStack[length].createTime	= newCall.createTime;
		m_callStacks.historyCallStack[length].endedTime		= newCall.endedTime;
		m_callStacks.historyCallStack[length].status		= newCall.status;
		m_callStacks.historyCallStack[length].type			= newCall.type;
		m_callStacks.historyCallStack[length].origin		= newCall.origin;
		m_callStacks.historyCallStack[length].destination	= newCall.destination;
		return;
	}

	/**
	  * modifyCallInCallStack
	  */
	void Telephone::modifyCallInCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall)
	{
		//run through the call stack
		for (unsigned int i(0); i < m_callStacks.callStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.callStack[i].callId == modifiedCall.callId)
			{
				//found it, now update it
				m_callStacks.callStack[i].connectedTime = modifiedCall.connectedTime;
				m_callStacks.callStack[i].createTime	= modifiedCall.createTime;
				m_callStacks.callStack[i].endedTime		= modifiedCall.endedTime;
				m_callStacks.callStack[i].status		= modifiedCall.status;
				m_callStacks.callStack[i].type			= modifiedCall.type;
				m_callStacks.callStack[i].origin		= modifiedCall.origin;
				m_callStacks.callStack[i].destination	= modifiedCall.destination;
				return;
			}
		}
	}

	/**
	  * modifyCallInSwitchCallStack
	  */
	void Telephone::modifyCallInSwitchCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall)
	{
		//run through the switch call stack
		for (unsigned int i(0); i < m_callStacks.switchCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.switchCallStack[i].callId == modifiedCall.callId)
			{
				//found it, now update it
				m_callStacks.switchCallStack[i].connectedTime	= modifiedCall.connectedTime;
				m_callStacks.switchCallStack[i].createTime		= modifiedCall.createTime;
				m_callStacks.switchCallStack[i].endedTime		= modifiedCall.endedTime;
				m_callStacks.switchCallStack[i].status			= modifiedCall.status;
				m_callStacks.switchCallStack[i].type			= modifiedCall.type;
				m_callStacks.switchCallStack[i].origin			= modifiedCall.origin;
				m_callStacks.switchCallStack[i].destination		= modifiedCall.destination;
				return;
			}
		}
	}

	/**
	  * modifyCallInEmergencyCallStack
	  */
	void Telephone::modifyCallInEmergencyCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall)
	{
		//run through the emergency call stack
		for (unsigned int i(0); i < m_callStacks.emergencyCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.emergencyCallStack[i].callId == modifiedCall.callId)
			{
				//found it, now update it
				m_callStacks.emergencyCallStack[i].connectedTime	= modifiedCall.connectedTime;
				m_callStacks.emergencyCallStack[i].createTime		= modifiedCall.createTime;
				m_callStacks.emergencyCallStack[i].endedTime		= modifiedCall.endedTime;
				m_callStacks.emergencyCallStack[i].status			= modifiedCall.status;
				m_callStacks.emergencyCallStack[i].type				= modifiedCall.type;
				m_callStacks.emergencyCallStack[i].origin			= modifiedCall.origin;
				m_callStacks.emergencyCallStack[i].destination		= modifiedCall.destination;
				return;
			}
		}
	}

	/**
	  * deleteCallFromCallStack
	  */
	void Telephone::deleteCallFromCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall)
	{
		//run through the call stack
		for (unsigned int i(0); i < m_callStacks.callStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.callStack[i].callId == deletedCall.callId)
			{
				//we've found it, now remove it
				for (; i < m_callStacks.callStack.length()-1; i++)
				{
					//shift all the following calls one back
					m_callStacks.callStack[i] = m_callStacks.callStack[i+1];
				}
				//make the length one less, this removes the last one
				m_callStacks.callStack.length(m_callStacks.callStack.length()-1);
				//exit
				return;
			}
		}
		//we should never get here
		return;
	}

	/**
	  * deleteCallFromSwitchCallStack
	  */
	void Telephone::deleteCallFromSwitchCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall)
	{
		//run through the switch call stack
		for (unsigned int i(0); i < m_callStacks.switchCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.switchCallStack[i].callId == deletedCall.callId)
			{
				//we've found it, now remove it
				for (; i < m_callStacks.switchCallStack.length()-1; i++)
				{
					//shift all the following calls one back
					m_callStacks.switchCallStack[i] = m_callStacks.switchCallStack[i+1];
				}
				//make the length one less, this removes the last one
				m_callStacks.switchCallStack.length(m_callStacks.switchCallStack.length()-1);
				//exit
				return;
			}
		}
		//we should never get here
		return;
	}

	/**
	  * deleteCallFromEmergencyCallStack
	  */
	void Telephone::deleteCallFromEmergencyCallStack(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall)
	{
		//run through the emergency call stack
		for (unsigned int i(0); i < m_callStacks.emergencyCallStack.length(); i++)
		{
			//verify the call ID
			if (m_callStacks.emergencyCallStack[i].callId == deletedCall.callId)
			{
				//we've found it, now remove it
				for (; i < m_callStacks.emergencyCallStack.length()-1; i++)
				{
					//shift all the following calls one back
					m_callStacks.emergencyCallStack[i] = m_callStacks.emergencyCallStack[i+1];
				}
				//make the length one less, this removes the last one
				m_callStacks.emergencyCallStack.length(m_callStacks.emergencyCallStack.length()-1);
				//exit
				return;
			}
		}
		//we should never get here
		return;
	}

};
