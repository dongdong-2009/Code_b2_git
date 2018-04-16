/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COECommandMonitor.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/05/14 15:47:54 $
  * Last modified by:  $Author: builder $
  *
  * Implementation file for the CommandMonitor class.  
  * CommandMonitor provides functionality to constantly listen for
  * user input from the command line, and to process that input
  * and execute the appropriate commands.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "app/cctv/video_switch_simulator/protocols/COE/src/COECommandMonitor.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.h"
#include <iostream>

using namespace TA_IRS_App;


/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/

COECommandMonitor::COECommandMonitor(const std::string &desc, TA_Base_Bus::GenericSimulator *simulator)
                    : CommandMonitor(desc,simulator)
{

}

COECommandMonitor::~COECommandMonitor()
{
}


TA_Base_Bus::ISimulationModule* COECommandMonitor::createSimulationModule(const std::string& type,
                                                               const std::string& name, const std::string& parameters)
{
    if ( type == "COE" )
    {
        char    location[100];
        char    host[100];
        char    port[100];
        char    mode[100];

        // Get the host and port
        int numTokens = sscanf( parameters.c_str(), "%s %s %s %s", mode, location, host, port );

        if ( ( numTokens == 4 ) && ( ( mode[0] == 'a' ) || ( mode[0] == 's' ) ) )
        {
            bool isActive = ( mode[0] == 'a' );
            return new TA_IRS_App::COESimulator( name, location, host, port, isActive );
        }
        else
        {
            std::cout << "Incorrect parameters for COE Video Switch simulator module." << name << std::endl;
            std::cout << "addmod <name> COE <active | standby> <location> <host> <port>" << std::endl;
        }
    }

    return NULL;
}


void coeSimulationModuleRemoved( const std::string& locationOfModuleRemoved )
{

}

