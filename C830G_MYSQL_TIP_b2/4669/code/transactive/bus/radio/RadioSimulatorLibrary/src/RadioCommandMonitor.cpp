/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioSimulatorLibrary/src/RadioCommandMonitor.cpp $
  * @author:  Ross Mckillop
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Implementation file for the CommandMonitor class.  
  * CommandMonitor provides functionality to constantly listen for
  * user input from the command line, and to process that input
  * and execute the appropriate commands.
  */

#include "bus/radio/RadioSimulatorLibrary/src/RadioCommandMonitor.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulator.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

namespace TA_IRS_Bus
{

    RadioCommandMonitor::RadioCommandMonitor( const std::string& desc,
                                              TA_Base_Bus::GenericSimulator *simulator )
            : TA_Base_Bus::CommandMonitor( desc, simulator )
    {
        FUNCTION_ENTRY( "RadioCommandMonitor" );
        FUNCTION_EXIT;
    }


    RadioCommandMonitor::~RadioCommandMonitor()
    {
        FUNCTION_ENTRY( "~RadioCommandMonitor" );
        FUNCTION_EXIT;
    }


    TA_Base_Bus::ISimulationModule* RadioCommandMonitor::createSimulationModule( const std::string& type,
                                                                                 const std::string& name,
                                                                                 const std::string& parameters )
    {
        FUNCTION_ENTRY( "createSimulationModule" );

        if ( type == "radio" )
        {
            char host[100];
            char port[100];

            // Get the host and port
            int numTokens = sscanf( parameters.c_str(), "%s %s", host, port );

            if ( numTokens == 2 )
            {
                FUNCTION_EXIT;
                return new TA_IRS_Bus::RadioSimulator( name, host, port );
            }
            else
            {
                CERR( "Incorrect parameters for radio simulator module." << name << std::endl
                      << "addmod <name> radio <host> <port>" << std::endl );
            }
        }

        FUNCTION_EXIT;
        return NULL;
    }

} // TA_IRS_Bus

