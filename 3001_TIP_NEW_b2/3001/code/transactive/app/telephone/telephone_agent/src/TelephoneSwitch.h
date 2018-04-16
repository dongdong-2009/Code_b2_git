#if !defined(TELEPHONESWITCH_H)
#define TELEPHONESWITCH_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_agent/src/TelephoneSwitch.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the 
  */

#include "stdafx.h"
#include "acs.h"
#include "csta.h"

#include "Telephone.h"
#include "TelephoneAgent.h"
#include "resource.h"
#include <vector>

#include "TSAPINotificationManager.h"

namespace TA_App
{
	class TelephoneSwitch
	{
	public:
		/**
		  * TelephoneSwitch
		  * 
		  * This is the default constructor for the telephone switch 
		  * 
		  * @return		nothing
		  */
		TelephoneSwitch(std::string switchName);
		/**
		  * ~TelephoneSwitch
		  * 
		  * This is the default destructor for the telephone switch
		  * 
		  * @return		nothing
		  */
		~TelephoneSwitch();
		/**
		  * getEventFromSwitchBlocking
		  * 
		  * This method gets an event from the top of the queue from the TSAPI server, it is a blocking request
		  * 
		  * @return		int
		  */
		int getEventFromSwitchBlocking();
		/**
		  * getSnapShotOfSwitch
		  * 
		  * This method will get a snapshot of each telephone on this switch, for initialisation
		  * 
		  * @return		nothing
		  */
		void getSnapShotOfSwitch();
		/**
		  * hasThisTelephone
		  * 
		  * This method verifies if this switch is servicing the given number
		  * 
		  * @return		bool, true if it has, false if not
		  * 
		  * @param		The telephone number to look for
		  */
		bool hasThisTelephone(std::string number);
		/**
		  * servesThisLocation
		  * 
		  * This method verifies if this switch has any telephones that are in the given location
		  * 
		  * @return		bool, true if it has, false if not
		  * 
		  * @param		the location key
		  */
		bool servesThisLocation(unsigned long locationKey);
		/**
		  * hasTelephonesByName
		  * 
		  * This method verifies if this switch has any telephones with the given name as a substring
		  * 
		  * @return		bool, true if it has, false if not
		  * 
		  * @param		the name to look for
		  */
		bool hasTelephonesByName(std::string name);
		/**
		  * getNumberOfTelephones
		  * 
		  * This method returns the total number of telephones in this switch
		  * 
		  * @return		int, the total number of telephones on this switch
		  */
		int getNumberOfTelephones();

		/**
		  * getTelephoneByNumber
		  * 
		  * This method returns the telephone object for the given number
		  * 
		  * @return		ITelephoneCorbaDef_ptr
		  * 
		  * @param		The number to return a telephone for
		  */
		ITelephoneCorbaDef_ptr getTelephoneByNumber(std::string number);
		/**
		  * getAllTelephones
		  * 
		  * This method returns all telephone objects for this switch
		  * 
		  * @return		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  */
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getAllTelephones();
		/**
		  * getTelephonesByLocation
		  * 
		  * This method returns all telephone objects within the given location
		  * 
		  * @return		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  * 
		  * @param		The location to return telephone objects for
		  */
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getTelephonesByLocation(unsigned long locationKey);
		/**
		  * getTelephonesByName
		  * 
		  * This method returns all telephones on this switch that have this name as a substring
		  * 
		  * @return		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  * 
		  * @param		the name to return telephones for
		  */
		ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getTelephonesByName(std::string name);

	private:
		RetCode_t		m_retCode; // the return code from all acs methods
		ACSHandle_t		m_acsHandle; // the handle to the TSAPI server's communication stream
		ServerID_t*		m_serverID; // the server ID
		LoginID_t*		m_loginID; // the login to use to open the TSAPI stream
		Passwd_t		m_password; // the password to use to open the TSAPI stream
		Level_t			m_ACSLevel; // the level of the ACS
		CSTAEvent_t		m_cstaEvent; // The TSAPI event to handle

		std::vector<Telephone*>				m_telephones; // the vector of telephones that belong to this switch
		TA_App::TSAPINotificationManager*	m_notificationManager; // the TSAPI notification manager, this one will be responsible for receiving the events from the TSAPI server
	};
}


#endif // !defined(TELEPHONESWITCH_H)
