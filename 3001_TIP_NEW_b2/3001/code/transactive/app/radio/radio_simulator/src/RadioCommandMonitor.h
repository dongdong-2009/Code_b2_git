#ifndef SDH_COMMAND_MONITOR_H
#define SDH_COMMAND_MONITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioCommandMonitor.h $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  */
#ifdef WIN32
#pragma warning(disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_simulator/src/CommandMonitor.h"

namespace TA_IRS_App
{
    /** 
      * CommandMonitor provides a new thread for the simulator, to listen for
      * and respond to user commands in runtime. PACommandMonitor extends
      * CommandMonitor by overriding createSimulationModule to return an
      * instance of the PASimulator class.
      */
    class RadioCommandMonitor : public TA_Base_Bus::CommandMonitor
    {
    public:
        RadioCommandMonitor(const std::string& desc, TA_Base_Bus::GenericSimulator *simulator);
        
        virtual ~RadioCommandMonitor();

        /** Override this method to create an instance of the derived
          * simulation class
          */
        virtual TA_Base_Bus::ISimulationModule* createSimulationModule(const std::string& type,
                                                                  const std::string& name,
                                                                  const std::string& parameters);
    };
}

#endif // ifndef SDH_COMMAND_MONITOR_H

