/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioSimulatorCommandProcessor.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * RadioSimulator implements ISimulationModule, for use with the generic simulator.
  */

#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulatorCommandProcessor.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulator.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

namespace TA_IRS_Bus
{

    std::string RadioSimulatorCommandProcessor::commands[NUM_COMMANDS] =
    {
        "status",
        "newcall",
        "trainsds",
        "calcsum",
        "loadsim",
        "clearsim",
        "showsim",
        "runsim",
        "sessions"
    };

    std::string RadioSimulatorCommandProcessor::commandUsage[NUM_COMMANDS] =
    {
        "status",
        "newcall <sessionid> <callreference> <calltype> <callingtSI> <calledTSI>",
        "trainsds <sessionid> <sourceITSI> <MessageData,MessageData,MessageData>",
        "calcsum <sessionid> <sourceITSI> <MessageData,MessageData,MessageData> # without sum digits",
        "loadsim <path to configSimulator.ini>",
        "clearsim ",
        "showsim ",
        "runsim <sim id>",
        "sessions"
    };


    RadioSimulatorCommandProcessor::RadioSimulatorCommandProcessor( RadioSimulator& parentSimulator )
        : m_parentSimulator( parentSimulator ),
          m_errMsg("")
    {
        FUNCTION_ENTRY( "RadioSimulatorCommandProcessor" );
        FUNCTION_EXIT;
    }


    RadioSimulatorCommandProcessor::~RadioSimulatorCommandProcessor()
    {
        FUNCTION_ENTRY( "~RadioSimulatorCommandProcessor" );
        FUNCTION_EXIT;
    }


    bool RadioSimulatorCommandProcessor::hasCommandName( const std::string &name )
    {
        FUNCTION_ENTRY( "hasCommandName" );

        if (name == "help")
        {
            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return hasCommand(name);
    }


    std::string RadioSimulatorCommandProcessor::usageString( const std::string &name )
    {
        FUNCTION_ENTRY( "usageString" );
        FUNCTION_EXIT;
        return cmdUsage( name );
    }


    std::string RadioSimulatorCommandProcessor::errorMsg()
    {
        FUNCTION_ENTRY( "errorMsg" );
        FUNCTION_EXIT;
        return m_errMsg;
    }


    bool RadioSimulatorCommandProcessor::execute( const std::string &name,
                                                  const std::string &params )
    {
        FUNCTION_ENTRY( "execute" );

        m_errMsg = "";

        if( name == "help" )
        {
            std::ostringstream output;
            for ( unsigned int i = 0; i < NUM_COMMANDS; ++i )
            {
                output << commandUsage[i] << std::endl;
            }

            COUT( output.str() );
            
            FUNCTION_EXIT;
            return true;
        }
        
        else if ( name == "status" )
        {
            m_parentSimulator.execStatus();

            FUNCTION_EXIT;
            return true;
        }
        else if ( name == "newcall" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.newCall(params);
        }
        else if ( name == "trainsds" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.sendSDSData(params);
        }
        else if ( name == "calcsum" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.calcChecksumForSDSData(params);
        }
        else if ( name == "loadsim" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.simulatorLoadTable(params);
        }
        else if ( name == "clearsim" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.simulatorClearTable(params);
        }
        else if ( name == "showsim" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.simulatorShowTable(params);
        }
        else if ( name == "runsim" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.simulatorRun(params);
        }
        else if ( name == "sessions" )
        {
            FUNCTION_EXIT;
            return m_parentSimulator.showSessions();
        }

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulatorCommandProcessor::hasCommand( const std::string &name )
    {
        FUNCTION_ENTRY( "hasCommand" );

        for ( unsigned int i = 0; i < NUM_COMMANDS; ++i )
        {
            if ( commands[i] == name )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    //
    //cmdUsage
    //
    std::string RadioSimulatorCommandProcessor::cmdUsage( const std::string& name )
    {
        FUNCTION_ENTRY( "cmdUsage" );

        for ( unsigned int i = 0; i < NUM_COMMANDS; ++i )
        {
            if ( commands[i] == name )
            {
                FUNCTION_EXIT;
                return commandUsage[i];
            }
        }

        FUNCTION_EXIT;
        return "";
    }

}
