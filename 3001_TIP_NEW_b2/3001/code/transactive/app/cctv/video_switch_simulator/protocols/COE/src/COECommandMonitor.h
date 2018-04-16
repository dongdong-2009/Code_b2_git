#if !defined(AFX_COECOMMANDMONITOR_H_INCLUDED_)
#define AFX_COECOMMANDMONITOR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COECommandMonitor.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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

#include "bus/generic_server_simulator/src/CommandMonitor.h"
#include <map>

namespace TA_IRS_App
{
    class COECommandMonitor : public TA_Base_Bus::CommandMonitor  
    {
        public:
            COECommandMonitor(const std::string& desc, TA_Base_Bus::GenericSimulator *simulator);
            virtual ~COECommandMonitor();

            // Override this method to create the supported simulation classes
            virtual TA_Base_Bus::ISimulationModule* createSimulationModule(const std::string& type,
                                                              const std::string& name,
                                                              const std::string& parameters);
    };
}; // Namespace

#endif // !defined(AFX_COECOMMANDMONITOR_H_INCLUDED_)

