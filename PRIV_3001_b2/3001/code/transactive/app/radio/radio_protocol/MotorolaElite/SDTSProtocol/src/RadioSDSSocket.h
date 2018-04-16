/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: 
  * @author:  
  * @version: $Revision:
  *
  * Last modification: $DateTime:
  * Last modified by:  $Author: 
  *
  * This is the socket class that will manage the connection 
  * to the H/W server.  This class works with a observer pattern, 
  * running on its own thread.  It will notify the observer when  
  * the connection is lost or established as well as when there
  * is incoming data.
  */

#if !defined(COE_SOCKET_H)
#define COE_SOCKET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/threads/src/Thread.h"

class ISocketConnectionObserver;

class RadioSDSSocket : public TA_Base_Core::Thread
{
public:
	RadioSDSSocket( const std::string& host,
               const std::string& service,
               ISocketConnectionObserver& observer,
               long socketTimeout = 1000, // in millisecond
               bool tcpNoDelay = true/*false*/ );

    ~RadioSDSSocket();


    /**
      * sendPacket
      * 
      * This is meant for caller to pass the information to be 
      * send to the remote party.  This function will buffer the 
      * message into a internal buffer before sending the 
      * information out.
      * 
      * @return void 
      * @param : const std::vector<unsigned char>& packets
      * 
      * @exception <exceptions> Optional
      */
    void sendPacket(const std::vector<unsigned char>& packets);

    /**
      * run
      * 
      * run() is called by the new thread that is spawned when start() is called.
      * interface required by Thread class
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
	void run();

    /**
      * terminate
      * 
      * terminate() is called by terminateAndWait(), but it can also be called
      * interface required by the Thread class
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
	void terminate();

    /**
      * isConnected
      * 
      * Indicate to the caller if the socket connection is still valid, i.e. connected.
      * Be very careful where isConnected is call.
      * 
      * @return bool 
      * 
      * @exception <exceptions> Optional
      */
    bool isConnected();

protected:
    //Uses a connector component connector_ to connect to a remote machine
    //and pass the connection into a stream component Signalling_stream_

    /**
      * connect
      * 
      * Establish a connection with the remote machine
      * Uses an ACE connector component to connect to a remote machine
      * and pass the connection into a stream component. It will perform a close
      * prior to a connect(...) attempt.
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void connect();

    /**
      * close
      * 
      * Close down the connection properly.
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void close();

    /**
      * sendData
      * 
      * Uses a stream component to send data to the remote host
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void sendData();


    /**
      * getIncomingData
      * 
      * Checks the socket for any incoming information
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void getIncomingData();

    /**
      * notifyConnectionLost
      * 
      * Notify observer when the connection is lost.
      * This will only be called once 
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void notifyConnectionLost();

    /**
      * notifyConnectionEstablished
      * 
      * Notify observer when the connection is established.
      * This will only be called once 
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void notifyConnectionEstablished();

    /**
      * notifyObserverOfIncomingData
      * 
      * Notify observer that there is data available.
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void notifyObserverOfIncomingData();

    /**
      * notifyObserverOfWriteFailed
      * 
      * Notify observer when a sendPacket attempt has failed.
      * This will only be called once 
      * 
      * @return void 
      * 
      * @exception <exceptions> Optional
      */
    void notifyObserverOfWriteFailed(const std::vector<unsigned char>& writeBuffer);

private:


    /**
      * isConnectionLost
      * 
      * Checks the errno to see if the error was caused by a connection lost
      * 
      * @return bool 
      * @param : int state
      * 
      * @exception <exceptions> Optional
      */
    bool isConnectionLost(int state);

private:


	
    /**
      * ACE Stream class that is used to send/receive data from remote machine
      */
    ACE_SOCK_Stream     m_clientStream;

    /**
      * ACE Connector class that performs connect() to the remote host
      */
    ACE_SOCK_Connector  m_connector;

    /**
      * ACE Time object to hold socket-timeout information
      */
    ACE_Time_Value      m_socketTimeout;
    
    /**
      * The interface that is being used to communicate with the observer
      */
    ISocketConnectionObserver& m_observer;
    
    /**
      * Internal buffer for the socket to write incoming information
      */
    char* m_internalDataBuffer;

    /**
      * Should the thread terminates/exit its run function
      */
    bool m_toTerminate;

    /**
      * Is the socket currently connected to the remote machine
      */
    bool m_isConnected;

    /**
      * Is the socket previously connected to the remote machine
      * Used mainly to ensure that the observers will only be called once
      */
    bool m_isPrevConnected;

    /**
      * Buffer to store the data to be passed to observer
      */
    std::vector<unsigned char> m_readBuffer;

    /**
      * Buffer to store data the observer wants to send to remote machine
      */
    std::vector<unsigned char> m_writeBuffer;

    /**
      * Threadguards
      */
    TA_Base_Core::ReEntrantThreadLockable m_writeBufferLock;

	TA_Base_Core::ReEntrantThreadLockable   m_socketLock;
    //TA_Base_Core::NonReEntrantThreadLockable m_readBufferLock;

    int                 m_tcpNoDelay;
    std::string         m_host;
    std::string         m_service;
    
    static const short CONNECTIION_RETRY_TIMER_ID;
    static const short CONNECTIION_RETRY_PERIOD_IN_SEC;

    static const short POLL_READ_TIMER_ID;
    static const short POLL_READ_TIMER_PERIOD_IN_SEC;

    static const short MAX_SLEEP_TIME_IN_MSEC;
    static const int MAX_RECEIVE_BUFFER;

};


#endif // !defined(COE_SOCKET_H)
