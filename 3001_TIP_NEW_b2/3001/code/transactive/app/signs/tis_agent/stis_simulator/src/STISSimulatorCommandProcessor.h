#ifndef STIS_SIMULATOR_COMMAND_PROCESSOR_H
#define STIS_SIMULATOR_COMMAND_PROCESSOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISSimulatorCommandProcessor.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * STISSimulator implements ISimulationModule, for use with the generic simulator.
  */

#include "app/signs/tis_agent/stis_simulator/src/STISSimulator.h"
#include "bus/generic_server_simulator/src/ISimulationCommandProcessor.h"


namespace TA_IRS_App
{

	class STISSimulatorCommandProcessor : public TA_Base_Bus::ISimulationCommandProcessor
	{
	public:

	//Construction & Destruction
	//no default constructor, as this command processor should never be instantiated without
	//a parent simulator.
		STISSimulatorCommandProcessor( TA_IRS_App::STISSimulator* parentSimulator );
	virtual ~STISSimulatorCommandProcessor();


	//////////////////////////////////////////////////////////////////////////////
	//																			//
	//					ISimulationCommandProcessor methods						//
	//																			//
	//////////////////////////////////////////////////////////////////////////////

	/**
	  * hasCommandName
	  * 
	  * hasCommandName indicates whether or not the CommandProcessor has a command
	  * with the name specified in the name parameter.  Returns true if a command
	  * does exist with that name, false otherwise.
	  *
	  * @param std::string &name The name of the command to search for
	  *
	  * @return bool hasCommand True if the command exists, false otherwise.
	  *
	  */
	bool hasCommandName(const std::string &name);


	/**
	  * execute
	  * 
	  * performs the processing required to run the command specified in the name parameter.
	  * The params string is a space separated list of parameters to the command.
	  *
	  * @param std::string &name The name of the command to execute
	  *
	  * @param std::string &params Parameters to the command
	  *
	  * @return bool successful True if the command executes successfully, false otherwise.
	  *
	  */
	bool execute(const std::string &name, const std::string &params);


	/**
	  * errorMsg
	  * 
	  * returns the error message to be displayed if the command fails to execute successfully
	  *
	  * @return std::string errorMsg The error message
	  *
	  */
	std::string errorMsg();


	/**
	  * usageString
	  * 
	  * returns the usage string to be displayed if the command is called with incorrect parameters
	  *
	  * @return std::string usageString The usage string
	  *
	  */
	std::string usageString(const std::string &name);



	private:

	/**
	  * hasCommand
	  * 
	  * hasCommand is called by the hasCommandName method.  This method searches
	  * the list of methods specific to this class, not 
	  *
	  * @param std::string &name The name of the command to search for
	  *
	  * @return bool hasCommand True if the command exists, false otherwise.
	  *
	  */
	bool hasCommand(const std::string &name);

	/**
	  * cmdUsage
	  * 
	  * returns the usage string of the specified command.  This is only called by usageString()
	  *
	  * @return std::string usageString The usage string
	  *
	  */
	std::string cmdUsage(const std::string &name);



	TA_IRS_App::STISSimulator* m_parentSimulator;

	enum defs { NUM_COMMANDS = 15};					//the number of commands defined	//TD9161 
	static std::string commands[NUM_COMMANDS];		//this is the list of commands
	static std::string commandUsage[NUM_COMMANDS];	//and the associated list of usage messages.

	std::string m_errMsg;			//the error message variable is updated every time a command is executed.


	};

}//close namespace
#endif // STIS_SIMULATOR_COMMAND_PROCESSOR_H
