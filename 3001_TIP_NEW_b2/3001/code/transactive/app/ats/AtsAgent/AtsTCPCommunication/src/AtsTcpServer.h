#pragma once

#include "app/ats/AtsAgent/AtsTCPCommunication/src/ITcpConnectionStatusObserver.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/TcpTransactionManager.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/TcpConnectionManager.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpMethod.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include <string>

namespace TA_IRS_App
{
	class AtsTcpServer :  public ITcpConnectionStatusObserver,
					   public ITcpEventProcessor
	{
	public:
		AtsTcpServer(const std::string & primaryAddress,
                        const std::string & secondaryAddress);
		AtsTcpServer(){};
		void setEntity(TA_Base_Core::IEntityData& entityData);

		//the interface for IRadioConnectionStatusObserver
		virtual void onConnectionEstablish();
		virtual void onConnectionLost();
		virtual void onSwitchConnection();

		void start();

		/**
		* Activates this server class and the connectionmanager
		*/
		void setToControlMode(void);

		/**
		* De-activates this server class and the connectionmanager
		*/
		void setToMonitorMode(void);

        void onTerminate(bool peerAvailable); 

	   /**
		* This method will receive the incoming events from the radio system.  It will notify waiting RadioSession of a new event through the RadioTransactionManager.  It will process all unsolited events.
		* @param    event    event
		*/
		virtual void onReceiveEvent(TA_IRS_App::AtsITcpEvent& atsitcpEvent);

	   /**
		* createCompatibleTcpEvent. Create a TcpEvent that is compatible with this type of observer 
		* @param    isPrimary
		*/
		virtual AtsITcpEvent * createCompatibleTcpEvent();

			/**
		* This method sends a method request to the radio system and waits for a reply (event).  It will be used by the RadioSession to perform its operations.
		*
		* @param    Method
		*/
		//boost::shared_ptr<AtsEvent> sendMethod(AtsITcpMethod& Method);
		void sendMethod(AtsITcpMethod& Method);

		   /**
         *
         */
		virtual void resetConnection(){}

		/**
         *
         */
		virtual void resetSocket(){}

		 private:

		/**
		 * Handler for unsolicited events (e.g. incoming message)
		 * @param    event
		 */
			 //TBD
		void onUnsolicitedEvent(AtsEvent& event);
		/**
		 * Handler for solicited events - This will send them to the RadioTransactionManager for processing
		 * @param    event
		 */
			 //TBD
		void onSolicitedEvent(AtsEvent& event);

			//TBD
		void onUnhandledSolicitedEvent(AtsEvent& event);

		void updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus );


	private:
        ///
        /// transaction manager for this server
		TcpTransactionManager               * m_transactionManager;

        ///
        /// connection manager for this server
		TcpConnectionManager                * m_connectionManager;

		//Event Handler
		std::vector<unsigned char> EventHandlerData;
	public:
		virtual ~AtsTcpServer(void);
	};
} ;
