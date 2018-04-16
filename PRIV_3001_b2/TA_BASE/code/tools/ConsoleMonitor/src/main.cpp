// NSAMonitor.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include "stdafx.h"
#include "MainGuard.h"
#include "MonitorMessageHelper.h"
#include "MonitorCommonDataType.h"
#include "CommandParserFactory.h"

#if defined( SOLARIS )
    #include "core/utilities/src/StdPubUtility.h"
    #define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "core/utilities/src/RunParams.h"


void dispalyUsage()
{
    std::cout << " common usage:" << std::endl;
    std::cout << " help" << std::endl;
    std::cout << " state -- show current module state" << std::endl;
    std::cout << " module [modulename] [entityname] [endpoint]  -- switch module" << std::endl;
    std::cout << " module current -- list current module" << std::endl;
    std::cout << " module -- list all loaded modules" << std::endl;
    std::cout << " dosmoketest -- test message sender" << std::endl;
    std::cout << " quit" << std::endl;

}

void processInBachMode()
{
    std::cout << "!!! working in batch mode !!!" << std::endl;

    std::ofstream* log_file_strm = NULL;
    boost::filesystem::path log_file_path;
    size_t batch_mode_command_number = TA_Base_Core::getRunParamValue( "BatchModeCommandNumber", 100 );
    size_t batch_mode_interval = TA_Base_Core::getRunParamValue( "BatchModeInterval", 60 );
    boost::uintmax_t batch_mode_log_file_max_size = TA_Base_Core::getRunParamValue( "BatchModeLogFileMaxSize", 20 * 1024 * 1024/* 20MB */ );
    std::string batch_mode_log_file = TA_Base_Core::RunParams::getInstance().get( "BatchModeLogFile" );

    if ( true == batch_mode_log_file.empty() )
    {
        batch_mode_log_file = "ConsoleMonitor.log";
    }

    std::cout << "command list:" << std::endl;

    std::vector<std::string> batch_command_list;

    for ( size_t i = 0; i < batch_mode_command_number; ++i )
    {
        std::string param = "BatchModeCommand" + boost::lexical_cast<std::string>(i + 1);
        std::string batch_mode_command = TA_Base_Core::RunParams::getInstance().get( param.c_str() );

        if ( true == batch_mode_command.empty() )
        {
            break;
        }

        batch_command_list.push_back( batch_mode_command );

        std::cout << "\t" << batch_mode_command << std::endl;
    }

    if ( true == batch_command_list.empty() )
    {
        return;
    }

    while ( true )
    {
        {
            // file iteration
            static size_t log_file_iterator = 0;
            boost::uintmax_t log_file_size = 0;

            if ( log_file_strm != NULL && boost::filesystem::exists( log_file_path ) )
            {
                std::cout << std::flush;
                log_file_size = boost::filesystem::file_size( log_file_path );
                assert( log_file_size != -1 );
            }

            if ( NULL == log_file_strm || batch_mode_log_file_max_size <= log_file_size )
            {
                log_file_path = boost::filesystem::path( batch_mode_log_file );
                log_file_path.replace_extension( boost::lexical_cast<std::string>( ++log_file_iterator ) + log_file_path.extension() );

                std::ofstream* tmp = log_file_strm;
                log_file_strm = new std::ofstream( log_file_path.filename().c_str() );
                std::cout.rdbuf( log_file_strm->rdbuf() );
                delete tmp;
            }
        }

        for ( size_t i = 0; i < batch_command_list.size(); ++i )
        {
            TA_Base_Core::CommonModuleCommandParser* parser = TA_Base_Core::CommandParserFactory::getInstance().getCurrentCommandParser();
            const char* currModuleName = parser->getModuleType();

            if ( !parser->getCatchInput() )
            {
                std::cout << currModuleName << ">";
            }

            std::vector<std::string> command_args;
            std::string& command = batch_command_list[i];

            TA_Base_Core::MonitorStaticLib::splitCommand(command.c_str(), command_args);

            if ( _stricmp("module", command_args[0].c_str() ) == 0 )
            {
                TA_Base_Core::CommandParserFactory::getInstance().processModuleCommand(command_args);
            }
            else
            {
                parser->parseCommand(command_args );
            }
        }

        TA_Base_Core::Thread::sleep( batch_mode_interval * 1000 );
    }

}

void processInInteractiveMode( bool isAgentSide )
{
    dispalyUsage();

    {
        //default module is NSA module
        std::string moduleType = TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_NSA;
        std::string entityName = "OccNotificationServiceAgent";

        std::string endPoint = TA_Base_Core::RunParams::getInstance().get(RPARAM_NOTIFYHOSTS);
        TA_ASSERT( !endPoint.empty(), "NotifyHosts parameter should not empty" );
        
        TA_Base_Core::CommandParserFactory::getInstance().connectModule(moduleType, entityName, endPoint);

        /*
        //just for test
        std::string moduleType = TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_COMMON;
        std::string entityName = "MyAgent";

        std::string endPoint = "localhost:7788";
        currParser->initCorbaEntity("MyAgent", endPoint);

        {
        //only for test
        std::vector<std::string> vtCommand;
        vtCommand.push_back("conninter");
        vtCommand.push_back("localhost:7788");
        vtCommand.push_back("MyAgentInterface");
        currParser->connectInteractiveObj(vtCommand);
        }
        */
    }

    TA_Base_Core::MonitorMessageHelper monitorHelper(isAgentSide);
    monitorHelper.init();

    std::string command;
    std::vector<std::string> vtCommandArg;
    while ( true )
    {
        TA_Base_Core::CommonModuleCommandParser * parser = TA_Base_Core::CommandParserFactory::getInstance().getCurrentCommandParser();
        const char * currModuleName = parser->getModuleType();
        if ( !parser->getCatchInput() )
        {
            std::cout << currModuleName << ">"; 
        }

        std::getline(std::cin, command);

        if ( command.empty() )
        {
            continue;
        }

        TA_Base_Core::MonitorStaticLib::splitCommand(command.c_str(), vtCommandArg);

        if ( vtCommandArg.empty() )
        {
            std::cout <<"Please input valid content" << std::endl;
            continue;
        }


        if(  _stricmp("quit", vtCommandArg[0].c_str() ) == 0 )
        {
            break;
        }
        else if( _stricmp("help", vtCommandArg[0].c_str() ) == 0 )
        {
            dispalyUsage();
            std::cout << "\ncurrent module usage" << std::endl;
            parser->displayUsage();
        }
        else if( _stricmp("module", vtCommandArg[0].c_str() ) == 0 )
        {
            TA_Base_Core::CommandParserFactory::getInstance().processModuleCommand(vtCommandArg);
        }
        else if( _stricmp("dosmoketest", vtCommandArg[0].c_str() ) == 0 )
        {
            monitorHelper.doSmokeTest();
        }
        else
        {
            parser->parseCommand(vtCommandArg );
        } //end if
    } //end while
}


int main(int argc, char* argv[])
{
    MainGuard main_guard( argc, argv );

    bool isAgentSide = 0;
    
    //set console montor's entityName
    if (!isAgentSide)
    {
        TA_Base_Core::RunParams::getInstance().set(RPARAM_ENTITYNAME,TA_Base_Core::InnerMonitorConsts::MONITOR_ENTITYNAME);
    }

    //check entityName and locationKey
    {
        std::string myEntityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        unsigned int myLocationKey = TA_Base_Core::getRunParamValue(RPARAM_LOCATIONKEY, 0);

        if ( true == myEntityName.empty() )
        {
            std::cout <<  "EntityName should not be empty" << std::endl;
            return -1;
        }

        if (myLocationKey <= 0)
        {
            std::cout << "locationKey should be larger than 0" << std::endl;
            return -1;
        }

        if ( ! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS) )
        {
            std::cout << "parameter NotifyHosts should be set" << std::endl;
            return -1;
        }

    }

    if ( TA_Base_Core::RunParams::getInstance().isSet( "BatchMode" ) )
    {
        processInBachMode();
    }
    else
    {
        processInInteractiveMode(isAgentSide);
    } 

    return 0;

}

