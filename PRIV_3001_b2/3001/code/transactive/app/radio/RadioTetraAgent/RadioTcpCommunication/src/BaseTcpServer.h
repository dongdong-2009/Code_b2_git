#pragma once

#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/ITcpConnectionStatusObserver.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/ITcpMethod.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEvent.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/TcpTransactionManager.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/TcpConnectionManager.h"

//Event
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventDynamicGroup.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCallFowardRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCancelCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventQueryCallForwarding.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include <string>

namespace TA_IRS_App
{

class BaseTcpServer :  public ITcpConnectionStatusObserver,
					   public ITcpEventProcessor
{
public:
	BaseTcpServer( ITcpConnectionStatusObserver* commandServant, const std::string & primaryAddress,
                        const std::string & secondaryAddress,unsigned long timeout = 500 );
	BaseTcpServer(){};
	void setEntity(TA_Base_Core::IEntityData& entityData);

	//the interface for IRadioConnectionStatusObserver
		virtual void onConnectionEstablish(bool isPrimary);
		virtual void onConnectionLost();
		virtual void onSwitchConnection(bool toPrimary);

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
		virtual void onReceiveEvent(TA_IRS_App::ITcpEvent& itcpEvent);

			/**
		* createCompatibleTcpEvent. Create a TcpEvent that is compatible with this type of observer 
		* @param    isPrimary
		*/
		virtual ITcpEvent * createCompatibleTcpEvent();

			/**
		* This method sends a method request to the radio system and waits for a reply (event).  It will be used by the RadioSession to perform its operations.
		*
		* @param    Method
		*/
		boost::shared_ptr<RadioEvent> sendMethod(RadioMethod& Method);

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
		void onUnsolicitedEvent(RadioEvent& event);
		/**
		 * Handler for solicited events - This will send them to the RadioTransactionManager for processing
		 * @param    event
		 */
		void onSolicitedEvent(RadioEvent& event);

		void onUnhandledSolicitedEvent(RadioEvent& event);

		void updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus );

		/**
		 * Handle the incoming event for the MFT monitoring profile request.
		 */
		void onIncomingEventProfileRequest(TA_IRS_App::RadioEventProfileRequest& eventReader);
		
		/**
		 * Handle the incoming event for the Subscriber Monitoring
		 */
		void onIncomingEventSubscriberMonitor(TA_IRS_App::RadioEventSubscriberMonitor& eventReader);

		/**
		 * Handle the incoming event for the Dynamic Group Number Assignment
		 */
		void onIncomingEventDynamicGroup(TA_IRS_App::RadioEventDynamicGroup& eventReader);

		/**
		 * Handle the incoming event for the Call Forward Request
		 */
		void onIncomingEventCallFowardRequest(TA_IRS_App::RadioEventCallForwardRequest& eventReader);

		/**
		 * Handle the incoming event for the Cancel Call Forwarding
		 */
		void onIncomingEventCancelCallForwarding(TA_IRS_App::RadioEventCancelCallForwarding& eventReader);

		/**
		 * Handle the incoming event for the Query Call Forwarding
		 */
		void onIncomingEventQueryCallForwarding(TA_IRS_App::RadioEventQueryCallForwarding& eventReader);

		

     private:
        ///
        /// transaction manager for this server
		TcpTransactionManager               * m_transactionManager;

        ///
        /// connection manager for this server
		TcpConnectionManager                * m_connectionManager;

		//Event Handler
		std::vector<unsigned char> EventHandlerData;
		//ITCPMessageProcessor* m_processor;

		ITcpConnectionStatusObserver		* m_commandServant;
public:
	virtual ~BaseTcpServer(void);
};
} ;
