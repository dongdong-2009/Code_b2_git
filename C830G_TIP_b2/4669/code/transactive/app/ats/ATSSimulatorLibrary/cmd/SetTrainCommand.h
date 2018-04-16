#ifndef SetTrainCommand_H
#define SetTrainCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTrainCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
