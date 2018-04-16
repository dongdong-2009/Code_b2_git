/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/radio/RadioTetraAgent/src/RadioTCPConnection.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioTCPConnection.h
//  Implementation of the Class RadioTCPConnection
//  Generated by Enterprise Architect
//  Created on:      29-Oct-2003 17:46:16
//  Original author: Andy Parker
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#pragma once

//#include "app/radio/RadioTetraAgent/src/RadioTimedEvent.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/ISocketObserver.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

/**
* This class will provide a TCP connection and the monitoring of incoming data
* through a dedicated thread.
* @author Andy Parker
* @version 1.0
* @generator Enterprise Architect
* @created 29-Oct-2003 17:46:16
*/
#include <string>

namespace TA_Base_Core
{
    class ObservedSocket;
}

namespace TA_IRS_App
{
	class IConnectionStatusObserver;
	class IRadioEventProcessor;
    class ITcpMethod;
    class ITcpEvent;
	class RadioTimedEvent;
	class RadioKeepAliveHandlerThread;

    using TA_Base_Core::SocketData;
    using TA_Base_Core::ObservedSocket;

    class TCPConnection : public TA_Base_Core::ISocketObserver
    {
        public:

        /**
        * Constructor
        * @param    IRadioConnectionObserver  
        */
        TCPConnection( ITcpEventProcessor & eventProcessor,
							IConnectionStatusObserver & observer,
                            std::string host,
                            std::string service,
                            bool isPrimary);

        virtual ~TCPConnection();
        
        // Sets if incoming data should be ignored.  Set to true by default.
        void TCPConnection::setIgnoreIncomingData(bool bIgnore);

        void start();

        /**
        * A thread guard must be used to ensure that a complete message is always sent without interruption.
        * @param    method
        */
        void Send(const ITcpMethod& method);

        ////////////////////////////////////////////////////
        //
        // ISocketObserver concrete implementations
        //
        ////////////////////////////////////////////////////
        /**
         *
         */
        virtual void processReceivedData( SocketData& data, ObservedSocket* socket );

        /**
         *
         */
        virtual void writeFailed( const SocketData& writeBuffer, ObservedSocket* socket );

        /**
         *
         */
        virtual void connectionLost( ObservedSocket* socket );

		//for radio server polling 
	    virtual void connectionLost();

        /**
         *
         */
        virtual void connectionEstablished( ObservedSocket* socket );


		void setCheckConnection(){}
        /**
         * Checks if the socket is connected
         *
         * @return true if the socket is connected
         */
        bool isConnected() const;

        /**
         *
         */
        virtual const std::string & getIdentifier() const;
    private:

        ///
        /// the observed socket
        TA_Base_Core::ObservedSocket                      * m_clientSocket;
        ///
        ///
        IConnectionStatusObserver                          & m_observer;
		ITcpEventProcessor							   & m_tcpEventPocessor;
        ///
        /// allows for ignoring the data coming from the socket when we are not active
        volatile bool                                       m_ignoreIncomingData;

        ///
        /// event used as a template for all incoming data to be placed in
        ITcpEvent											* m_event;

        ///
        /// indicator if this connection is to the primary/secondary TcpServer
        volatile bool                                       m_primary;

        ///
        /// a cached copy of the 
        std::string                                         m_identifier;

		volatile bool										isConnectionLost;

		//RadioTimedEvent									*	m_RadioTimedEvent;

	//	RadioKeepAliveHandlerThread						*	m_keepAliveProcessor;

	public:
		inline bool getIgnoreIncomingData()
		{
			return m_ignoreIncomingData;
		}

		inline bool isPrimary()
		{
			return m_primary;
		}
    };
};



