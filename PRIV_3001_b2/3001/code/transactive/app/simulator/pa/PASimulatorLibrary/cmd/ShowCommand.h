#ifndef ShowCommand_H
#define ShowCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/ShowCommand.h $
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
#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowTableCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowIscsTableCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowModeCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowRegisterCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
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
                //new ShowTableCommand(this);
                //new ShowIscsTableCommand(this,"watchdog");
                //new ShowIscsTableCommand(this,"train");
                //new ShowIscsTableCommand(this,"platform");
                //new ShowIscsTableCommand(this,"electrical");
                //new ShowIscsTableCommand(this,"atcdata");
                //new ShowIscsTableCommand(this,"wash");
                //new ShowIscsTableCommand(this,"twp");
                //new ShowModeCommand(this);
				new ShowRegisterCommand(this);
            }
    }; // class ShowCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef ShowCommand_H
