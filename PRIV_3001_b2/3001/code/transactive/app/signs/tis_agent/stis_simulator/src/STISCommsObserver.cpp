/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISCommsObserver.cpp $
  * @author:  Ross Mckillop
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * CommsObserver allows the CommsHandler to delegate incoming socket connections.
  * Each new connection is passed to a new CommsObserver object, a vector of which
  * is held by the WattsSim singleton object.
  */

#pragma warning (disable : 4786)

#include <iostream>
#include <algorithm>
#include <sstream>

#include "app/signs/tis_agent/stis_simulator/src/STISCommsObserver.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace
{
	void logReceivedData(const std::vector<unsigned char>& data)
	{
		std::stringstream ss;
		ss << "START@";
		for(std::vector<unsigned char>::const_iterator ite = data.begin();
			ite != data.end();
			++ite)
		{
			ss << (*ite);
		}
		ss << "@END";
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str());
	}
}

namespace TA_IRS_App
{
//#include <boost/tokenizer.hpp>

	//////////////////////////////////////////////////////////////////////////////
	//			Construction/Destruction
	//////////////////////////////////////////////////////////////////////////////

	STISCommsObserver::STISCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket, const std::string& location, EResponseType ratisResponseType, EResponseType displayRequestResponseType)
	:			m_clientSocket(socket),
				m_stillConnected(true),
				m_stisMessageProcessor(0),
                m_ratisResponseType(ratisResponseType),                         //TD9161
                m_displayRequestResponseType(displayRequestResponseType)        //TD9161
	{
		m_stisMessageProcessor = new TA_IRS_App::STISMessageProcessor( this );
		m_stisMessageProcessor->initialise( location );

		m_clientSocket->addObserver(* this);
		
		m_clientSocket->start();
	}

	STISCommsObserver::~STISCommsObserver()
	{
		FUNCTION_ENTRY("~STISCommsObserver");
		if( 0 != m_clientSocket )
		{
			//terminate the thread the socket is holding.
			m_clientSocket->terminate();
			delete m_clientSocket;
			m_clientSocket = 0;
		}

		if( 0 != m_stisMessageProcessor )
		{
			delete m_stisMessageProcessor;
			m_stisMessageProcessor = 0;
		}
		FUNCTION_EXIT;
	}



	//////////////////////////////////////////////////////////////////////////////
	//																			//
	//							ITcpObserver methods							//
	//																			//
	//////////////////////////////////////////////////////////////////////////////

	//
	//processReceivedData
	//
	void STISCommsObserver::processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket )
	{
		//TD 8191
		//zhou yuan++
		TA_Base_Core::ThreadGuard guard(m_incomingMessageLock);
		//++zhou yuan

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Data received" );
		logReceivedData(pData);

		std::vector<unsigned char>::iterator stx(std::find(pData.begin(), pData.end(), 0x02));
		while( stx != pData.end() )
		{
			std::vector<unsigned char>::iterator etb(std::find(stx, pData.end(), 0x03));
			
			//TD 8191
			//zhou yuan++
			if(etb == pData.end() )
			{
				break;
			}
			//++zhou yuan

			std::vector<unsigned char> data(stx, etb + 1);
        
			pData.erase( pData.begin(), etb + 1 );

			std::stringstream packetInfoStream;

			////////////////////////////////////////////
			packetInfoStream << "\nPacket Received: ";

			std::vector< unsigned char>::iterator x = data.begin();
			for(; x != data.end(); ++x )
			{
				 //std::cout << (int)(*x) << " ";
				packetInfoStream << (*x);
			}
			////////////////////////////////////////////

			std::string packetInfo = packetInfoStream.str();

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, packetInfo.c_str() );

			m_stisMessageProcessor->addMessageRequest(data);	

			stx = std::find(pData.begin(), pData.end(), 0x02);
		}   
	}


	//
	//writeFailed
	//
	void STISCommsObserver::writeFailed()
	{
			//should probably throw some sort of exception.
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Write failed!" );
	}


	//
	//connectionLost
	//
	bool STISCommsObserver::connectionLost()
	{
		m_stillConnected = false;

		return true;
		//the TcpObservedSocket run() method checks this value to see 
		//if it should try to reconnect.
		//set the socket pointer to null.
	}



	//
	//connectionEstablished
	//
	void STISCommsObserver::connectionEstablished()
	{
		m_stillConnected = true;
	//        VirtualVideoSwitch::getInstance().incrementConnections();
	}

	void STISCommsObserver::updatePIDStatus( int pidAddress, int status )
	{
		m_stisMessageProcessor->updatePIDStatus( pidAddress, status );	
	}

	void STISCommsObserver::updateResourceStatus( const std::string& resourceName, int status )
	{
		m_stisMessageProcessor->updateResourceStatus( resourceName, status );
	}

	void STISCommsObserver::updateLibraryVersion( const std::string& libraryType, unsigned long version )
	{
		m_stisMessageProcessor->updateNextLibraryVersion( libraryType, version );
	}

	void STISCommsObserver::updateCurrentSTISLibraryVersion( unsigned long version )
	{
		m_stisMessageProcessor->updateCurrentSTISLibraryVersion( version );
	}

	void STISCommsObserver::updateCurrentTTISLibraryVersion( unsigned long version )
	{
		m_stisMessageProcessor->updateCurrentTTISLibraryVersion( version );
	}

	void STISCommsObserver::displayResourceStatus()
	{
		m_stisMessageProcessor->displayResourceStatus();
	}

	void STISCommsObserver::sendRATISPacketToStation( const std::string& command )
	{
		m_stisMessageProcessor->sendRATISPacketToStation(command);
	}

	void STISCommsObserver::sendRATISPacketToTrain( const std::string& command )
	{
		m_stisMessageProcessor->sendRATISPacketToTrain(command);
	}

	void STISCommsObserver::sendRATISPacketToAllStations( const std::string& command )
	{
		m_stisMessageProcessor->sendRATISPacketToAllStations(command);
	}

	void STISCommsObserver::sendRATISPacketToAllTrains( const std::string& command )
	{
		m_stisMessageProcessor->sendRATISPacketToAllTrains(command);
	}


}
