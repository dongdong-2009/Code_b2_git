/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_server_simulator/src/GenericCommandProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GenericCommandProcessor.h"
#include "bus/generic_server_simulator/src/VersionCommand.h"
#include "bus/generic_server_simulator/src/DetachCommand.h"
#include "bus/generic_server_simulator/src/ExecCommand.h"

using namespace TA_Base_Bus;


GenericCommandProcessor::GenericCommandProcessor( CommandMonitor * commandMonitor )
: GssCommandProcessor(0)
, m_commandMonitor(commandMonitor)
{
    new VersionCommand(this);
    new DetachCommand(this, "background");
    new DetachCommand(this, "bg");
    new ExecCommand(this, commandMonitor);
}

