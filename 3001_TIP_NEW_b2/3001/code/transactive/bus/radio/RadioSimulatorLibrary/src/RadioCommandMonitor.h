/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioCommandMonitor.h $
  * @author:  Ross Mckillop
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * CommandMonitor provides a new thread for the simulator, to listen for and 
  * respond to user commands in runtime.
  */

#if !defined(RadioCommandMonitor_H)
#define RadioCommandMonitor_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_server_simulator/src/CommandMonitor.h"

#include "core/defaults/src/DefaultEnvironment.h"

namespace TA_IRS_Bus
{
    class RadioCommandMonitor : public TA_Base_Bus::CommandMonitor  
    {
    public:

        /**
         * Constructor
         */
        RadioCommandMonitor( const std::string& desc,
                             TA_Base_Bus::GenericSimulator *simulator );
        
        
        /**
         * Destructor
         */
        virtual ~RadioCommandMonitor();


        /**
         * Overridden from generic simulator
         * This creates radio specific simulation modules.
         */
        virtual TA_Base_Bus::ISimulationModule* createSimulationModule( const std::string& type,
                                                                        const std::string& name,
                                                                        const std::string& parameters );
    };
}; // Namespace

#endif // !defined(RadioCommandMonitor_H)

