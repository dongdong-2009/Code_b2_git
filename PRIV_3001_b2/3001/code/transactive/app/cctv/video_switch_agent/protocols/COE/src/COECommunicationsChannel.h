#if !defined(COE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define COE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>
#include <memory>


namespace TA_IRS_App
{
    class COECommunicationsHandler;
    class IAgentRequestCommand;
    class ReceivedDataProcessor;
}

namespace TA_IRS_App
{
    class COECommunicationsChannel : public TA_Base_Core::Thread, public virtual TA_Base_Core::ITcpSocketObserver
    {
    public: 
        
        /** 
          * COECommunicationsChannel
          *
          * description
          *
          * @param commsChannelIndex
          * @param address
          * @param port
          * @param maxResponseTime
          * @param pollInterval
          * @param commsHandler
          * @param 
          *
          */
        COECommunicationsChannel( unsigned int commsChannelIndex, const std::string& address, const std::string& port, unsigned long connectTimeout, unsigned long maxResponseTime, 
			unsigned long pollInterval, unsigned long maxSummaryPollingResponseTime, unsigned long summaryPollingInterval, COECommunicationsHandler& commsHandler, ReceivedDataProcessor& receivedDataProcessor, 
			std::string entityName, unsigned long entityKey, unsigned long locationKey, unsigned long entityType, unsigned long susbSystem, std::string assetName );

        /**
          * ~COECommunicationsChannel
          *
          * Standard destructor.
          */
        virtual ~COECommunicationsChannel();


        /**
          * sendCommand
          *
          * Adds the command to the vector of commands ready to be sent to 
          * the Video Switch.
          *
          * @param      IAgentRequestCommand& command
          *             The command to be sent.  Once the command
          *             has been processed it will be deleted.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if the agent is not connected to the hardware.
          */
        void sendCommand( IAgentRequestCommand& command, unsigned long maxResponseTime = 0);
        

        /**
          * connect
          *
          * Connects to the switching hardware. Once connected this will
          * poll the state of the switch.
          */
        void activateChannel();

        
        /**
          * disconnect
          *
          * Disconnects from the switching hardware.
          */
        void deactivateChannel();


        /** 
          * isChannelActive
          *
          * description
          *
          *
          * @return 
          */
        bool isChannelActive();


        ///////////////////////////////////////////////////////////////////////
        //
        // Thread Methods
        //
        ///////////////////////////////////////////////////////////////////////
        
		/**
		  * run()
		  *
		  * Continually processes requests from the agent and responses from
          * the hardware. Commands from the received queue have priority over 
          * the request queue.
		  */
		virtual void run();


		/**
		  * terminate()
		  *
		  * Signals the run() loop to complete.  This method should not be called
          * directly.  Clients should call TA_Base_Core::Thread::terminateAndWait().
		  */
		virtual void terminate();

        ///////////////////////////////////////////////////////////////////////
        //
        // ITcpSocketObserver Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
		  * processReceivedData()
		  *
		  * Inherited from ITcpSocketObserver. This function is called when incoming data is 
		  * detected by the socket thread. The received data is queued until it
          * can be processed.
		  *
          * @param      std::vector< unsigned char >& pData
          *             The received data.
          * @param      TA_Base_Core::ISocket* pSocket
          *             The socket from which the data is received.
		  */			
        virtual void processReceivedData( std::vector<unsigned char>& pData, 
            TA_Base_Core::ISocket* pSocket );


		/**
		  * writeFailed()
		  *
		  * Inherited from ITcpSocketObserver. This function is called when a socket write
		  * operation fails.
		  */
        virtual void writeFailed();


		/**
		 * connectionLost()
		 *
		 * Inherited from ITcpSocketObserver. This function is called when a break in the
		 * the connection has been detected. An alarm is raised and the comms status 
         * set to DISCONNECTED.
		 */	
        virtual bool connectionLost();


		/**
		 * connectionEstablished()
		 *
		 * Inherited from ITcpSocketObserver. This function is called when connection with the
		 * hardware has been established by the socket thread. If there is an outstanding
		 * comms alarm, it is closed. Comms status is set to CONNECTED.  
         * Will request the state of all devices from the hardware.
		 */		
        virtual void connectionEstablished();	

    private:
    
        //
        // Disable copy constructor and assignment operator.
        //

        COECommunicationsChannel( const COECommunicationsChannel& theCOECommunicationsChannel );
        COECommunicationsChannel& operator=( const COECommunicationsChannel& );


        /** 
          * createAndConnectSocket
          *
          * description
          *
          */
        void createAndConnectSocket();


        /** 
          * ensureSocketClosed
          *
          * description
          *
          */
        void ensureSocketDisconnected();


        /** 
          * ensureSocketHealthy
          *
          * description
          *
          */
        void ensureSocketHealthy();


        /** 
          * sendSummaryPollingCommand
          *
          * description
          *
          */
        void sendSummaryPollingCommand();

		
		/** 
          * sendHeartbeatCommand
          *
          * description
          *
          */
        void sendHeartbeatCommand();


        /** 
          * invalidPacketHeaderPacketReceived
          *
          * description
          *
          */
        void invalidPacketHeaderReceived();


        /** 
          * validPacketReceived
          *
          * description
          *
          */
        void validPacketReceived();



        //
        // private members
        //

        //
        // some constants this class uses
        //
        static const unsigned long WAIT_FOR_OBSERVED_SOCKET_CONNECT_ATTEMPT_INTERVAL;
        static const unsigned long DELAY_AFTER_SUCCESSFUL_SOCKET_CONNECT;

        //
        // The socket to which we are connected.
        // This is an auto_ptr because we are responsible for cleaning it up.
        //
        
        std::auto_ptr< TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > > m_socket;	

        // Flag indicating if when thread method (run) should terminate.
        
        volatile bool m_doTerminate;

        //
        // The current status of communications.
        //
    
        volatile bool m_isConnected;

        volatile bool m_socketHasAttemptedConnection;

        volatile bool m_previousWriteFailed;

        // Index number of this Comms channel.

        unsigned int m_commsChannelIndex;

        //
        // The address and port information to connect to.
        // 

		std::string	m_address;
		std::string	m_port;

        unsigned long m_connectTimeout;

        unsigned long m_maxResponseTime;

        unsigned long m_pollInterval; // The interval to sleep between polling the channel.

		unsigned long m_maxSummaryPollingResponseTime;		// for command 0x3A

		unsigned long m_summaryPollingInterval; // The interval to sleep between polling the channel for command 0x3A.

        COECommunicationsHandler& m_commsHandler; // Object that manages all th Comms Channels

        ReceivedDataProcessor& m_receivedDataProcessor;

        IAgentRequestCommand* m_commandInProgress;

        std::vector< unsigned char > m_currentReceivedPacket;

        unsigned int m_currentReceivedPacketLength;

        TA_Base_Core::ReEntrantThreadLockable m_channelUsageLock;
        TA_Base_Core::ReEntrantThreadLockable m_threadStateLock;

        TA_Base_Core::Condition m_commandInProgressCondition;
		std::string m_entityName;
		unsigned long m_entityKey;
        unsigned long m_locationKey;
        unsigned long m_entityTypeKey;
		unsigned long m_subsystemKey;
		std::string m_assetName;
		int m_heartBeatCommandFailureCount;
        
    }; // COECommunicationsChannel

} // TA_IRS_App
    
#endif // !defined(CCOE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
