#ifndef GENERIC_SIMULATOR_H
#define GENERIC_SIMULATOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_simulator/src/GenericSimulator.h $
  * @author:  KL Team
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is the main controller for the simulation.  It basically controls
  * a bunch of ISimulationModules.  A CommandMonitor controls an instance of this.
  *
  */

#include "ISimulationModule.h"
#include <vector>

namespace TA_Base_Bus
{
	class GenericSimulator
	{
		public:
			/** 
			  * GenericSimulator
			  *
			  * description
			  *
			  */
			GenericSimulator();

			/** 
			  * ~GenericSimulator
			  *
			  * description
			  *
			  */
			~GenericSimulator();

			/** 
			  * addModule
			  *
			  * the module is owned by the GenericSimulator...
			  * RLM TODO make this a ref
			  *
			  * @param *module
			  */
			void addModule(ISimulationModule *module);

			/** 
			  * removeModule
			  *
			  * module removed and ownership transfered back to caller.  Shouldn't be
			  * needed unless drastically reconfiguring the simulation...
			  * RLM TODO make this a ref
			  *
			  * @param *module
			  */
			void removeModule(ISimulationModule *module);


			/** 
			  * getModuleByName
			  *
			  * //RLM TODO make this a ref
			  *
			  * @param name
			  *
			  * @return ISimulationModule*
			  */
			ISimulationModule* getModuleByName(const std::string& name);

			/** 
			  * startSimulation
			  *
			  * simulation control
			  *
			  */
			void startSimulation();

			/** 
			  * stopSimulation
			  *
			  * description
			  *
			  */
			void stopSimulation();

			/** 
			  * restartSimulation
			  *
			  * description
			  *
			  */
			void restartSimulation();

			/** 
			  * getSimulationModules
			  *
			  * description
			  *
			  *
			  * @return std::vector<ISimulationModule *>& 
			  */
			std::vector<ISimulationModule *>& getSimulationModules() {return m_modules;}

		private:
			std::vector<ISimulationModule *> m_modules;
			bool m_isRunning;
	};
}; // namespace

#endif
