/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_server_simulator/src/CommandOwner.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include "bus/generic_server_simulator/src/CommandOwner.h"
#include "bus/generic_server_simulator/src/ICommand.h"

using namespace TA_Base_Bus;


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

void CommandOwner::addCommand(ICommand * cmd)
{
    if ( cmd != 0 )
    {
		const std::string & name = cmd->getName();

        m_commands[name] = cmd;
    }
}

ICommand * CommandOwner::find(const std::string & name) const
{
    CommandMap::const_iterator found = m_commands.find(name);

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



