#ifndef GenericSimulator_H
#define GenericSimulator_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/GenericSimulator.h $
  * @author:  KL Team
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the main controller for the simulation.  It basically controls
  * a bunch of ISimulationModules.  A CommandMonitor controls an instance of this.
  *
  */

#include "bus/generic_server_simulator/src/ISimulationModule.h"
#include <vector>

namespace TA_Base_Bus
{
    typedef std::vector< ISimulationModule * > ModuleContainer;

    class GenericSimulator
    {
        public:
            /** Default Constructor
              */
            GenericSimulator();

            /** Destructor
              */
            ~GenericSimulator();

            /** 
              * Adds the given module to the list owned by this
              *
              * @param module the module to add to this simulator
              */
            void addModule(ISimulationModule * module);

            /** 
              * module removed and ownership transfered back to caller.  Shouldn't be
              * needed unless drastically reconfiguring the simulation...
              *
              * @param module the module to remove
              */
            void removeModule(ISimulationModule * module);


            /** Searches for the module with the given name. 
              *
              * @param  name the name of the module to search for
              * @return pointer to the found ISimulationModule, nil if a module
              *         with the given name could not be found
              */
            ISimulationModule * getModuleByName(const std::string & name);

            /** 
              *
              */
            void startSimulation();

            /** 
              *
              */
            void stopSimulation();

            /** 
              *
              */
            void restartSimulation();

            /** Deletes all simulation modules
              *
              */
            void clearSimulationModules();

            /** accessor for the vector of ISimulationModules owned by this
              *
              * @return const ref to the m_modules vector
              */
            const ModuleContainer & getSimulationModules() const;

        private:
            ///
            ModuleContainer m_modules;

            ///
            bool m_isRunning;
    };

    inline const ModuleContainer & GenericSimulator::getSimulationModules() const
    {
        return m_modules;
    }

}; // namespace

#endif // #ifndef GenericSimulator_H
