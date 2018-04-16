#ifndef SetTrainCommand_H
#define SetTrainCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetTrainCommand.h $
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
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTrainLocationCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTrainNumberCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTrainEventCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    /** 
      * 
      * 
      */
    class SetTrainCommand : public IntermediateCommand
    {
	public:
        SetTrainCommand(CommandOwner * owner)
            : IntermediateCommand(owner, "train")
            {
                new SetTrainLocationCommand(this);
                new SetTrainNumberCommand(this);
                new SetTrainEventCommand(this);
            }
    }; // class SetHierarchicalCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetTrainCommand_H
