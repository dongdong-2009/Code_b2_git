/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioSimulator.h $
 * @author:  Andy Parker
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/01/24 17:57:59 $
 * Last modified by:  $Author: CM $
 *
 * RadioSimulator implements ISimulationModule, for use with the generic simulator.
 */

#ifndef RadioSimulator_H
#define RadioSimulator_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h"

#include "bus/generic_simulator/src/ISimulationModule.h"

#include "core/defaults/src/DefaultEnvironment.h"
#include "core/threads/src/Thread.h"

#include <vector>
#include <map>

namespace TA_IRS_Bus
{
    class TA_Base_Bus::ISimulationCommandProcessor;

    class RadioCommsObserver;

    class SimulatorUseCase;
}

namespace TA_IRS_Bus
{

    class RadioSimulator : public virtual TA_Base_Core::Thread,
                           public TA_Base_Bus::ISimulationModule,
                           public TA_IRS_Bus::ISimulatorSender
    {

    public:

        /**
         * Constructor
         */
        RadioSimulator( const std::string & name,
                        const char * host,
                        const char * port );

        /**
         * Destructor
         */
        virtual ~RadioSimulator();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                              Thread methods                              //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
         * Does the processing.
         */
        void run();


        /**
         * Signals the thread to stop
         */
        void terminate();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                        ISimulationModule methods                         //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
         * startSim
         *
         * startSim is called whenever the "start" command is issued by the simulater user.
         * Any initialisation that needs to be taken care of before the simulation starts
         * for the first time is done in this method.
         */
        void startSim();


        /**
         * stoptSim
         *
         * stopSim is called whenever the "stop" command is issued by the simulater user.
         * Any processing that needs to be taken care of as the simulation finishes
         * is done in this method.
         */
        void stopSim();


        /**
         * restartSim
         *
         * restartSim is called whenever the "start" command is issued by the simulater user.
         * Any initialisation that needs to be taken care of before the simulation restarts
         * is done in this method.
         */
        void restartSim();


        /**
         * destroySim
         *
         * destroySim is called whenever the "quit" command is issued by the simulater user.
         * Any processing that needs to be taken care of before the simulator is destroyed
         * should be completed in this method.
         */
        void destroySim();


        /**
         * getName
         *
         * returns the name that the simulator module was instantiated with.
         *
         * @return std::string Name
         */
        std::string getName();


        /**
         * getSimDescription
         *
         * returns the description of the simulation module, for help messages.
         *
         * @return std::string simulatorDescription
         */
        std::string getSimDescription();


        /**
         * getCommandProcessor
         *
         * returns a pointer to the object handling user commands
         */
        TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                            Execute methods                               //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        
        /**
         * execStatus
         *
         * Displays status of simulator module
         *
         * @return bool successful True if the command is executed sucessfully.
         */
        bool execStatus();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                        Simulator public methods                          //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////
        
        
        /**
         * newCall
         *
         * Generates and sends a new call
         *
         * @return bool successful True if the command is executed sucessfully.
         */
        bool newCall( std::string parameters );

        
        /**
         * sendSDSData
         *
         * Sends an SDS message to the Radio Agent
         *
         * @return bool successful True if the command is executed sucessfully.
         */
        bool sendSDSData( std::string parameters );

        
        /**
         * Caculates train checksums
         */
        bool calcChecksumForSDSData( std::string parameters );

        
        /**
         * Loads simulation use cases
         */
        bool simulatorLoadTable( std::string parameters );
        
        /**
         * Loads simulation use cases
         */
        bool simulatorClearTable( std::string parameters );
        
        
        /**
         * Prints simulation use cases
         */
        bool simulatorShowTable( std::string parameters );
        
        
        /**
         * Runs a simulation
         */
        bool simulatorRun( std::string parameters );
        
        
        /**
         * Runs a simulation use case
         */
        void simulatorRunUseCase( const SimulatorUseCase& uc );

        
        /**
         * Lists the connected radio sessions
         */
        bool showSessions();

    private:

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                          Simulator private methods                       //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        
        /**
         * getCommsObserverVector
         *
         * returns the vector of CommsObserver objects held by this class.  Each CommsObserver
         * monitors a connected client socket.
         *
         * @return std::vector< CommsObserver* >& commsObservers The vector of CommsObserver objects
         *     held by the Simulator
         */
        std::vector< RadioCommsObserver* >& getCommsObserverVector();


        /**
         * addCommsObserver
         *
         * adds the passed in CommsObserver to the vector of CommsObserver objects held by the
         * simulator
         *
         * @param CommsObserver* com A pointer to the CommsObserver object to be added
         */
        void addCommsObserver( RadioCommsObserver* com );


        /**
         * removeCommsObserver
         *
         * removes the disconnected CommsObservers from the vector of CommsObserver objects held by the
         * simulator
         */
        void removeDisconnectedCommsObserver();

        /**
         * removeAllCommsObservers
         *
         * removes all the CommsObserver from the vector of CommsObserver objects held by the
         * simulator
         */
        void removeAllCommsObservers();



        /**
         * sendFromAllCommsObserver
         *
         * send data to each observered socket
         */
        void sendFromAllCommsObserver( const char * pBuffer,
                                       const unsigned int dataLength );

        /**
         * Implemented from ISimulatorSender
         */
        void simulatorSendEvent( const char * buf,
                                 const unsigned int length );

        
        /**
         * Create a generic message header
         */
        void setMessageHeader( char* buffer,
                               unsigned long  messageLength,
                               unsigned long  sessionId,
                               unsigned long  transactionId,
                               unsigned long  status,
                               unsigned long  event );

        /**
         * Write a long into the message header
         */
        void setMessage( char* buffer,
                         unsigned long data );

        /**
         * Write a short into the message header
         */
        void setMessage( char* buffer,
                         short data );

        /**
         * Write a string into the message header
         */
        void setMessage( char* buffer,
                         const char* data );


        /**
         * the name this module was instantiated with.
         */
        std::string m_name;


        /**
         * Socket configuration
         */
        std::string m_socketHost;
        std::string m_socketPort;


        /**
         * the command processor
         */
        TA_Base_Bus::ISimulationCommandProcessor* m_commandProcessor;


        /**
         * Whether the thread is running
         */
        volatile bool m_threadIsRunning;


        /**
         * Whether the thread should terminate
         */
        volatile bool m_doThreadTerminate;


        /**
         * a vector to hold an observed socket for each connection that is accepted.
         */
        std::vector< RadioCommsObserver* > m_commsObserverVector;
    };

} // TA_IRS_Bus

#endif // RadioSimulator_H
