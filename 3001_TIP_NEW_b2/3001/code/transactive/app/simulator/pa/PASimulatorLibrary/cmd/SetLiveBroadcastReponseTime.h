#ifndef SetLiveBroadcastReponseTime_H
#define SetLiveBroadcastReponseTime_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetLiveBroadcastReponseTime.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/12/06 09:29:42 $
  * Last modified by:  $Author: qi.huang $
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
    /** 
      * 
      * 
      */
    class SetLiveBroadcastReponseTime : public LeafCommand
    {
	public:
        SetLiveBroadcastReponseTime(CommandOwner * owner);

        bool makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err);

        void usage(const std::string & name, std::ostream & os) const;
    }; // class SetTableSubCommand


} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef SetLiveBroadcastReponseTime_H
