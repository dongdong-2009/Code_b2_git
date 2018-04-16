/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/stis_simulator/src/STISSimulatorCommandProcessor.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * STISSimulator implements ISimulationModule, for use with the generic simulator.
  */

#include <iostream>
#include "app/signs/tis_agent/stis_simulator/src/STISSimulatorCommandProcessor.h"

namespace TA_IRS_App
{
	//TD9161 add two commands
	std::string STISSimulatorCommandProcessor::commands[NUM_COMMANDS] =
	{
        "sendPacket",
		"status",
        "updatepid",
		"resourcestatus",
		"updateresource",
		"newstislibrary",
		"newttislibrary",
		"currentstislibrary",
        "sendratistostation",
        "sendratistotrain",
        "sendratistoallstations",
        "sendratistoalltrains",
		"ratisresponse",
		"displayrequestresponse"
 	};

	//TD9161 add two commands
	std::string STISSimulatorCommandProcessor::commandUsage[NUM_COMMANDS] =
	{
        "sendPacket",
		"status",
        "updatepid <pid address> <status value>",
		"resourcestatus",
		"updateresource <resource name> <status value>",
		"newstislibrary <library version>",
		"newttislibrary <library version>",
		"currentstislibrary <library version>",
        "sendratistostation <STA> <num PID> <tag> <priority 1-8> <vetting 0/1> <override 0/1> <clearing 0/1> <start YYYYMMDDHHMMSS> <end YYYYMMDDHHMMSS>",
        "sendratistotrain <num train> <tag> <priority 1-8> <vetting 0/1> <override 0/1> <clearing 0/1> <start YYYYMMDDHHMMSS> <end YYYYMMDDHHMMSS>",
        "sendratistoallstations <tag> <priority 1-8> <vetting 0/1> <override 0/1> <clearing 0/1> <start YYYYMMDDHHMMSS> <end YYYYMMDDHHMMSS>",
        "sendratistoalltrains <tag> <priority 1-8> <vetting 0/1> <override 0/1> <clearing 0/1> <start YYYYMMDDHHMMSS> <end YYYYMMDDHHMMSS>",
		"ratisresponse < 0-Positive 1-Negative 2-PlayDead>",
		"displayrequestresponse < 0-Positive 1-Negative 2-PlayDead>"
	};


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	STISSimulatorCommandProcessor::STISSimulatorCommandProcessor(STISSimulator* parentSimulator)
	:		m_parentSimulator(parentSimulator),
			m_errMsg("")
	{

	}

	STISSimulatorCommandProcessor::~STISSimulatorCommandProcessor()
	{

	}

		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//					ISimulationCommandProcessor methods						//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

	//
	//hasCommandName
	//
	bool STISSimulatorCommandProcessor::hasCommandName( const std::string &name )
	{
		if (name == "help")
		{
			return true;
		}

		return hasCommand(name);
	}


	//
	//usageString
	//
	std::string STISSimulatorCommandProcessor::usageString(const std::string &name)
	{
		return cmdUsage(name);
	}


	//
	//errorMsg
	//
	std::string STISSimulatorCommandProcessor::errorMsg()
	{
		return m_errMsg;
	}


	//
	//execute
	//
	bool STISSimulatorCommandProcessor::execute(const std::string &name, const std::string &params)
	{
		m_errMsg = "";

		if(name == "help" )
		{
			for (unsigned int i = 0; i < NUM_COMMANDS; i++)
			{
				std::cout << commandUsage[i] << std::endl;
			}
			return true;
		}
        else if(name == "sendPacket")
		{
			m_parentSimulator->execSendPacket( params );
			return true;
		}
		else if(name == "status")
		{
			m_parentSimulator->execStatus();
			return true;
		}
        else if(name == "updatepid")
        {
            m_parentSimulator->pidStatusChange( params  );
            return true;
        }
        else if(name == "resourcestatus")
        {
            m_parentSimulator->displayResourceStatus();
            return true;
        }
        else if(name == "updateresource")
        {
            m_parentSimulator->resourceStatusChange( params  );
            return true;
        }
		else if( name == "newstislibrary" )
		{
			m_parentSimulator->newStisLibrary( params  );
            return true;
		}
		else if( name == "newttislibrary" )
		{
			m_parentSimulator->newTtisLibrary( params  );
            return true;
		}
		else if( name == "currentstislibrary" )
		{
			m_parentSimulator->currentStisLibrary( params  );
            return true;
		}
		else if( name == "sendratistostation" )
		{
			m_parentSimulator->sendRATISPacketToStation( params  );
            return true;
		}
		else if( name == "sendratistotrain" )
		{
			m_parentSimulator->sendRATISPacketToTrain( params  );
            return true;
		}
		else if( name == "sendratistoallstations" )
		{
			m_parentSimulator->sendRATISPacketToAllStations( params  );
            return true;
		}
		else if( name == "sendratistoalltrains" )
		{
			m_parentSimulator->sendRATISPacketToAllTrains( params  );
            return true;
		}
		else if( name == "ratisresponse")
		{
			m_parentSimulator->setRATISResponseType(params);
			return true;
		}
		else if( name == "displayrequestresponse")
		{
			m_parentSimulator->setDisplayRequestResponseType(params);
			return true;
		}

		return true;
	}


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//				StisSimulatorCommandProcessor specific methods				//
		//																			//
		//////////////////////////////////////////////////////////////////////////////


	//
	//hasCommand
	//
	bool STISSimulatorCommandProcessor::hasCommand( const std::string &name )
	{
		for (unsigned int i = 0; i < NUM_COMMANDS; i++)
		{
			if (commands[i] == name)
			{
				return true;
			}
		}
		return false;
	}


	//
	//cmdUsage
	//
	std::string STISSimulatorCommandProcessor::cmdUsage(const std::string& name)
	{
		for (unsigned int i = 0; i < NUM_COMMANDS; i++)
		{
			if (commands[i] == name)
			{
				return commandUsage[i];
			}
		}
		return "";
	}

}//close namespace
