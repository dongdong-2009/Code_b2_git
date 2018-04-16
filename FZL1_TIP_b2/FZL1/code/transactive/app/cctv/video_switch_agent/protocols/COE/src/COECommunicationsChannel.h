#if !defined(COE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define COE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include <boost/noncopyable.hpp>


namespace TA_IRS_App
{
    class COECommunicationsHandler;
    class IAgentRequestCommand;
    class ReceivedDataProcessor;
}

namespace TA_IRS_App
{
	class ICOEChannelStateChangeObserver;
    class COECommunicationsChannel : public TA_Base_Core::Thread,
        public virtual TA_Base_Core::ITcpSocketObserver,
        boost::noncopyable
    {
        public:
            enum ECommunicationChannelState
            {
                NOT_CURRENTLY_ESTABLISHED,
                COMMUNICATIONS_FAILURE,
                IDLE_SWITCH_CHANNEL,
                ACTIVE_SWITCH_CHANNEL
            };

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
            COECommunicationsChannel( unsigned int commsChannelIndex,
                                      const std::string& address,
                                      const std::string& port,
                                      unsigned long connectTimeout,
                                      unsigned long maxResponseTime,
                                      unsigned long pollInterval,
                                      ICOEChannelStateChangeObserver& commsHandler,
                                      ReceivedDataProcessor& receivedDataProcessor,
                                      std::string entityName,
                                      unsigned long entityKey,
                                      unsigned long locationKey,
                                      unsigned long entityType,
                                      unsigned long susbSystem,
                                      std::string assetName );

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
              *             Thrown if the agent is not connected to the hardware.
              */
            void sendCommand( IAgentRequestCommand& command );


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
            /**
              * sendSummaryPollingCommand
              *
              * description
              *
              */
            void sendSummaryPollingCommand();


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


            // Flag indicating if when thread method (run) should terminate.

            volatile bool m_running;

            // Index number of this Comms channel.

            unsigned int m_commsChannelIndex;

            //
            // The address and port information to connect to.
            //

            std::string m_address;
            std::string m_port;

            unsigned long m_connectTimeout;
            //
            // The socket to which we are connected.
            // This is an auto_ptr because we are responsible for cleaning it up.
            //

            TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* m_socket;


            unsigned long m_maxResponseTime;

            unsigned long m_pollInterval; // The interval to sleep between polling the channel.

            ICOEChannelStateChangeObserver& m_commsHandler; // Object that manages all th Comms Channels

            ReceivedDataProcessor& m_receivedDataProcessor;

            IAgentRequestCommand* m_commandInProgress;

            std::vector< unsigned char > m_currentReceivedPacket;

            unsigned int m_currentReceivedPacketLength;

            TA_Base_Core::ReEntrantThreadLockable m_socketLock;
            TA_Base_Core::Condition m_commandInProgressCondition;
            // Data for alarm raise
            struct ConnectionLostAlarmRaiser
            {
                ConnectionLostAlarmRaiser( const std::string& entityname,
                                           unsigned long entityKey,
                                           unsigned long locationKey,
                                           unsigned long entityTypeKey,
                                           unsigned long subsystemKey,
                                           std::string& assetName );
                void raiseAlarm( const std::string& videoSwitchAddress );
                void closeAlarm();
                std::string m_entityName;
                unsigned long m_entityKey;
                unsigned long m_locationKey;
                unsigned long m_entityTypeKey;
                unsigned long m_subsystemKey;
                std::string m_assetName;
            };
            ConnectionLostAlarmRaiser m_alarmRaiser;

    }; // COECommunicationsChannel

    class ICOEChannelStateChangeObserver
    {
        public:
            virtual void processCommunicationsChannelState( unsigned int channelIndex,
                                                            COECommunicationsChannel::ECommunicationChannelState state ) = 0;
    };


} // TA_IRS_App

#endif // !defined(CCOE_COMMUNICATIONS_CHANNEL_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
