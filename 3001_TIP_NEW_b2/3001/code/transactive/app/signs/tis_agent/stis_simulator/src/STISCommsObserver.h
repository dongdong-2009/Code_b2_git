#ifndef STIS_COMMS_OBSERVER_H
#define STIS_COMMS_OBSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISCommsObserver.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * CommsObserver holds a member observed socket, and acts as an interface for objects
  * to access that socket.
  */

#pragma warning (disable : 4786)

#include <vector>

#include "app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.h"

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
//#include "app/signs/tis_agent/stis_simulator/src/STISSimulator.h"

namespace TA_IRS_App
{
   	//TD9161
	enum EResponseType
    {
        POSITIVE_RESPONSE,
        NEGATIVE_RESPONSE,
        NO_RESPONSES
    };



	class STISCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver
	{
	public:

		//Construction & Destruction
		
		//we have no need for a default constructor.
		STISCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket, const std::string& location, EResponseType ratisResponseType, EResponseType displayRequestResponseYype);
		STISCommsObserver();
		
		virtual ~STISCommsObserver();
		
		
		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//							ITcpObserver methods							//
		//																			//
		//////////////////////////////////////////////////////////////////////////////


		/**
		  * processReceivedData
		  * 
		  * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
		  * packet is received on the specified socket, this method is called.
		  *
		  * @param std::vector<unsigned char>& pData the data contained in the received packet
		  *
		  * @param TA_Base_Core::ISocket* pSocket a pointer to the socket the packet was received on
		  *
		  */
		void processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket );


		/**
		  * writeFailed
		  * 
		  * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
		  * request to write to the observed socket fails, this method is called.  It allows
		  * the user to include any cleanup they require after a failed write.
		  *
		  */
		void writeFailed();


		/**
		  * connectionLost
		  * 
		  * This method must be implemented to inherit ITcpSocketObserver.  This method is called
		  * whenever the observed socket loses it's connection to the remote socket.
		  *
		  * @return bool continueToObserve returns true if the socket should continue to be observed
		  *			and returns false if the socket should no longer be observed.
		  *
		  */
		bool connectionLost();


		/**
		  * connectionEstablished
		  *
		  * This method must be implemented to inherit ITcpSocketObserver. it is called after a
		  * connection is successfully established.
		  *
		  */
		void connectionEstablished();


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//							CommsObserver methods							//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

		
		/**
		  * getSocket
		  *
		  * returns a pointer to the socket being observed.
		  *
		  * @return TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> socket the observed socket.
		  *
		  */
		TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* getSocket() { return m_clientSocket; }


		/**
		  * stillConnected
		  *
		  * indicates if the observed socket is still connected to it's remote host.
		  *
		  * @return bool true if the socket is still connected, false otherwise.
		  *
		  */
		bool stillConnected() { return m_stillConnected; }

		void updatePIDStatus( int pidAddress, int status );

		void updateResourceStatus( const std::string& resourceName, int status );

		void updateLibraryVersion( const std::string& libraryType, unsigned long version );

		void updateCurrentSTISLibraryVersion( unsigned long version );

		void updateCurrentTTISLibraryVersion( unsigned long verison );

		void displayResourceStatus();

    	void sendRATISPacketToStation( const std::string& command );

    	void sendRATISPacketToTrain( const std::string& command );

    	void sendRATISPacketToAllStations( const std::string& command );

    	void sendRATISPacketToAllTrains( const std::string& command );

		//TD9161
        void setRATISResponseType( EResponseType ratisResponseType) { m_ratisResponseType = ratisResponseType; }
       
        void setDisplayRequestResponseType( EResponseType displayRequestResponseType) { m_displayRequestResponseType = displayRequestResponseType; }
        
        EResponseType getRATISResponseType(){ return m_ratisResponseType; }
        
        EResponseType getDisplayRequestResponseType(){ return m_displayRequestResponseType; }
        
	private:

		TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* m_clientSocket;	//the observed socket

		bool m_stillConnected;											//indicates if the observed socket
																		//is still connected.	
		TA_IRS_App::STISMessageProcessor* m_stisMessageProcessor;					//Request processor to handle incoming packets.	

        //TD9161
        EResponseType m_ratisResponseType;
        EResponseType m_displayRequestResponseType;

		//TD 8191
		//zhou yuan++
		TA_Base_Core::ReEntrantThreadLockable m_incomingMessageLock;
		//++zhou yuan

	};

}//close namespace
#endif // !defined(AFX_COMMSOBSERVER_H__1C6ACB32_AD6D_4753_9B81_AE8A14B4CF20__INCLUDED_)

