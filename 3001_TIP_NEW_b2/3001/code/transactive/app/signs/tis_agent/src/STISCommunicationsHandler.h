#ifndef STISCOMMUNICATIONSHANDLER_H_INCLUDED
#define STISCOMMUNICATIONSHANDLER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISCommunicationsHandler.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/alarm/src/AlarmHelperManager.h"

namespace TA_IRS_App
{
	
	class STISCommunicationsHandler : public TA_Base_Core::ITcpSocketObserver
	{
	public:

		STISCommunicationsHandler();

		~STISCommunicationsHandler();

		void processReceivedData( std::vector<unsigned char>& data, TA_Base_Core::ISocket* socket);
		virtual void writeFailed();
		virtual bool connectionLost();
		virtual void connectionEstablished();

		void sendMessage( const std::vector<unsigned char>& data );

		bool isConnected();

		void connectToServer( const std::string& serverIP, int port );

		void disconnectFromServer();

        void reconnect();

	protected:
		TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* m_clientSocket;	//the observed socket
		TA_Base_Core::ReEntrantThreadLockable    m_writeLock;   // Ensure only one thread writes at a time.
		bool m_connected;   // Is the socket connected to the remote host

		TA_Base_Core::AlarmHelper* m_alarmHelper;
        std::string m_serverIP;
        int m_port;

	};

	class CCTCommunicationsHandler : public STISCommunicationsHandler
	{
	public:
		void sendMessage( const std::vector<unsigned char>& data );
		virtual bool connectionLost();
		virtual void connectionEstablished();
	};

}

#endif
