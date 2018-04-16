#ifndef SetPlatformCommand_H
#define SetPlatformCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetPlatformCommand.h $
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
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetPlatformDoorCommand.h"
// #include "app/simulator/pa/PASimulatorLibrary/cmd/SetPlatformEventCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    /** 
      * 
      * 
      */
    class SetPlatformCommand : public IntermediateCommand
    {
	public:
        SetPlatformCommand(CommandOwner * owner)
            : IntermediateCommand(owner, "platform")
            {
                new SetPlatformDoorCommand(this);
                // new SetPlatformEventCommand(this);
            }
    }; // class SetPlatformCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetPlatformCommand_H
