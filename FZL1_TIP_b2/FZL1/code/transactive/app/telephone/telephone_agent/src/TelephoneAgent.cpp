/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_agent/src/TelephoneAgent.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the implementation for the TelephoneAgent class
  */

#include "stdafx.h"


#include "TelephoneAgent.h"
#include "TelephoneSwitch.h"
#include "core\utilities\src\RunParams.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"

namespace TA_App
{
	/**
	  * TelephoneAgent
	  */
	TelephoneAgent::TelephoneAgent(int argc, char* argv[])
	{
		//initiate some CORBA stuff
		TA_Core::CorbaUtil::getInstance().initialise();
		TA_Core::CorbaUtil::getInstance().activate();
		//set the run params
		TA_Core::RunParams::getInstance().parseCmdLine(argc, argv);

		//load the configuration data for this agent
		if (TA_Core::RunParams::getInstance().isSet("EntityName"))
		{
			//activate the agent as a CORBA servant 
			activateServantWithName(TA_Core::RunParams::getInstance().get("EntityName"));
			//get the switches from the database
			getSwitchesFromDatabase(TA_Core::RunParams::getInstance().get("EntityName"));
			//get the directories for this agent from the database
			getDirectoriesFromDatabase(TA_Core::RunParams::getInstance().get("EntityName"));
		}
	}

	/**
	  * ~TelephoneAgent
	  */
	TelephoneAgent::~TelephoneAgent()
	{
		//clean up
		for (unsigned int i(0); i < m_switches.size(); i++)
		{
			//delete all the switches
			delete m_switches[i];
		}
		//deactivate the CORBA servant
		deactivateServant();
	}

	/**
	  * getTelephoneFromSessionId
	  */
	ITelephoneCorbaDef_ptr	TelephoneAgent::getTelephoneFromSessionId(const char* sessionId)
	{
		//get the telephone from the sessionID through the console
		//TODO
		return NULL;
	}

	/**
	  * getTelephoneByNumber
	  */
	ITelephoneCorbaDef_ptr	TelephoneAgent::getTelephoneByNumber(const char* sessionId, const char* number)
	{
		//check each switch until we find the telephone
		for (unsigned int i(0); i < m_switches.size(); i++)
		{
			//verify if this switch has this telephone
			if (m_switches[i]->hasThisTelephone(number))
			{
				//it has, so return it
				return m_switches[i]->getTelephoneByNumber(number);
			}
		}
		//we didn't find it
		return NULL;
	}

	/**
	  * getTelephoneFromDirectoryEntry
	  */
	ITelephoneCorbaDef_ptr	TelephoneAgent::getTelephoneFromDirectoryEntry(const char* sessionId, const IDirectoryCorbaDef::DirectoryEntryData& directoryEntry)
	{
		//return the telephone for this directory entry's number
		return getTelephoneByNumber(sessionId,directoryEntry.number);
	}

	/**
	  * getAllTelephones
	  */
	TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneAgent::getAllTelephones(const char* sessionId)
	{
		//prepare a sequence to be returned with all the telephones from all the switches
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* sequence = new TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence;
		//create a temporary sequence for all the telephones from each switch
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* tempSequence;
		//the total number of telephones from all switches
		int totalNumberOfTelephones(0);
		//get the total number of telephones from all the switches together
		for (unsigned int i(0); i < m_switches.size(); i++)
		{
			//add the number of telephones to the total
			totalNumberOfTelephones += m_switches[i]->getNumberOfTelephones();
		}
		//specify the length of the return sequence
		sequence->length(totalNumberOfTelephones);
		//initiate the count, this will keep count of the telephone to be added to the return sequence
		int count(0);
		//go through each switch
		for (i = 0; i < m_switches.size(), count < totalNumberOfTelephones; i++)
		{
			//get all the telephones from this switch
			tempSequence = m_switches[i]->getAllTelephones();
			//go through the list of telephones from this switch
			for (unsigned int j(0); j < tempSequence->length(), count < totalNumberOfTelephones; j++)
			{
				//copy the telephone into the return sequence, using 'count'
				(*sequence)[count] = (*tempSequence)[j];
				//increment 'count'
				count++;
			}
		}
		//return the sequence with all telephones from all switches in this agent
		return sequence;
	}

	/**
	  * getTelephonesForLocation
	  */
	TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneAgent::getTelephonesForLocation(const char* sessionId, CORBA::ULong locationKey)
	{
		//for each switch
		for (unsigned int i(0); i < m_switches.size(); i++)
		{
			//find out if this switch serves this location
			if (m_switches[i]->servesThisLocation(locationKey))
			{
				//if it does, return it's list of telephones for the specified location
				return m_switches[i]->getTelephonesByLocation(locationKey); // it is assumed here that a location will not be shared by more than one switch
			}
		}
		// no telephones found
		return NULL;
	}

	/**
	  * getTelephonesByName
	  */
	TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* TelephoneAgent::getTelephonesByName(const char* sessionId, const char* name)
	{
		//prepare a sequence to be returned with all the telephones from all the switches that have this name as a substring
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* sequence = new TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence;
		//create a temporary sequence for all the telephones that have this name as a substring from each switch
		TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* tempSequence;
		//initiate the count, this will keep count of the telephone to be added to the return sequence
		unsigned int count(0);
		//go through each switch
		for (unsigned int i(0); i < m_switches.size(); i++)
		{
			//check if this switch has any telephones by this name
			if (m_switches[i]->hasTelephonesByName(name))
			{
				//it has
				//get all the telephones with this name from this switch
				tempSequence = m_switches[i]->getTelephonesByName(name);
				//change the length of the return sequence to add the telephones to the return sequence
				sequence->length(sequence->length() + tempSequence->length());
				//go through all the telephones that were returned
				for (unsigned int j(0); j < tempSequence->length(), count < sequence->length(); j++)
				{
					//copy each telephone into the return sequence
					(*sequence)[count] = (*tempSequence)[j];
					//increment the count
					count++;
				}
			}
		}
		//return the sequence with all the telephones by the given name from all switches in this agent
		return sequence;
	}

	/**
	  * getAllTelephoneDirectories
	  */
	TA_App::ITelephoneAgentCorbaDef::IDirectoryCorbaDefSequence* TelephoneAgent::getAllTelephoneDirectories(const char* sessionId)
	{
		//prepare a return sequence that contains all directories within this agent
		TA_App::ITelephoneAgentCorbaDef::IDirectoryCorbaDefSequence* sequence = new TA_App::ITelephoneAgentCorbaDef::IDirectoryCorbaDefSequence;
		//set the length to the number of directories
		sequence->length(m_directories.size());
		//for each directory
		for (unsigned int i(0); i < m_directories.size(); i++)
		{
			//create a CORBA object of each directory
			CORBA::Object* obj = TA_Core::CorbaUtil::getInstance().getPOA()->servant_to_reference(m_directories[i]);
			//narrow each object into a TA_App::IDirectoryCorbaDef object
			(*sequence)[i] = TA_App::IDirectoryCorbaDef::_narrow(obj);
		}
		//return the sequence with all the directories in it
		return sequence;
	}

	/**
	  * getSwitchesFromDatabase
	  */
	void TelephoneAgent::getSwitchesFromDatabase(std::string telephoneAgentName)
	{
		//get all the telephone switches for this agent from the database
		//TODO
		std::vector<TA_Core::IEntityData*> switches = TA_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(telephoneAgentName);
		for (unsigned int i(0); i < switches.size(); i++)
		{
			TelephoneSwitch* telephoneSwitch = new TelephoneSwitch(switches[i]->getName());
			m_switches.push_back (telephoneSwitch);
//			telephoneSwitch->getSnapShotOfSwitch();
		}
	}

	/**
	  * getDirectoriesFromDatabase
	  */
	void TelephoneAgent::getDirectoriesFromDatabase(std::string telephoneAgentName)
	{
		//get all the telephone directories for this agent from the database
		//TODO
//TEMPORARY
		TelephoneDirectory* telephoneDirectory = new TelephoneDirectory("default",TA_App::IDirectoryCorbaDef::InternalDirectory);
		m_directories.push_back(telephoneDirectory);
//TEMPORARY - END
	}
};