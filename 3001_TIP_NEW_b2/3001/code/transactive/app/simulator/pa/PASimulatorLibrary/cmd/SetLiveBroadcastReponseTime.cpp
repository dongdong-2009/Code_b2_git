/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/cmd/SetLiveBroadcastReponseTime.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for PASimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/cmd/SetLiveBroadcastReponseTime.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/LiveAnnouncementScenario.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace TA_IRS_App::PA_Sim;

TA_IRS_App::PA_Sim::SetLiveBroadcastReponseTime::SetLiveBroadcastReponseTime( CommandOwner * owner )
: LeafCommand(owner, "LRT")
{

}

bool TA_IRS_App::PA_Sim::SetLiveBroadcastReponseTime::makeQuery( TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err )
{
	int interval = 0;
	if ( !args.empty())
	{
		interval = atoi(args.front().c_str());
		LiveAnnouncementScenario::setLiveBroadcastReponseTime(interval);
	}
	return true;
}

void TA_IRS_App::PA_Sim::SetLiveBroadcastReponseTime::usage( const std::string & name, std::ostream & os ) const
{
	os << "LRT(LiveBroadcast Reponse Time) <Time=Second> " << std::endl;
}
