#if !defined(COE_SIMULATOR_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_)
#define COE_SIMULATOR_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * COESimulator implements ISimulationModule, for use with the generic simulator.
  *
  */

#include <vector>
#include "bus/generic_server_simulator/src/ISimulationModule.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

#include "app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h"

namespace TA_IRS_App
{
    class ISimulationCommandProcessor;
    class COECommsObserver;

    class COESimulator : public virtual TA_Base_Core::Thread, public TA_Base_Bus::ISimulationModule
    {

    public:

        //Construction & Destruction
        COESimulator( const std::string& name, const char * location, const char * host, const char * port, bool isActive );
        virtual ~COESimulator();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                              Thread methods                              //
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
        //                          ISimulationModule methods                       //
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


        /** 
          * getLocation
          *
          * description
          *
          *
          * @return 
          */
        std::string getLocation()
        {
            return m_location;
        }


        /**
          * getCommandProcessor
          * 
          * returns a pointer to the 
          *
          */
        TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                              Execute methods                             //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * execStatus
          * 
          * outputs the current status of the VirtualVideoSwitch.  This method is 
          * executed when 
          *
          * @return bool successful True if the command is executed sucessfully.
          */
        bool execStatus();

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       Simulator public methods                           //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

    
        /** 
          * updateObserversWithMonitorState
          *
          * description
          *
          * @param monitorID
          * @param 
          */
        void updateObserversWithMonitorState( unsigned char monitorID, const COEEquipmentState::MonitorState& state );


        /** 
          * updateObserversWithQuadState
          *
          * description
          *
          * @param monitorID
          * @param 
          */
        void updateObserversWithQuadState( unsigned char quadID, const COEEquipmentState::QuadState& state );


        /** 
          * updateObserversWithSequenceState
          *
          * description
          *
          * @param sqeuenceID
          * @param 
          */
        void updateObserversWithSequenceState( unsigned char sequenceID, const COEEquipmentState::SequenceState& state );


        /** 
          * updateObserversWithRecordingUnitState
          *
          * description
          *
          * @param recordingUnitID
          * @param 
          */
        void updateObserversWithRecordingUnitState( unsigned char recordingUnitID, COEEquipmentState::ERecordingUnitState state );


        /** 
          * updateObserversWithSwitchStatus
          *
          * description
          *
          * @param recordingUnitID
          * @param 
          */
        void updateObserversWithSwitchStatus( bool isActive );

        /** 
          * updateObserversWithTriggeringEvents
          *
          * description
          *
          * @param TriggeringEventList
          * @param 
          */
        void updateObserversWithTriggeringEvents( TriggeringEventList& events );

        
    private:

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       Simulator private methods                          //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * getCommsObserverVector
          * 
          * returns the vector of CommsObserver objects held by this class.  Each CommsObserver
          * monitors a connected client socket.
          *
          * @return std::vector< CommsObserver* >& commsObservers The vector of CommsObserver objects
          *                 held by the Simulator
          *
          */
        std::vector< COECommsObserver* >& getCommsObserverVector();


        /**
          * addCommsObserver
          * 
          * adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be added
          *
          */
        void addCommsObserver(COECommsObserver* com); 


        /**
          * removeCommsObserver
          * 
          * removes the passed in CommsObserver from the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be removed
          *
          */
        void removeCommsObserver( COECommsObserver* com );

        /**
          * removeAllCommsObservers
          * 
          * removes all the CommsObserver from the vector of CommsObserver objects held by the 
          * simulator
          *
          * @param None
          *
          */
        void removeAllCommsObservers();


        std::string m_name;                 //the name this module was instantiated with.
        TA_Base_Bus::ISimulationCommandProcessor* m_commandProcessor;

        volatile bool m_threadIsRunning;
        volatile bool m_doThreadTerminate;

        //socket member variables.
        volatile bool m_isConnected;

        std::string m_location;
        std::string m_socketHost; /// The host used by the socket.
        std::string m_socketPort;


        bool m_isActive;

        std::vector< COECommsObserver* > m_commsObserverVector; /// a vector to hold an observed socket for each connection that is accepted.
        TA_Base_Core::ReEntrantThreadLockable m_observersLock; // Semaphore to thread guard the observers.

    };

}//close namespace

#endif // !defined(COE_SIMULATOR_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_)