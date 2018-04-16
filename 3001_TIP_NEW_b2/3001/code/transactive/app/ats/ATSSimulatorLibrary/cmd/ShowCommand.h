#ifndef ShowCommand_H
#define ShowCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/ShowCommand.h $
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
#include "app/ats/ATSSimulatorLibrary/cmd/ShowTableCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowIscsTableCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowModeCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    /** 
      * 
      * 
      */
    class ShowCommand : public IntermediateCommand
    {
	public:
        ShowCommand(CommandOwner * owner)
            : IntermediateCommand(owner, "show")
            {
                new ShowTableCommand(this);
                new ShowIscsTableCommand(this,"watchdog");
                new ShowIscsTableCommand(this,"train");
                new ShowIscsTableCommand(this,"platform");
                new ShowIscsTableCommand(this,"electrical");
                new ShowIscsTableCommand(this,"atcdata");
                new ShowIscsTableCommand(this,"wash");
                new ShowIscsTableCommand(this,"twp");
                new ShowModeCommand(this);
            }
    }; // class ShowCommand

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef ShowCommand_H
