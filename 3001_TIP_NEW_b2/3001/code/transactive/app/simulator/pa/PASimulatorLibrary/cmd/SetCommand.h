#ifndef SetCommand_H
#define SetCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetCommand.h $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/IntermediateCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTableCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTrainCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetWildCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    /** harbours LeafCommands, but doesn't implement any functionality itself
      * 
      * The definition only contains a constructor which could really have been
      * inline in this file, however every time a new set subcommand is added a
      * large number of files need to be recompiled, which is just silly.
      */
    class SetCommand : public IntermediateCommand
    {
	public:
        SetCommand(CommandOwner * owner);
    }; // class SetCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetCommand_H
