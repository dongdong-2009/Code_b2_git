#ifndef SetPlatformCommand_H
#define SetPlatformCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/cmd/SetPlatformCommand.h $
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
#include "app/ats/ATSSimulatorLibrary/cmd/SetPlatformDoorCommand.h"
// #include "app/ats/ATSSimulatorLibrary/cmd/SetPlatformEventCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
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

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetPlatformCommand_H
