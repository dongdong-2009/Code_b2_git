#pragma once
#include "app/cctv/tvss_agent/TCPCommunication/src/ITcpConnectionStatusObserver.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEvent.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSCommandResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainborneEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentResponseEventReader.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpTransactionManager.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpConnectionManager.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/ITCPMessageProcessor.h"
#include <string>

namespace TA_IRS_App
{

class BaseTcpServer :  public ITcpConnectionStatusObserver,
					   public ITcpEventProcessor
{
public:
	BaseTcpServer(ITCPMessageProcessor* processor, const std::string & primaryAddress,
                        const std::string & secondaryAddress,unsigned long timeout);
	BaseTcpServer(){};
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
		boost::shared_ptr<TVSSEvent> sendMethod(TVSSMethod& Method, unsigned long transactionID = 0xFFFF, bool waitReply = true);

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
		void onUnsolicitedEvent(TVSSEvent& event);
		/**
		 * Handler for solicited events - This will send them to the RadioTransactionManager for processing
		 * @param    event
		 */
		void onSolicitedEvent(TVSSEvent& event);

		void onUnhandledSolicitedEvent(TVSSEvent& event);

		void updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus );

		/**
		 * IncomingCommandEventReader Event handler.
		 */
		void onIncomingCommandEventReader(TA_IRS_App::TVSSCommandEventReader& eventReader);
		
		/**
		 * IncomingTrainborneEventReader Event handler.
		 */
		void onIncomingTrainborneEventReader(TA_IRS_App::TVSSTrainborneEventReader& eventReader);

		/**
		 * IncomingEquipmentAlarmResponse Event handler.
		 */
		void onIncomingEquipmentAlarmResponse(TA_IRS_App::TVSSEquipmentResponseEventReader& eventReader);

     private:
        ///
        /// transaction manager for this server
		TcpTransactionManager               * m_transactionManager;

        ///
        /// connection manager for this server
		TcpConnectionManager                * m_connectionManager;

		//Event Handler
		std::vector<unsigned char> EventHandlerData;
		ITCPMessageProcessor* m_processor;
		unsigned long m_timeout;
public:
	virtual ~BaseTcpServer(void);
};
} ;
