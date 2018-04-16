#ifndef SetTrainCommand_H
#define SetTrainCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTrainCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Declaration file for classes used by ATSSimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/IntermediateCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainLocationCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainNumberCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainEventCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
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

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetTrainCommand_H
