#if !defined(TELEPHONE_H)
#define TELEPHONE_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/Telephone.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the 
  */
#include "app/telephone/telephone_agent/IDL/src/ITelephoneCorbaDef.h"
#include <string>
#include "acs.h"
#include "csta.h"
#include "core/corba/src/ServantBase.h"

namespace TA_App
{
	class Telephone : public virtual POA_TA_App::ITelephoneCorbaDef, public virtual TA_Core::ServantBase
	{
	public:
		/**
		  * constructor
		  * 
		  * The default constructor for a Telephone
		  * 
		  * @param		The handle to the TSAPI communication's stream
		  * @param		The name of the telephone
		  * @param		The telephone number
		  * @param		The maximum length of the history call stack
		  */
		Telephone(ACSHandle_t& handle, std::string name, std::string number, unsigned int maximumHistoryCallStackLength);
		/**
		  * destructor
		  * 
		  * This is the default destructor
		  */
		~Telephone();

		/**
		  * getTelephoneData
		  * 
		  * This method returns the telephone data as DirectoryEntryData
		  * 
		  * @return		TA_App::IDirectoryCorbaDef::DirectoryEntryData*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		TA_App::IDirectoryCorbaDef::DirectoryEntryData* getTelephoneData(const char* sessionId);
		/**
		  * getTelephoneCallStacks
		  * 
		  * This method returns the call stacks for this telephone
		  * 
		  * @return		TA_App::ITelephoneCorbaDef::TelephoneCallStackData*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		TA_App::ITelephoneCorbaDef::TelephoneCallStackData* getTelephoneCallStacks(const char* sessionId);
		/**
		  * answerCall
		  * 
		  * This method answres the alerting call for this telephone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The callId of the alerting call
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void answerCall		(const char* sessionId, unsigned long callId);
		/**
		  * holdCall
		  * 
		  * This method puts a call on hold for this telephone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The call ID to put on hold
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void holdCall		(const char* sessionId, unsigned long callId);
		/**
		  * makeCall
		  * 
		  * This method initiates a call to a specified number from this phone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The destination telephone or number
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void makeCall		(const char* sessionId, const TA_App::IDirectoryCorbaDef::DirectoryEntryData& destination);
		/**
		  * pickUpCall
		  * 
		  * This method answers a call from another telephone on this telephone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The call ID to pick up
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void pickUpCall		(const char* sessionId, unsigned long callId);
		/**
		  * endCall
		  * 
		  * This method ends the specified call on this telephone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The call ID to end
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void endCall		(const char* sessionId, unsigned long callId);
		/**
		  * transferCall
		  * 
		  * This method trasnfers a call that is on hold to a call that is active on this telephone
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The call ID that is on hold and needs to be transferred
		  * @param		The call ID that is active
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void transferCall	(const char* sessionId, unsigned long heldCallId, unsigned long activeCallId);
		/**
		  * conferenceCall
		  * 
		  * This method conferences two call, one held and one active
		  * 
		  * @return		nothing
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The call ID that is on hold and will be conferenced with this telephone
		  * @param		The call ID that is active and will be conferenced with this telephone
		  *
		  * @exception	TA_App::ITelephoneCorbaDef::TelephoneException
		  */
		void conferenceCall	(const char* sessionId, unsigned long heldCallId, unsigned long activeCallId);

		/**
		  * getNumber
		  * 
		  * This method returns the number for this telephones, for quick lookup
		  * 
		  * @return		std::string
		  */
		std::string		getNumber();
		/**
		  * getLocationKey
		  * 
		  * This method returns the location for this telephones, for quick lookup
		  * 
		  * @return		unsigned long
		  */
		unsigned long	getLocationKey();
		/**
		  * getName
		  * 
		  * This method returns the name for this telephones, for quick lookup
		  * 
		  * @return		std::string
		  */
		std::string		getName();

		/**
		  * addCall
		  * 
		  * This method adds a call to this telephone's call stacks
		  * 
		  * @return		nothing
		  * 
		  * @param		The call data for the new call
		  */
		void addCall	(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall);
		/**
		  * modifyCall
		  * 
		  * This method modifies a call in this telephone's call stacks
		  * 
		  * @return		nothing
		  * 
		  * @param		The call data for the modified call
		  */
		void modifyCall	(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall);
		/**
		  * deleteCall
		  * 
		  * This method removes a call from this telephone's call stacks
		  * 
		  * @return		nothing
		  * 
		  * @param		The call data for the deleted call
		  */
		void deleteCall	(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall);

	private:
		RetCode_t		m_retCode; //the return code for TSAPI methods
		ACSHandle_t&	m_acsHandle; // the reference to the handle to the TSAPI communication's stream
		TA_App::IDirectoryCorbaDef::DirectoryEntryData m_data; //the directory entry data, containing name, number etc.
		TA_App::ITelephoneCorbaDef::TelephoneCallStackData m_callStacks; // the call stacks for this telephone
		unsigned int m_historyCallStackMaximumLength; // the maximum length of the history call stack

		/**
		  * getConnectedCallId
		  * 
		  * This method returns the connected call ID for this phone
		  * 
		  * @return		the call ID for the connected call
		  */
		unsigned long getConnectedCallId();
		/**
		  * isCallInCallStack
		  * 
		  * This method verifies if this call ID is part of the call stack
		  * 
		  * @return		true when the call is in the call stack, false if no
		  * 
		  * @param		the call ID to find
		  */
		bool isCallInCallStack					(unsigned long callId);
		/**
		  * isCallInSwitchCallStack
		  * 
		  * This method verifies if this call ID is part of the switch call stack
		  * 
		  * @return		true when the call is in the switch call stack, false if no
		  * 
		  * @param		the call ID to find
		  */
		bool isCallInSwitchCallStack			(unsigned long callId);
		/**
		  * isCallInEmergencyCallStack
		  * 
		  * This method verifies if this call ID is part of the emergency call stack
		  * 
		  * @return		true when the call is in the emergency call stack, false if no
		  * 
		  * @param		the call ID to find
		  */
		bool isCallInEmergencyCallStack			(unsigned long callId);
		/**
		  * addCallToCallStack
		  * 
		  * This method adds the given call to the call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be added
		  */
		void addCallToCallStack					(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall);
		/**
		  * addCallToSwitchCallStack
		  * 
		  * This method adds the given call to the switch call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be added
		  */
		void addCallToSwitchCallStack			(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall);
		/**
		  * addCallToEmergencyCallStack
		  * 
		  * This method adds the given call to the emergency call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be added
		  */
		void addCallToEmergencyCallStack		(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall);
		/**
		  * addCallToHistoryCallStack
		  * 
		  * This method adds the given call to the history call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be added
		  */
		void addCallToHistoryCallStack			(TA_App::ITelephoneCorbaDef::TelephoneCallData& newCall);
		/**
		  * modifyCallInCallStack
		  * 
		  * This method modifies the given call in the call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be modified
		  */
		void modifyCallInCallStack				(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall);
		/**
		  * modifyCallInSwitchCallStack
		  * 
		  * This method modifies the given call in the switch call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be modified
		  */
		void modifyCallInSwitchCallStack		(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall);
		/**
		  * modifyCallInEmergencyCallStack
		  * 
		  * This method modifies the given call in the emergency call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be modified
		  */
		void modifyCallInEmergencyCallStack		(TA_App::ITelephoneCorbaDef::TelephoneCallData& modifiedCall);
		/**
		  * deleteCallFromCallStack
		  * 
		  * This method deletes the given call from the call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be deleted
		  */
		void deleteCallFromCallStack			(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall);
		/**
		  * deleteCallFromSwitchCallStack
		  * 
		  * This method deletes the given call from the switch call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be deleted
		  */
		void deleteCallFromSwitchCallStack		(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall);
		/**
		  * deleteCallFromEmergencyCallStack
		  * 
		  * This method deletes the given call from the emergency call stack
		  * 
		  * @return		nothing
		  * 
		  * @param		the call to be deleted
		  */
		void deleteCallFromEmergencyCallStack	(TA_App::ITelephoneCorbaDef::TelephoneCallData& deletedCall);
	};
}


#endif // !defined(TELEPHONE_H)
