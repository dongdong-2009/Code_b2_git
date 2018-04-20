/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_simulator/src/ISimulationModule.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// ISimulationModule.h
//
// interface for a simulation module.

#ifndef ISIMULATION_MODULE_H
#define ISIMULATION_MODULE_H

#include <string>

namespace TA_Base_Bus
{
	class ISimulationCommandProcessor;

	class ISimulationModule
	{
		public:
			// start the module
			virtual void startSim() = 0;

			// stop (pause) the module
			virtual void stopSim() = 0;

			// restart the module from whatever constitutes "the beginning".  It should
			// reinitialise itself as appropriate.
			virtual void restartSim() = 0;

			// as modules are probably threaded, you don't actually call their destructor.  Instead
			// call destroy() and assume it will self-cleanup.
			virtual void destroySim() = 0;

			// this is an informative one-line string so we can have something meaningful to
			// display in the command monitor.
			virtual std::string getSimDescription() = 0;

			// this is a name for this instance without any spaces
			virtual std::string getName() = 0;

			// returns a reference.  Make sure to create a member ISimulatironCommandProcessor.
			virtual ISimulationCommandProcessor& getCommandProcessor() = 0;

			virtual ~ISimulationModule(){ };

			//this should be implemented in any class that inherits this interface.
			//private:
			//		ISimulationCommandProcessor m_commandProcessor;
	};
}; // Namespace

#endif
