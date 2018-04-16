/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimulatorCommandProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  * ATSSimulatorCommandProcessor implements CommandProcessor, for use with the generic simulator.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimulatorCommandProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"

#include "app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/StatusCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowStationCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/LogCommand.h"

using namespace TA_IRS_App::ATS_Sim;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ATSSimulatorCommandProcessor::ATSSimulatorCommandProcessor(ATSSimulator* parent, LocationType location)
: m_parent(parent)
, m_location(location)
, m_showLocation(location)
, m_errMsg("")
, m_showCmd(0)
, m_setCmd(0)
{
    m_setCmd  = new SetCommand(this);
    m_showCmd = new ShowCommand(this);
    m_statusCmd = new StatusCommand(this, m_parent);
    m_logCmd  = new LogCommand(this);

    // the 'show station' command reports directly to "this"
    new ShowStationCommand(m_showCmd, this);

    if (m_location == STATION)
    {
        m_showLocation = MSS;
    }
}

ATSSimulatorCommandProcessor::~ATSSimulatorCommandProcessor()
{

}

    //////////////////////////////////////////////////////////////////////////////
    //                                                                            //
    //                    ISimulationCommandProcessor methods                        //
    //                                                                            //
    //////////////////////////////////////////////////////////////////////////////

//
//hasCommandName
//
bool ATSSimulatorCommandProcessor::hasCommandName( const std::string &name )
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
std::string ATSSimulatorCommandProcessor::usageString(const std::string &name)
{
    return cmdUsage(name);
}


//
//errorMsg
//
std::string ATSSimulatorCommandProcessor::errorMsg()
{
    return m_errMsg;
}


//
//execute
//
bool ATSSimulatorCommandProcessor::execute(const std::string &name, const std::string &params)
{
    m_errMsg = "";

    if (name == "help" )
    {
        const CommandMap & cmds = getCommands();

        for ( CommandMap::const_iterator iter = cmds.begin() ;
                iter != cmds.end() ;
                iter++
            )
        {
            ((*iter).second)->usage((*iter).first, std::cout);
        }

        return true;
    }

    else if(name == "dump")
    {
//        return m_parent->execShowTable( params, Dump );
		return false;
    }
    
    else if(name == "log")
    {
        return execLog( params );
    }
    
    else if(name == "set")
    {
        // return m_parent->execSetTable( params );
        return execSet( params );
    }
    
    else if(name == "show")
    {
        // return m_parent->execShowTable( params, Show );
        return execShow( params, Show );
    }
    
    else if(name == "status")
    {
        return execStatus( params );
    }


    return true;
}


    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                ATSSimulatorCommandProcessor specific methods             //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

LocationType ATSSimulatorCommandProcessor::getLocation() const
{
    return m_location;
}

LocationType ATSSimulatorCommandProcessor::getShowLocation() const
{
    return m_showLocation;
}

void ATSSimulatorCommandProcessor::setShowLocation(LocationType sLocation)
{
    m_showLocation = sLocation;
}

bool ATSSimulatorCommandProcessor::hasCommand( const std::string &name )
{
    if ( find(name) != 0)
    {
        return true;
    }

    return false;
}


std::string ATSSimulatorCommandProcessor::cmdUsage(const std::string& name)
{

    HierarchicalCommand * cmd = find(name);
    if ( cmd != 0 )
    {
        std::stringstream streem;
        cmd->usage(name, streem);

        return streem.str();
    }

    return "";
}

bool ATSSimulatorCommandProcessor::execShow( const std::string & params, Byte qType )
{
    // TA_Base_Core::ThreadGuard oguard(m_observerLock);

    // demultiplex/morph complex show queries into the generic
    // "show table <table name> <record range>" format

    // the tokenise'd version of given params
    TokenList parameters;

    int numtoks = ATSSimUtility::tokenise(params.c_str(),parameters);

    if (numtoks >= 1)
    {
        UserQuery * query = new UserQuery((MessageType)qType, m_parent->getOutputFormat());

        if ( ! m_showCmd->makeQuery( parameters, params, query, m_showLocation, std::cout ) )
        {
            delete query;
            return false;
        }

        //
        // some successful command executions don't generate a real query
        // (eg. 'show mode'). The best way to detect those is to check if a
        // valid table address is set
        if ( query->getTableAddress() != TableAddressRegister::getInstance().INVALID_ADDRESS )
        {
            m_parent->getQueryProcessor()->addUserQuery(query);
        }
    }
    else
    {
		std::cout << "ERROR: 'show' command requires parameters" << std::endl;
        return false;
    }

    return true;
}

bool ATSSimulatorCommandProcessor::execSet( const std::string & params )
{
    // TA_Base_Core::ThreadGuard oguard(m_observerLock);

    // demultiplex/morph complex show queries into the generic
    // "show table <table name> <record range>" format

    // the tokenise'd version of given params
    TokenList parameters;

    int numtoks = ATSSimUtility::tokenise(params.c_str(),parameters);

    if (numtoks > 1)
    {
        UserQuery * query = new UserQuery(Set, m_parent->getOutputFormat());

        if ( ! m_setCmd->makeQuery( parameters, params, query, m_showLocation, std::cout ) )
        {
            delete query;
            return false;
        }

        //
        // some successful queries don't generate a real query. The best way
        // to detect those is to check if a valid table address is set
        if ( query->getTableAddress() != TableAddressRegister::getInstance().INVALID_ADDRESS )
        {
            m_parent->getQueryProcessor()->addUserQuery(query);
        }
    }
    else
    {
		std::cout << "ERROR: 'set' command requires parameters" << std::endl;
        return false;
    }

    return true;
}

bool ATSSimulatorCommandProcessor::execLog( const std::string & params )
{
    // TA_Base_Core::ThreadGuard oguard(m_observerLock);
    // the tokenise'd version of given params
    TokenList parameters;

    int numtoks = ATSSimUtility::tokenise(params.c_str(),parameters);

    if (numtoks >= 1)
    {
        UserQuery * query = new UserQuery(LogStart, m_parent->getOutputFormat());

        if ( ! m_logCmd->makeQuery( parameters, params, query, m_showLocation, std::cout ) )
        {
            delete query;
            return false;
        }

        //
        // some successful command executions don't generate a real query
        // (eg. 'show mode'). The best way to detect those is to check if a
        // valid table address is set
        if ( query->getTableAddress() != TableAddressRegister::getInstance().INVALID_ADDRESS )
        {
            m_parent->getQueryProcessor()->addUserQuery(query);
        }
    }
    else
    {
		std::cout << "ERROR: 'log' command requires parameters" << std::endl;
        return false;
    }

    return true;
}

bool ATSSimulatorCommandProcessor::execStatus( const std::string & params )
{
    TokenList parameters;
    return m_statusCmd->makeQuery( parameters, params, 0, m_showLocation, std::cout );
}

