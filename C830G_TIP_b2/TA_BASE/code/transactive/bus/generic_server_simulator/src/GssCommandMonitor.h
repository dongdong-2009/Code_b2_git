#ifndef GENERIC_SERVER_SIMULATOR_COMMAND_MONITOR_H
#define GENERIC_SERVER_SIMULATOR_COMMAND_MONITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/GssCommandMonitor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#ifdef WIN32
#pragma warning(disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/CommandMonitor.h"

namespace TA_Base_Bus
{

    /** 
      * CommandMonitor provides a new thread for the simulator, to listen for
      * and respond to user commands in runtime. GssCommandMonitor extends
      * CommandMonitor by overriding createSimulationModule to return an
      * instance of the appropriate class, using the GssSimulationModuleFactory.
      */
    class GssCommandMonitor : public CommandMonitor
    {
    public:
        GssCommandMonitor(const std::string& desc, TA_Base_Bus::GenericSimulator *simulator);
        
        virtual ~GssCommandMonitor();

        /** Override this method to create an instance of the derived
          * simulation class
          */
        virtual TA_Base_Bus::ISimulationModule* createSimulationModule(const std::string& type,
                                                                  const std::string& name,
                                                                  const std::string& parameters);
    };
}

#endif // ifndef GENERIC_SERVER_SIMULATOR_COMMAND_MONITOR

