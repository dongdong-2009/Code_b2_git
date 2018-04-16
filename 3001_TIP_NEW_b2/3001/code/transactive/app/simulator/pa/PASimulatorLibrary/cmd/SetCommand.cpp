/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetCommand.cpp $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/SetCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTableCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTrainCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetWildCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/ShowModeCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetDssCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetPlatformCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetTwpCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetRegisterCommand.h"
#include "app/simulator/pa/PASimulatorLibrary/cmd/SetLiveBroadcastReponseTime.h"
//#include "app/simulator/pa/PASimulatorLibrary/cmd/SetWatchdogCommand.h"

using namespace TA_IRS_App::PA_Sim;

SetCommand::SetCommand(CommandOwner * owner) 
: IntermediateCommand(owner, "set")
{
    /*new SetTableCommand(this);*/
    //new SetTrainCommand(this);
    //new SetWildCommand(this);
    //new ShowModeCommand(this);
    //new SetDssCommand(this);
	new SetLiveBroadcastReponseTime(this);
    //new SetPlatformCommand(this);
    //new SetTwpCommand(this);
	new SetRegisterCommand(this);
    //new SetWatchdogCommand(this);
}

