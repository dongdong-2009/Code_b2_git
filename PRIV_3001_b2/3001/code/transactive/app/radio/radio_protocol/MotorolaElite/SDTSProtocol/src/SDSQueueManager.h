/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the state of the interface to the SDS Host.
  * Rate limiting calculations/strategies are also encapsulated within this class
  */


#ifndef __SDS_QueueManager_H
#define __SDS_QueueManager_H

//#include "core/sockets/src/TcpObservedSocket.h"
//#include "core/sockets/src/ITcpSocketObserver.h"
//#include "core/sockets/src/TcpSocket.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/ISocketConnectionObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/RadioSDSSocket.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventSubject.h"

#include "ace/OS.h"

namespace TA_IRS_App {
    class SDSInputQueue;
    class SDSOutputQueue;
    class SDTSTransactionManager;

	class SDSQueueManager: public ISocketConnectionObserver, public virtual SDSEventSubject
	{
		public:

	        /**
	          * Constructor
	          */
			SDSQueueManager(SDTSTransactionManager& transactionManager);

	        /**
	          * Destructor
	          */
			virtual ~SDSQueueManager();

            void setQueues(SDSInputQueue* input, SDSOutputQueue* output);
            
	        /**
	          * Called by the output queue to limit the rate of sending SDTS
              * messages, so as not to exceed capacity of the SDTS Host.
              * If there was activity within the message interval (either send
              * or recv), a (configuration-based) delay is performed before
              * returning
	          */
			void rateLimit();

	        /**
	          * Received an incoming SDTS event. Update last event timestamp
	          */
			void updateResponseTimestamp();

	        /**
	          * Sending an SDTS event. Update last event timestamp
	          */
			void updateRequestTimestamp();

	        /**
	          * Establish socket connection to SDR
	          */
            void connect(const std::string& host, const std::string& port);

	        /**
	          * Terminate socket connection with SDR
	          */
			void disconnect();

	        /**
	          * Check the socket connection state
	          */
            bool isConnected() const;

            // ITcpSocketObserver methods
//			virtual void processReceivedData( std::vector<unsigned char> & pData,
//                                 			  TA_Base_Core::ISocket * pSocket );
//
//			virtual void writeFailed();
//
//			virtual bool connectionLost();
//
//			virtual void connectionEstablished();

			///////////////////////////////////////////////////////////////////////
			//
			// ISocketConnectionObserver Methods
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
			virtual void processReceivedData( std::vector<unsigned char>& pData );
			
			
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
			virtual void connectionLost();
			
			
			/**
			* connectionEstablished()
			*
			* Inherited from ITcpSocketObserver. This function is called when connection with the
			* hardware has been established by the socket thread. If there is an outstanding
			* comms alarm, it is closed. Comms status is set to CONNECTED.  
			* Will request the state of all devices from the hardware.
			*/		
			virtual void connectionEstablished();	


			void sendMessage(const std::vector<unsigned char>& reqMsg);

			void setPeriod(const ACE_Time_Value& period);

			virtual void registerObserver(SDSEventObserver* evtObj);

			virtual void deregisterObserver(SDSEventObserver* evtObj);

		private:

			void notifyConnectionStatus();
					
		private:
			
	        /**
	          * Last time an SDS function was actually called
	          */
			ACE_Time_Value m_lastRequestTimestamp;

	        /**
	          * Last time an SDS message was received
	          */
			ACE_Time_Value m_lastResponseTimestamp;

	        /**
	          * Rate limiting period
	          */
			ACE_Time_Value m_sdtsPeriod;

	        /**
	          * Last time a socket data was received. Used to ensure complete
              * data is received within an acceptable time period.
	          */
			ACE_Time_Value m_lastReceivedDataTimestamp;

			//TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>*	m_Socket;	//the observed socket

			RadioSDSSocket* m_Socket;
			
            std::vector<unsigned char> m_receivedData;

			//provides a thread lock
			TA_Base_Core::ReEntrantThreadLockable m_lock;
			TA_Base_Core::ReEntrantThreadLockable m_socketLock;

			TA_Base_Core::ReEntrantThreadLockable m_socketLock_Rev;  //for receiving data

			bool m_Connected;
			//bool m_isReconnect;

            SDSInputQueue* m_inputQueue;
            SDSOutputQueue* m_outputQueue;

            SDTSTransactionManager& m_transactionManager;

			std::vector<SDSEventObserver*> m_observers;

        public:

	        /**
	          * If packets are broken in batches, the data must come within
              * this time period or it will be considered invalid.
	          */
            static const unsigned int MAX_RECEIVED_DATA_DELAY_IN_SECS;
	};

};
	
#endif //__SDS_QueueManager_H
