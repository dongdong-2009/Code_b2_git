// start_monitor_agent.cpp : Defines the entry point for the console application.
//

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_iterator.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <sstream>


const std::string RARAM_DEPLOY_HOST_NAMES = "DeployHostNames";
const std::string RARAM_NORMAL_OPERATION_MODE = "NormalOperationMode";
const std::string RARAM_START_OR_STOP = "StartOrStop";
const std::string RARAM_WAIT_SYSTEM_CONTROLLER_STARTUP_IN_SECONDS = "WaitSystemControllerStartupInSeconds";


class DeployStartAgents : public TA_Base_Core::Thread
{
public:

    DeployStartAgents( const std::string& hostname, TA_Base_Core::EOperationMode normalOperationMode, bool isStart )
        : m_isStart( isStart ),
          m_normalOperationMode( normalOperationMode ),
          m_hostname( hostname )
    {
        FUNCTION_ENTRY( "DeployStartAgents" );

        start();

        FUNCTION_EXIT;
    }

    virtual void run()
    {
        FUNCTION_ENTRY( "run" );

		initSystemController();

        startAgents();

        FUNCTION_EXIT;
    }

    virtual void terminate() {};

    virtual ~DeployStartAgents()
    {
        FUNCTION_ENTRY( "~DeployStartAgents" );

        terminateAndWait(); 

        FUNCTION_EXIT;
    }

private:

    void startAgents()
    {
        FUNCTION_ENTRY( "startAgents" );

        if ( CORBA::is_nil( m_systemController ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_systemController is null [hostname=%s]", m_hostname.c_str() );

            FUNCTION_EXIT;
            return;
        }

        static TA_Base_Core::NonReEntrantThreadLockable lock;
        
        TA_THREADGUARD( lock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin start agents [hostname=%s][mode=%d][is start=%s]", m_hostname.c_str(), m_normalOperationMode, m_isStart ? "true" : "false" );

        try
        {
            m_systemController->deployProcesses( m_normalOperationMode, m_isStart );
        }
        catch ( CORBA::Exception& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception [%s]", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "" );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End start agents [hostname=%s][mode=%d][is start=%s]", m_hostname.c_str(), m_normalOperationMode, m_isStart ? "true" : "false" );

        FUNCTION_EXIT;
    }

    void initSystemController()
    {
        FUNCTION_ENTRY( "initSystemController" );

        std::string portNumber = TA_Base_Core::RunParams::getInstance().get( "PortNumber" );

        if ( true == portNumber.empty() )
        {
            portNumber = boost::lexical_cast< std::string >( TA_Base_Core::DEFAULT_MGR_PORT );
        }

        std::string connectionString;

        connectionString = "corbaloc::";
        connectionString += m_hostname + ":";
        connectionString += portNumber + "/SystemControllerAdmin";

        static TA_Base_Core::NonReEntrantThreadLockable lock;

        TA_THREADGUARD( lock );

        int waitSystemController = 0;

        std::string waitSystemControllerStr = TA_Base_Core::RunParams::getInstance().get( RARAM_WAIT_SYSTEM_CONTROLLER_STARTUP_IN_SECONDS.c_str() );

        if ( false == waitSystemControllerStr.empty() )
        {
            waitSystemController = boost::lexical_cast< int >( waitSystemControllerStr );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin narrow [%s]", connectionString.c_str() );

        for ( ; 0 <= waitSystemController; --waitSystemController, sleep( 1000 ) )
        {
            try
            {
                CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( connectionString.c_str() );
                
                m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_narrow( obj );

                break;
            }
            catch ( CORBA::Exception& ex )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Caught CORBA exception while narrow corba object SystemControllerAdmin [connection string=%s][exception: %s]",
                    connectionString.c_str(), TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str() );
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Caught unknown exception while narrow corba object SystemControllerAdmin [connection string=%s]", connectionString.c_str() );
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End narrow [%s]", connectionString.c_str() );

        FUNCTION_EXIT;
    }

private:

    bool m_isStart;
    TA_Base_Core::EOperationMode m_normalOperationMode;
    std::string m_hostname;
    TA_Base_Core::ISystemControllerAdminCorbaDef_var m_systemController;
};

typedef boost::shared_ptr<DeployStartAgents> DeployStartAgentsPtr;
typedef std::vector<DeployStartAgentsPtr> DeployStartAgentsPtrList;


int main( int argc, char* argv[] )
{
    TA_Base_Core::UtilityInitialiser::initialiseUtilities( argc, argv );

    if ( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_VERSION ) )
    {
        // Version information will have been printed to std::cout
        // No need to proceed any further.
        exit( 0 );
    }

    // get run parameter host names
    std::string deployHostNames = TA_Base_Core::RunParams::getInstance().get( RARAM_DEPLOY_HOST_NAMES.c_str() );

    if ( true == deployHostNames.empty() )
    {
        deployHostNames =
			"occa,occb,sms-dbg,sms-bbs,sms-epn,sms-pmn,"
			"sms-nch,sms-sdm,sms-mbt,sms-dkt,sms-pyl,"
			"sms-mps,sms-tsg,kcda,kcdb,sms-bly,"
			"sms-ser,sms-lrc,sms-bsh,sms-mrm,sms-tsn,"
			"sms-bkb,sms-btn,sms-frr,sms-hlv,sms-bnv,"
			"sms-onh,sms-krg,sms-wct,sms-ppj,sms-lbd,"
			"sms-tlb,sms-hbf";
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[deploy host names=%s]", deployHostNames.c_str() );

    // get run parameter normal operation mode
    TA_Base_Core::EOperationMode normalOperationMode = TA_Base_Core::NotApplicable;

    std::string operationMode = TA_Base_Core::RunParams::getInstance().get( RARAM_NORMAL_OPERATION_MODE.c_str() );

    if ( "control" == operationMode || "CONTROL" == operationMode || "Control" == operationMode )
    {
        normalOperationMode = TA_Base_Core::Control;
    }
    else if ( "monitor" == operationMode || "MONITOR" == operationMode || "Monitor" == operationMode )
    {
        normalOperationMode = TA_Base_Core::Monitor;
    }
    
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[normalOperationMode=%d][run param=%s]", normalOperationMode, operationMode.c_str() );

    // get run parameter start or stop
    bool isStart = false;

    std::string startStop = TA_Base_Core::RunParams::getInstance().get( RARAM_START_OR_STOP.c_str() );

    if ( "start" == startStop || "START" == startStop || "Start" == startStop )
    {
        isStart = true;
    }

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[isStart=%d][run param=%s]", isStart, startStop.c_str() );

    DeployStartAgentsPtrList deployStartAgentsList;

    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( ", " );

    tokenizer tokens( deployHostNames, sep );

    for( tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); ++iter )
    {
		std::string hostname = *iter;

        DeployStartAgentsPtr item( new DeployStartAgents( hostname, normalOperationMode, isStart ) );

        deployStartAgentsList.push_back( item );
    }

    deployStartAgentsList.clear();

    TA_Base_Core::CorbaUtil::cleanup();

    return 0;
}
