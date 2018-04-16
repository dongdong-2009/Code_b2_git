#ifndef SetCommand_H
#define SetCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/cmd/SetCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/IntermediateCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetTableCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetWildCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
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

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetCommand_H
