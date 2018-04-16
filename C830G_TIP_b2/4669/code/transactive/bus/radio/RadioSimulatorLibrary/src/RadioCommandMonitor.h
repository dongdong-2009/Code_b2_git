/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioSimulatorLibrary/src/RadioCommandMonitor.h $
  * @author:  Ross Mckillop
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

