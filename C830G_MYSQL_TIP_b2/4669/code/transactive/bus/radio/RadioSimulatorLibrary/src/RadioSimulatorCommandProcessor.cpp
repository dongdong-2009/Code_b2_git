/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * RadioSimulator implements ISimulationModule, for use with the generic simulator.
  */

#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulatorCommandProcessor.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulator.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

namespace TA_IRS_Bus
{
	struct param
	{
RadioSimulator *ps;
unsigned simid_from,simid_to,wait_interval,simid_from_2, simid_to_2, wait_interval_2;
	};
	class runner : public virtual TA_Base_Core::Thread
	{
		param *v_param;
	public:
		runner(param *p)
		{
		v_param=p;
		start();
		};
		void terminate()
		{

		}
		void run()
		{
			while(1)
			{
			for(unsigned i=v_param->simid_from;i<=v_param->simid_to;i++)
			{
				char buf[200];
				sprintf(buf,"%u",i);
				v_param->ps->simulatorRun(buf);
				sprintf(buf,"Running : Simid - %u\n",i);
				COUT(buf);
			}
			Sleep(v_param->wait_interval*1000);
			for(unsigned j=v_param->simid_from_2;j<=v_param->simid_to_2;j++)
			{
				char buf[200];
				sprintf(buf,"%u",j);
				v_param->ps->simulatorRun(buf);
				sprintf(buf,"Running 2 : Simid - %u\n",j);
				COUT(buf);
			}
            Sleep(v_param->wait_interval_2*1000);
			}
			
		};
	};
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
		"runsim_multiple",
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
		"runsim_multiple <simid-FROM> <simid-TO> <interval in secs> < simid-FROM_2> <simid-TO_2> <interval_2 in secs> ",
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
		else if ( name == "runsim_multiple" )
		{
	unsigned simid_from,simid_to,interval,simid_from_2, simid_to_2, interval_2 ;
      
	if(sscanf(params.c_str(),"%u %u %u %u %u %u ",&simid_from,&simid_to,&interval,&simid_from_2, &simid_to_2 ,&interval_2)!= 6 )
	{
	return false;
	}
	param *p=new param;
	p->simid_from=simid_from;
	p->simid_to=simid_to;
	p->wait_interval=interval;
	p->ps=&m_parentSimulator;
	p->simid_from_2 = simid_from_2 ; 
	p->simid_to_2 = simid_to_2 ;
	p->wait_interval_2 = interval_2 ;



	runner *r=new runner(p);
	FUNCTION_EXIT;
	return true;
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
