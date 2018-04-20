/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/LeafCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/LeafCommand.h"


using namespace TA_Base_Bus;

LeafCommand::LeafCommand(CommandOwner * owner, const char * name)
: ICommand(owner, name)
{
}

bool LeafCommand::isCommand(const std::string & name) const
{
    return (name == getName());
}

bool LeafCommand::hasSubCommand(const std::string & name) const
{
    return false;
}

