/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Robin Ashcroft
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include "app/signs/tis_agent/src/STISCommunicationsHandler.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/types/TISAlarms_MessageTypes.h"


namespace TA_IRS_App
{

	void STISCommunicationsHandler::connectionEstablished()
	{	
		FUNCTION_ENTRY("connectionEstablished");

		m_connected = true;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Connection established with STIS server" );

		STISManager::getInstance()->closeAlarm( TA_Base_Core::TISAlarms::STISServerCommFailure );

		FUNCTION_EXIT;
	}

	bool STISCommunicationsHandler::connectionLost()
	{
		FUNCTION_ENTRY("connectionLost");

		m_connected = false;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Lost connection to STIS server" );

		TA_Base_Core::DescriptionParameters dp;

		TA_Base_Core::NameValuePair nvp("Location", STISManager::getInstance()->getLocationName() );

		dp.push_back( &nvp );

		STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::STISServerCommFailure, dp );

		// Set the state of the datapoints for the following two objects to Bad 
		// this ensures the schematics display the state of the associated equipment as 'unknown'
		StatusMonitor::getInstance()->setBadDataPointQuality();

		// Only set the PID datapoints to bad quality if not at the OCC
		// as the OCC 'points' do not have real TransActive datapoint objects
		if( STISManager::getInstance()->getLocationType() != TA_Base_Core::ILocation::OCC)
		{
			STISManager::getInstance()->setBadDataPointQuality();
		}

 		FUNCTION_EXIT;
		return true;  //return TRUE because we want the socket to reattempt connection
	}

	void STISCommunicationsHandler::processReceivedData( std::vector<unsigned char>& pData, TA_Base_Core::ISocket* socket)
	{
		FUNCTION_ENTRY("processReceivedData");

		std::vector<unsigned char>::iterator stx(std::find(pData.begin(), pData.end(), 0x02));
		while( stx != pData.end() )
		{
			std::vector<unsigned char>::iterator etb(std::find(stx, pData.end(), 0x03));
			std::vector<unsigned char> data(stx, etb + 1);
       		
			std::stringstream pack;
			
			pack << "Packet Received: ";
			std::vector< unsigned char>::iterator x = data.begin();
			for(; x != data.end(); ++x )
			{
				 //std::cout << (int)(*x) << " ";
				pack << (*x);
			}
			
			pData.erase( pData.begin(), etb + 1 );

			////////////////////////////////////////////

			////////////////////////////////////////////

			std::string packStr = pack.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, packStr.c_str() );

			STISManager::getInstance()->processResponseMessage( data );

			stx = std::find(pData.begin(), pData.end(), 0x02);
		}   
		
		FUNCTION_EXIT;
	}

	void STISCommunicationsHandler::sendMessage( const std::vector<unsigned char>& data )
	{
		FUNCTION_ENTRY("sendMessage");

        TA_Base_Core::ThreadGuard guard( m_writeLock );

        char* packet = ( char* ) &(data[0]);

		if( m_connected )
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Writing data to socket");

		    m_clientSocket->write( packet, data.size() ); 
		}
		else
		{
			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		

		FUNCTION_EXIT;
	}

	bool STISCommunicationsHandler::isConnected()
	{
		FUNCTION_ENTRY("isConnected");

		FUNCTION_EXIT;

		return m_connected;
	}

	void STISCommunicationsHandler::connectToServer( const std::string& serverIP, int port )
	{
		FUNCTION_ENTRY("connectToServer");
		
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to connect to STIS server");

        TA_ASSERT(m_clientSocket == NULL, "Trying to connect to server when a socket already exists.");

		std::stringstream serverPort;

		serverPort << port;

        m_serverIP = serverIP;
        m_port = port;
		m_clientSocket = new TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>( serverIP,serverPort.str() );

		TA_ASSERT(m_clientSocket!=NULL,"Unable to create client socket.");

		m_clientSocket->addObserver(*this);

		try
		{
			m_clientSocket->start();
		}
		catch ( const TA_Base_Core::TcpSocketException& se )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Exception caught while attempting to start socket thread:");
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, se.what() );
		}

		// Block until connected
/*		while ( !m_connected && !m_agentShuttingDown )
		{
			TA_Base_Core::Thread::sleep(1000);
		}  */

		FUNCTION_EXIT;
	}


	void STISCommunicationsHandler::writeFailed()
	{
		FUNCTION_ENTRY("writeFailed");

		FUNCTION_EXIT;
	}

	void STISCommunicationsHandler::disconnectFromServer()
	{
		FUNCTION_ENTRY("disconnectFromServer");

		if( m_clientSocket != NULL )
        {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to disconnect from STIS server");

		    m_clientSocket->terminateAndWait();
			m_clientSocket->removeObserver(*this);

            m_connected = false;

            // delete the socket as a new one will be created in the next call to connect
            delete m_clientSocket;
            m_clientSocket = NULL;

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Disconnected from STIS server");
        }
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Not connected to STIS server, disconnection not attempted");
		}

		FUNCTION_EXIT;
	}

	STISCommunicationsHandler::STISCommunicationsHandler() : m_connected( false ), m_clientSocket( NULL )
	{

	}

	STISCommunicationsHandler::~STISCommunicationsHandler()
	{
		if(m_clientSocket)
		{
			delete m_clientSocket;
		}
	}



    void STISCommunicationsHandler::reconnect()
    {
        FUNCTION_ENTRY("reconnect");

        TA_ASSERT(m_clientSocket!=NULL, "Not connected to server.");
        
        disconnectFromServer();

        connectionLost();

        connectToServer( m_serverIP, m_port );

        FUNCTION_EXIT;
    }
}