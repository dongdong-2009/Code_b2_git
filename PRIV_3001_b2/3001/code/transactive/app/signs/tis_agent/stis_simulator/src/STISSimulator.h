#ifndef STIS_SIMULATOR_H
#define STIS_SIMULATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISSimulator.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * STISSimulator implements ISimulationModule, for use with the generic simulator.
  */

#include <vector>

#include "bus/generic_server_simulator/src/ISimulationModule.h"
#include "app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.h"
#include "app/signs/tis_agent/stis_simulator/src/STISCommsObserver.h"

#include "core/threads/src/Thread.h"
#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"


namespace TA_IRS_App
{


	class STISSimulator : public virtual TA_Base_Core::Thread, public TA_Base_Bus::ISimulationModule
	{

	public:

		//Construction & Destruction
		STISSimulator(const std::string &name,const char* port, const std::string& location);
		virtual ~STISSimulator();


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//								Thread methods								//
		//																			//
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
		//																			//
		//							ISimulationModule methods						//
		//																			//
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
		  * getCommandProcessor
		  * 
		  * returns a pointer to the 
		  *
		  */
		TA_Base_Bus::ISimulationCommandProcessor& getCommandProcessor();


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//								Execute methods								//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

        /**
		  * execSendPacket
		  * 
		  * forms a packet from the supplied string, and sends it.  This method uses no error checking
          * of the packet, so a response is never guaranteed.  It does however allow the user to test
          * responses to a wide variety of packets
          *
          * @param  std::string params  The parameters passed that were given with the command
          *
		  * @return bool successful True if the command is executed sucessfully.
		  *
		  */
        bool execSendPacket( const std::string& params );

		/**
		  * execStatus
		  * 
		  * outputs the current status of the VirtualVideoSwitch.  This method is 
		  * executed when 
		  *
		  * @return bool successful True if the command is executed sucessfully.
		  *
		  */
		bool execStatus();

        /**
		  * pidStatusChange
		  * 
		  * Changes the status of the specified PID to the specified status
          *
		  * @return bool successful True if the command is executed sucessfully.
		  *
		  */
		bool pidStatusChange( const std::string& params );

		bool resourceStatusChange( const std::string& params );

		bool newStisLibrary( const std::string& params );

		bool newTtisLibrary( const std::string& params );

		bool currentStisLibrary( const std::string& params );

		bool currentTtisLibrary( const std::string& params );

		bool displayResourceStatus();

    	bool sendRATISPacketToStation( const std::string& command );

    	bool sendRATISPacketToTrain( const std::string& command );

    	bool sendRATISPacketToAllStations( const std::string& command );

    	bool sendRATISPacketToAllTrains( const std::string& command );

		//TD9161
		bool setRATISResponseType( const std::string& params);

		//TD9161
		bool setDisplayRequestResponseType( const std::string& params);
 		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//						 Simulator public methods							//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

	

		
	private:

		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//						 Simulator private methods							//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

		/**
		  * getCommsObserverVector
		  * 
		  * returns the vector of CommsObserver objects held by this class.  Each CommsObserver
		  * monitors a connected client socket.
		  *
		  * @return std::vector< CommsObserver* >& commsObservers The vector of CommsObserver objects
		  *					held by the Simulator
		  *
		  */
		std::vector< TA_IRS_App::STISCommsObserver* >& getCommsObserverVector();


		/**
		  * addCommsObserver
		  * 
		  * adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
		  * simulator
		  *
		  * @param CommsObserver* com A pointer to the CommsObserver object to be added
		  *
		  */
		void addCommsObserver( TA_IRS_App::STISCommsObserver* com); 


		//TD 8191
		//zhou yuan++
		/**
		  * removeCommsObserver
		  * 
		  * removes the passed in CommsObserver from the vector of CommsObserver objects held by the 
		  * simulator
		  *
		  * @return std::vector<STISCommsObserver*>::iterator 
		  * @param : std::vector<STISCommsObserver*>::iterator iter
		  * 
		  * @exception <exceptions> Optional
		  */
		std::vector<STISCommsObserver*>::iterator removeCommsObserver( std::vector<STISCommsObserver*>::iterator iter );
		//++zhou yuan

		std::string m_name;					//the name this module was instantiated with.
		TA_Base_Bus::ISimulationCommandProcessor* m_commandProcessor;

		volatile bool m_threadIsRunning;
		volatile bool m_doThreadTerminate;

		//socket member variables.
		volatile bool m_isConnected;
		std::string m_socketPort;
		std::string m_socketHost;

		std::string m_location;

		std::vector< TA_IRS_App::STISCommsObserver* > m_commsObserverVector;			//a vector to hold an observed socket for
																//each connection that is accepted.
        
		TA_IRS_App::STISMessageProcessor* m_stisMessageProcessor;

        //TD9161
        EResponseType m_ratisResponseType;
        EResponseType m_displayRequestResponseType;

	};

}//close namespace

#endif // STIS_SIMULATOR_H
