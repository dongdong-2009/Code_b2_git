#ifndef ATS_SIM_COMMAND_MONITOR_H
#define ATS_SIM_COMMAND_MONITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimCommandMonitor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */
#ifdef WIN32
#pragma warning(disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_simulator/src/CommandMonitor.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{

    /** 
      * CommandMonitor provides a new thread for the simulator, to listen for
      * and respond to user commands in runtime. ATSSimCommandMonitor extends
      * CommandMonitor by overriding createSimulationModule to return an
      * instance of the ATSSimulator class.
      */
    class ATSSimCommandMonitor : public TA_Base_Bus::CommandMonitor
    {
    public:
        ATSSimCommandMonitor(const std::string& desc, TA_Base_Bus::GenericSimulator *simulator);
        
        virtual ~ATSSimCommandMonitor();

        /** Override this method to create an instance of the derived
          * simulation class
          */
        virtual TA_Base_Bus::ISimulationModule* createSimulationModule(const std::string& type,
                                                                  const std::string& name,
                                                                  const std::string& parameters);
    };
} // namespace ATS_Sim
}

#endif // ifndef ATS_SIM_COMMAND_MONITOR

