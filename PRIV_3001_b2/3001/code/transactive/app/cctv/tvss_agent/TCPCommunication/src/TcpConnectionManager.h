#pragma once
#include "app/cctv/tvss_agent/TCPCommunication/src/ITcpConnectionStatusObserver.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TCPConnection.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_IRS_App
{

class TcpConnectionManager :public IConnectionStatusObserver
{
	public:

		//TcpConnectionManager(RadioTcpServer& eventProcesser);
		TcpConnectionManager( ITcpEventProcessor& eventProcesser ) ;
		virtual ~TcpConnectionManager();

	public:

		/**
		* This function enables the radio hardware IP address to be defined
        *
        * @param isPrimary flag to indicate if the primary or secondary
        *        connection's address is being set
        * @param hostAddress the host address, in format "12.34.56.78:4000"
		*/
        void setTcpHost(bool isPrimary,std::string hostAddress);

		void setEntity(TA_Base_Core::IEntityData& entityData);

        /**
         *
         */
        void start();

        /**
         *
         */
		void setToControlMode(void);

        /**
         *
         */
		void setToMonitorMode(void);

		/**
		* Sends the content of the radio method to the Radio server via TCP/IP
		* @param    Method
		*/
		void SendMethod(const ITcpMethod& Method);

		/**
		* Change connection from primary to secondary.
		*/
		void switchConnection(bool toPrimary);

        /**
         * Accessor for determining if either of the servers is currently
         * actively connected
         */
        int numConnected();

        /**
         * Clears the SendMethod() if it is stuck in the waiting-for-a-connection state
         */
	    void releaseSendMethod();

		/**
         * This function is only used in monitor mode to hold the connection status of control agent
         */
		void setControlConnection(ETcpControlConnectionStatus controlConnectionStatus);
	
		void getControlConnectionStatus(ETcpControlConnectionStatus& controlConnectionStatus);

		void registForConnectionStatus( ITcpConnectionStatusObserver* observer);

		void unregistForConnectionStatus( ITcpConnectionStatusObserver* observer);

		virtual void onConnectionFail(bool isPrimary);
	
		virtual void onConnectionResumed(bool isPrimary);

		virtual void onCloseSocket();
		TCPConnection * getCurrentConnection();
	private:

        ///
        /// why a private accessor for a data member you ask ?
        /// apparently using "volatile" on pointers to UDT's results in 
        /// unusable pointers. If a fix is found, then by all means use it
        

        bool waitForConnection();

		void notifyConnectionSwitch();

        ///
        /// vector of primary servers
		TCPConnection            * m_primaryConnection;

        ///
        /// vector of secondary servers
		TCPConnection            * m_secondaryConnection;

        ///
        /// the callback for notifying of connection failure/resume and on
        /// receiving a radio event

        ///
        /// Indicator for if this agent is in control/monitor mode
		volatile bool					m_active;

        /// 
        /// locks the m_currentConnection so that it isn't switched while sending
        TA_Base_Core::ReEntrantThreadLockable m_serverGuard;

		ITcpEventProcessor				  & m_tcpServer;

        ///
        TCPConnection            * m_currentConnection;
        ///
        ///
	    volatile bool                   m_waitForConnection;

		std::vector< ITcpConnectionStatusObserver* > m_observers;

		TA_Base_Core::ReEntrantThreadLockable m_observerGuard;

		TA_Base_Core::ReEntrantThreadLockable m_connectionSwitchGuard;

		std::string         m_objectName;
		unsigned long       m_entityKey;
		unsigned long   	m_entityLocation;
		unsigned long		m_entitySubsystem;
		unsigned long		m_entityTypeKey;
		std::string			m_assetName;

		//use for hot standby
		volatile ETcpControlConnectionStatus m_controlConnectionStatus;		

		volatile bool m_isConnected;
};
};
