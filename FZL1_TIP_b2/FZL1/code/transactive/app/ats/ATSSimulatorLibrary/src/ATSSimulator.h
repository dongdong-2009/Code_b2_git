#ifndef ATS_SIM_SIMULATOR_H
#define ATS_SIM_SIMULATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimulator.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for the ATSSimulator class
  */

#include <vector>

#include "bus/generic_simulator/src/ISimulationModule.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <map>

namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimCommsObserver;
    class ATSSimTableQueryProcessor;

    /**
      * ATSSimulator implements ISimulationModule, for use with the generic simulator.
      */
    class ATSSimulator
        : public virtual TA_Base_Core::Thread
        , public TA_Base_Bus::ISimulationModule
    {

    public:
        typedef std::map<LocationType, std::string> PortLocationMap;

        /// vector of comms observers
        typedef std::vector< ATSSimCommsObserver * > CommsObserverContainer;


        // Construction & Destruction
        ATSSimulator(const std::string &name, LocationType location, const char * host, const char * port);
        virtual ~ATSSimulator();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                             Thread methods                               //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * run
          * 
          * The run method must be implemented in any class that inherits from TA_Base_Core::Thread.
            *
          */
        void run();


        /**
          * terminate
          * 
          * The terminate method must be implemented in any class that inherits from TA_Base_Core::Thread.
          *
          */
        void terminate();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                         ISimulationModule methods                        //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * startSim
          * 
          * startSim is called whenever the "start" command is issued by the simulater user.
          * Any initialisation that needs to be taken care of before the simulation starts 
          * for the first time is done in this method.
          *
          */
        void startSim();


        /**
          * stoptSim
          * 
          * stopSim is called whenever the "stop" command is issued by the simulater user.
          * Any processing that needs to be taken care of as the simulation finishes 
          * is done in this method.
          *
          */
        void stopSim();


        /**
          * restartSim
          * 
          * restartSim is called whenever the "start" command is issued by the simulater user.
          * Any initialisation that needs to be taken care of before the simulation restarts 
          * is done in this method.
          *
          */
        void restartSim();


        /**
          * destroySim
          * 
          * startSim is called whenever the "quit" command is issued by the simulater user.
          * Any processing that needs to be taken care of before the simulator is destroyed
          * should be completed in this method.
          *
          */
        void destroySim();    


        /**
          * getName
          * 
          * returns the name that the simulator module was instantiated with.
          *
          * @return std::string Name
          *
          */
        std::string getName();


        /**
          * getSimDescription
          * 
          * returns the description of the simulation module, for help messages.
          *
          * @return std::string simulatorDescription
          *
          */
        std::string getSimDescription();


        /** accessor for this object's CommandProcessor
          * 
          * returns a pointer to the derived ATSSimulatorCommandProcessor
          */
        TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                           ATSSimulator methods                           //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * execStatus
          * 
          * outputs the current status of the PASimulator.
          *
          * @return true if the command is executed sucessfully.
          *
          */
        bool execStatus(std::ostream & streem);


        /** accessor for this object's TableQueryProcessor
          *
          * @return a pointer to the m_queryProcessor data member
          */
        ATSSimTableQueryProcessor * getQueryProcessor();


        /**
          * Accessor for the current format for displaying tables
          *
          * @return the value of the m_queryProcessor->getTableOutputFormat()
          */
        TableOutputFormat getOutputFormat() const;
        
    private:

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       Simulator private methods                          //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * addCommsObserver
          * 
          * adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be added
          *
          */
        void addCommsObserver( ATSSimCommsObserver * com); 


        /**
          * removeCommsObserver
          * 
          * removes the CommsObserver pointed to by the given iterator, from the
          * vector of CommsObserver objects held by the simulator
          *
          * @param iter  the iterator to the CommsObserver object to be removed
          * @return      the iterator returned by vector::erase()
          */
        CommsObserverContainer::iterator removeCommsObserver( CommsObserverContainer::iterator iter );


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       Simulator data members                             //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        // The name of this SimulationModule
        std::string m_name;

        // The ATSSimulationCommandProcessor
        TA_Base_Bus::ISimulationCommandProcessor * m_commandProcessor;

        ///
        volatile bool m_threadIsRunning;

        ///
        volatile bool m_doThreadTerminate;

        /// The location
        LocationType m_location;

        /// The ports listened to. Key'd by location, important for stations
        PortLocationMap m_socketPorts;

        /// The host used by the socket.
        std::string m_socketHost;

        /// a vector which holds an observed socket for each connection that
        /// is accepted.
        CommsObserverContainer m_commsObservers;

        ///
        /// Lock for a ThreadGuard to only allow one thread at a time to
        /// access the comms observers.
        TA_Base_Core::ReEntrantThreadLockable m_observerLock;

        ///
        /// There is a queryProcessor [thread] for each simulationmodule,
        /// shared between all comms observers for that simulator.
        ATSSimTableQueryProcessor * m_queryProcessor;
    };

    inline ATSSimTableQueryProcessor * ATSSimulator::getQueryProcessor()
    {
        return m_queryProcessor;
    }
} // namespace ATS_Sim
}//close namespace

#endif // ATS_SIM_SIMULATOR_H
