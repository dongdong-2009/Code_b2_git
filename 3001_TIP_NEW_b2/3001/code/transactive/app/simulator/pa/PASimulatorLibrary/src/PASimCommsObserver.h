#ifndef PA_SIM_COMMS_OBSERVER_H
#define PA_SIM_COMMS_OBSERVER_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimCommsObserver.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"

#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"

namespace TA_IRS_App
{
namespace PA_Sim
{

    class PASimTableQueryProcessor;
    class ModbusQuery;

    /**
      * CommsObserver holds a member observed socket, and acts as an interface for objects
      * to access that socket.
      */
    class PASimCommsObserver : public virtual TA_Base_Core::ITcpSocketObserver
    {
    public:
        // Typedef for classes used
		//
		// The socket type is constructed by TcpObservedSocket, and hence must be
		// of the most derived type that you wish to use.
		//
        typedef TA_Base_Core::TcpNonblockingSocket SocketType;		

        typedef TA_Base_Core::TcpObservedSocket< SocketType > ObservedSocketType;

        typedef TA_Base_Core::TcpServerSocket< ObservedSocketType > ServerSocketType;

        //Construction & Destruction
        
        //we have no need for a default constructor.
        PASimCommsObserver(    ObservedSocketType * socket,
                                LocationType loc,
                                PASimTableQueryProcessor * q_processor);

        PASimCommsObserver();
        
        virtual ~PASimCommsObserver();
        
        
        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                            ITcpObserver methods                            //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////


        /**
          * processReceivedData
          * 
          * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
          * packet is received on the specified socket, this method is called.
          *
          * @param pData the data contained in the received packet
          *
          * @param TA_Base_Core::ISocket* pSocket a pointer to the socket the packet was received on
          *
          */
/*        void processReceivedData( std::vector<unsigned  char> & pData, ACE_SOCK_Stream * pSocket); */
        void processReceivedData( std::vector<unsigned  char> & pData, TA_Base_Core::ISocket * pSocket );

        ModbusQuery * processReceivedRead(std::vector<unsigned char>::iterator & iter);

        ModbusQuery * processReceivedWrite(std::vector<unsigned char>::iterator & iter);


        /**
          * writeFailed
          * 
          * This method must be implemented to inherit ITcpSocketObserver.  Any time a 
          * query to write to the observed socket fails, this method is called.  It allows
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
          *            and returns false if the socket should no longer be observed.
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
        //                                                                            //
        //                            CommsObserver methods                            //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

        
        /**
          * getSocket
          *
          * returns a pointer to the socket being observed.
          *
          * @return TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> socket the observed socket.
          *
          */
        ObservedSocketType * getSocket() { return m_clientSocket; }


        /**
          * stillConnected
          *
          * indicates if the observed socket is still connected to it's remote host.
          *
          * @return bool true if the socket is still connected, false otherwise.
          *
          */
        bool stillConnected() { return m_stillConnected; }

        /** Get the name of the client. Hacked version of TcpSocket function of
          * same name, using functions other than getpeername and gethostbyaddr
          *
          * @return std::string that contains the name or ip address of the
          *         host at the other end of the socket
          */
        std::string getClientName() const;

    private:
        /// the observed socket
        ObservedSocketType * m_clientSocket;

        /// indicates if the observed socket is still connected.    
        bool m_stillConnected;

        /// the location
        LocationType m_location;

        /// Query processor to handle incoming packets.    
        PASimTableQueryProcessor * m_queryProcessor;
    };

} // namespace PA_Sim
}//close namespace
#endif // ifndef(PASim_COMMS_OBSERVER_H)

