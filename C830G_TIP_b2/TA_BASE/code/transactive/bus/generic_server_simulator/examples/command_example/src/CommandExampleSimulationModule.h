#ifndef CommandExampleSimulationModule_H
#define CommandExampleSimulationModule_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/examples/command_example/src/CommandExampleSimulationModule.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for the CommandExampleSimulationModule class
  */


#include "bus/generic_server_simulator/src/GssSimulationModule.h"

namespace TA_Base_Bus
{
	class CommandExampleCommsObserver;

    /**
      * CommandExampleSimulationModule implements GssSimulationModule, for use with the
      * generic server simulator.
      */
    class CommandExampleSimulationModule
		: public GssSimulationModule
    {

    public:
        /// Constructor
        CommandExampleSimulationModule(const std::string &name, const std::string & host, const GssSimulationModule::PortList & ports);

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       GssSimulationModule methods                        //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * createCommsObserver
          * 
          * Used by GssSimulationModule::run() to create a CommsObserver when a
          * new socket connection is accepted.
          *
          * @return the new'd derived GssCommsObserver class instance.
          */
		virtual GssCommsObserver * createCommsObserver( GssCommsObserver::ObservedSocketType * socket );


        /**
          * getSimDescription
          * 
          * returns the description of the simulation module, for help messages.
          *
          * @return the value of the simulatorDescription
          */
        std::string getSimDescription();

    }; // class CommandExampleSimulationModule

} // namespace TA_Base_Bus

#endif // CommandExampleSimulationModule_H
