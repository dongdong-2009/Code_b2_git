#if !defined(TELEPHONEAGENT_H)
#define TELEPHONEAGENT_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_agent/src/TelephoneAgent.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 
  */

#include "Telephone.h"
#include "TelephoneDirectory.h"

#include "app/telephone/telephone_agent/IDL/src/ITelephoneAgentCorbaDef.h"
#include <vector>

namespace TA_App
{
    class Telephone; // Forward Declaration
	class TelephoneSwitch;

	class TelephoneAgent : public virtual POA_TA_App::ITelephoneAgentCorbaDef, public virtual TA_Core::ServantBase
	{
	public:
		/**
		  * TelephoneAgent
		  * 
		  * This is the default constuctor for the telephone agent
		  * 
		  * @return		nothing
		  */
		TelephoneAgent(int argc, char* argv[]);
		/**
		  * ~TelephoneAgent
		  * 
		  * This is the defalt destructor 
		  * 
		  * @return		nothing
		  */
		~TelephoneAgent();

		/**
		  * getTelephoneFromSessionId
		  * 
		  * This method returns the telephone that is connected to this sessionID/console
		  * 
		  * @return		ITelephoneCorbaDef_ptr
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		ITelephoneCorbaDef_ptr	getTelephoneFromSessionId(const char* sessionId);
		/**
		  * getTelephoneByNumber
		  * 
		  * This method returns the telephone object that has this number
		  * 
		  * @return		ITelephoneCorbaDef_ptr
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The number for which to return the telephone object
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		ITelephoneCorbaDef_ptr	getTelephoneByNumber(const char* sessionId, const char* number);
		/**
		  * getTelephoneFromDirectoryEntry
		  * 
		  * This method returns the telephone object for the specified DirectoryEntry
		  * 
		  * @return		ITelephoneCorbaDef_ptr
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The directory Entry data for which to return the telephone object
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		ITelephoneCorbaDef_ptr	getTelephoneFromDirectoryEntry(const char* sessionId, const IDirectoryCorbaDef::DirectoryEntryData& directoryEntry);
		/**
		  * ITelephoneCorbaDefSequence
		  * 
		  * This method returns all telephones for this agent
		  * 
		  * @return		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getAllTelephones(const char* sessionId);
		/**
		  * getTelephonesForLocation
		  * 
		  * This method returns all telephones that are in the specified location for this agent
		  * 
		  * @return		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The location to look for
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getTelephonesForLocation(const char* sessionId, CORBA::ULong locationKey);
		/**
		  * getTelephonesByName
		  * 
		  * This method returns all telephones that have this name as a substring
		  * 
		  * @return		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence*
		  * 
		  * @param		The sessionID used for security verification
		  * @param		The name to look for
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* getTelephonesByName(const char* sessionId, const char* name);
		/**
		  * getAllTelephoneDirectories
		  * 
		  * This method returns all telephone directories in this agent
		  * 
		  * @return		TA_App::ITelephoneAgentCorbaDef::IDirectoryCorbaDefSequence*
		  * 
		  * @param		The sessionID used for security verification
		  *
		  * @exception	TA_App::ITelephoneAgentCorbaDef::TelephoneAgentException
		  */
		TA_App::ITelephoneAgentCorbaDef::IDirectoryCorbaDefSequence* getAllTelephoneDirectories(const char* sessionId);
	private:
		std::vector<TelephoneSwitch*>	m_switches; // the vector of switches
		std::vector<TelephoneDirectory*> m_directories; // the vector of directories

		/**
		  * getSwitchesFromDatabase
		  * 
		  * This method gets all switches from the database that belong to this telephone agent
		  * 
		  * @return		nothing
		  * 
		  * @param		The name of this telephone agent
		  */
		void getSwitchesFromDatabase(std::string telephoneAgentName);
		/**
		  * getDirectoriesFromDatabase
		  * 
		  * This method gets all the directories that belong to this telephone agent
		  * 
		  * @return		nothing
		  * 
		  * @param		The name of this telephone agent
		  */
		void getDirectoriesFromDatabase(std::string telephoneAgentName);
	};
}

#endif // !defined(TELEPHONEAGENT_H)
