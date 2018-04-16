/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/CommandOwner.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include "app/ats/ATSSimulatorLibrary/cmd/CommandOwner.h"
#include "app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h"

using namespace TA_IRS_App::ATS_Sim;


////////////////////////////////////////////////////////////////
//
// CommandOwner
//
////////////////////////////////////////////////////////////////
CommandOwner::CommandOwner()
{
}

CommandOwner::~CommandOwner()
{
    for ( CommandMap::iterator iter = m_commands.begin() ;
            iter != m_commands.end() ;
            iter++
        )
    {
        delete ((*iter).second);
    }
}

void CommandOwner::addCommand(HierarchicalCommand * cmd)
{
    if ( cmd != 0 )
    {
		const std::string & name = cmd->getName();

        m_commands[name] = cmd;
    }
}

HierarchicalCommand * CommandOwner::find(const std::string & name)
{
    CommandMap::iterator found = m_commands.find(name);

    if ( found != m_commands.end() )
    {
        return ((*found).second);
    }

    return 0;
}

const CommandMap & CommandOwner::getCommands() const
{
    return m_commands;
}



