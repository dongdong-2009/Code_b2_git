#ifndef LogModeCommand_H
#define LogModeCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/LogModeCommand.h $
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

#include "app/simulator/pa/PASimulatorLibrary/cmd/LeafCommand.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    class PASimulatorCommandProcessor;

    /** 
      * 
      * 
      */
    class LogModeCommand : public LeafCommand
    {
	public:
        LogModeCommand(CommandOwner * owner);

        bool makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err);

        void usage(const std::string & name, std::ostream & os) const;
	
	}; // class ShowTableSubCommand

} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef LogModeCommand_H
