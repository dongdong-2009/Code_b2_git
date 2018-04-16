#ifndef ShowStationCommand_H
#define ShowStationCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/ShowStationCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/LeafCommand.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimulatorCommandProcessor;

    /** 
      * 
      * 
      */
    class ShowStationCommand : public LeafCommand
    {
	public:
        ShowStationCommand(CommandOwner * owner, ATSSimulatorCommandProcessor * commandProcessor);

        bool makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err);

        void usage(const std::string & name, std::ostream & os) const;

    private:
		ATSSimulatorCommandProcessor * m_commandProcessor;
	
	}; // class ShowTableSubCommand

} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // #ifndef ShowStationCommand_H
