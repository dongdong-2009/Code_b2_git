#ifndef LogCommand_H
#define LogCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/LogCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for classes used by PASimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/IntermediateCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/LogStartCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/LogStopCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/LogModeCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    /** 
      * 
      * 
      */
    class LogCommand : public IntermediateCommand
    {
	public:
        LogCommand(CommandOwner * owner)
            : IntermediateCommand(owner, "log")
            {
                new LogStartCommand(this);
                new LogStopCommand(this);
                new LogModeCommand(this);
            }
    }; // class LogCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef LogCommand_H
