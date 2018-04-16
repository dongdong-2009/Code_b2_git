
#if defined(_MSC_VER)
	#pragma warning(disable:4018)	// signed/unsigned mismatch
#endif // defined _MSC_VER

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#if !defined( WIN32 )
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

//
// Headers not available under FreeBSD.
//
#if !defined( FREEBSD )
#include <stropts.h>
#endif // !defined( FREEBSD )

#else // defined( WIN32 )
#include <winsock2.h>
#include <windows.h>

#include <io.h>
#endif //  defined( WIN32 )
#include <signal.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <ctime>
#include <exception>
#include "UdpSocket.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{


    UdpSocket::UdpSocket (  const std::string & host,              // Host running service
                            const std::string & service,           // Required service
                            const bool pBlocking,
                            unsigned long readingTimeout)          // How long in milliseconds for readong selects to timeout and throw failure exception?
        : socketId( -1 ),
          isBlocking( pBlocking ),
          m_isDisconnected( true )
    {
        
        m_readingTimeout.tv_sec  = readingTimeout / 1000;
        m_readingTimeout.tv_usec = 1000 * (readingTimeout % 1000);

        //
        // MS Visual C++ has its own exception code that we must capture.
        //
        try
        {

            #if defined( WIN32 )

            //
            // It is necessary to initialise winsock before using any of its
            // features, so we will do that here.  Of course this is only
            // necessary under "brain dead" operating systems.
            //
            int lvError = WSAStartup( MAKEWORD( 2, 2 ), & winsockData );
            if (0 != lvError)
            {
                    TA_THROW( TA_Base_Core::TcpSocketException( "Couldn't initialise Windows sockets" ) );
            }

            #endif // !WIN32

            //
            // Get the host details.
            //
            struct in_addr lvHostAddress;
            getIpAddress( & lvHostAddress, host );

            //
            // Get the service details.
            //
            struct servent lvService;
            getServicePort( & lvService, service );
            serviceName = service;

            //
            // Create the socket.
            //
            socketId = socket( AF_INET, SOCK_DGRAM, 0 );
            if ( 0 > socketId )
            {
                std::string lvMsg( "Unable to create socket " );
                lvMsg += host;
                lvMsg += ":";
                lvMsg += service;
                lvMsg += ".  Reason: ";
                lvMsg += strerror( errno );
                    TA_THROW( TA_Base_Core::TcpSocketException( lvMsg ) );
            }

            //
            // Set up socket name for future connection.
            //
            memset( & socketHandle, 0, sizeof( socketHandle ) );
            socketHandle.sin_family = AF_INET;
            socketHandle.sin_port = lvService.s_port;
            //socketHandle.sin_port = htons((u_short)20248);
			socketHandle.sin_addr = lvHostAddress;
			
			char *optval2 = "1";
			int optval;

			setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, optval2, sizeof optval);
			
			int nBind = bind(socketId,(sockaddr*)&socketHandle,sizeof(socketHandle));

			std::stringstream ss;
			ss << "Created socket to host " << host << " on service port " << service;
			LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str() );
        }

        //
        // Windows exceptions.
        //
        catch ( unsigned int )
        {
                TA_THROW( TA_Base_Core::TcpSocketException( "Unknown Microsoft exception caught" ) );
        }
    }

    //*****************************************************************************
    //
    // Method: TcpSocket
    //
    // Purpose:
    //
    // Returns:
    //
    //     nothing
    //
    // Arguments:
    //
    //     const std::string & service      - name of the published service
    //
    //*****************************************************************************
    UdpSocket::UdpSocket (  const std::string & service,           // Required service
                            const bool pBlocking,                  // Blocking socket?
                  
                            unsigned long readingTimeout)          // How long for readong selects to timeout and throw failure exception?

        : socketId( -1 ),
          isBlocking( pBlocking ),
          m_isDisconnected( true )
    {
  
        m_readingTimeout.tv_sec  = readingTimeout / 1000;
        m_readingTimeout.tv_usec = readingTimeout % 1000;
        //
        // MS Visual C++ has its own exception code that we must capture.
        //
        try
        {

            #if defined( WIN32 )

            //
            // It is necessary to initialise winsock before using any of its
            // features, so we will do that here.  Of course this is only
            // necessary under "brain dead" operating systems.
            //
            int lvError = WSAStartup( MAKEWORD( 2, 2 ), & winsockData );
            if (0 != lvError)
            {
                    TA_THROW( TA_Base_Core::TcpSocketException( "Couldn't initialise Windows sockets" ) );
            }

            #endif // !WIN32

            //
            // Get the service details.
            //
            struct servent lvService;
            getServicePort( & lvService, service );
            serviceName = service;

            //
            // Create the socket.
            //
            socketId = socket( AF_INET, SOCK_DGRAM, 0 );
            if ( 0 > socketId )
            {
                std::string lvMsg( "Unable to create server socket " );
                lvMsg += service;
                lvMsg += ".  Reason: ";
                lvMsg += strerror( errno );
                    TA_THROW( TA_Base_Core::TcpSocketException( lvMsg ) );
            }

            //
            // Set up socket name for future connection.
            //
            memset( & socketHandle, 0, sizeof( socketHandle ) );
            socketHandle.sin_family = AF_INET;
            socketHandle.sin_port = lvService.s_port;
            socketHandle.sin_addr.s_addr = INADDR_ANY;
			

            //
            // Set the blocking/non-blocking flags.  In the Windows world we
            // force the socket into the correct blocking/non-blocking mode as
            // it seems to default to non-blocking.  In the UNIX world the
            // socket is blocking and we switch it when necessary.
            //
            #if !defined( WIN32 )
            if ( false == pBlocking )
            {
                 fcntl( socketId, F_SETFL, O_NONBLOCK );
            }
            #else // defined( WIN32 )
            const unsigned int lvNonBlocking = pBlocking ? 0 : 1;
            ioctlsocket( socketId, FIONBIO, ( unsigned long * )& lvNonBlocking );
            #endif // defined( WIN32 )
            //
            // Don't allow the socket to linger on close.
            //
            struct linger lvLingerOpts;
            lvLingerOpts.l_onoff = 0;
            lvLingerOpts.l_linger = 0;
            int nRet = setsockopt( socketId, 0, SO_LINGER, ( char * ) & lvLingerOpts, sizeof( lvLingerOpts ) );
			
			if (nRet != 0)
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Error On setsockopt, %d",
					WSAGetLastError());
			}

                LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                      TA_Base_Core::DebugUtil::DebugDebug, 
                  "Created server socket on service port %s", service.c_str() );
        }

        //
        // Windows exceptions.
        //
        catch ( unsigned int )
        {
                TA_THROW( TA_Base_Core::TcpSocketException( "Unknown Microsoft exception caught" ) );
        }
    }

    //*****************************************************************************
    //
    // Method: ~TcpSocket
    //
    // Purpose:
    //
    //     This method closes a socket connection and then destroys the channel.
    //
    // Returns:
    //
    //     nothing
    //
    // Arguments:
    //
    //     none
    //
    // History:
    //
    // Date      Author          Reason
    // ====      ======          ======
    //
    // 27/11/97  Geoff Glasson   Original version
    //
    //*****************************************************************************
    UdpSocket::~UdpSocket ()
    {
        close();

        #if defined( WIN32 )
        WSACleanup();
        #endif // !WIN32
    }

    void UdpSocket::close()
    {

        try
        {

            if( 0 == socketId )
            {
                    LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                          TA_Base_Core::DebugUtil::DebugInfo, 
                      "Socket Id = 0 => Not closing socket" );

                return;
            }

    
                LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                      TA_Base_Core::DebugUtil::DebugInfo, 
                  "Closing socket" );

            #if !defined( WIN32 )

            ::close( socketId );

            #else
            closesocket( socketId );
            #endif // !WIN32

            socketId = 0;

            m_isDisconnected = true;
        }
        catch( ... )
        {
        }

    }

    //*****************************************************************************
    //
    // Method: me_read
    //
    // Purpose:
    //
    //     This method reads a number of bytes from the channel.  It returns the
    //     number of bytes read from the channel.  The method determines if the
    //     client is still connected, and throws an exception if it is not.  The
    //     checking works as follows:
    //
    //     If there is a socket ready for reading and there is no data to be read,
    //     then the socket may not be connected.  After COMMS_CHECK_TIME seconds,
    //     the client is "pinged" and if there is no response, the client is
    //     assumed to have disconnected.  In this case, an exception is thrown.  If
    //     a response is received, the method waits for another COMMS_CHECK_TIME
    //     seconds before checking the connection status again.
    //
    // Returns:
    //
    //     0                                - read succeeded
    //     non-0                            - read failed
    //
    // Arguments:
    //
    // O   char *buffer                     - buffer to read into
    // O   unsigned int *length             - number of bytes read
    // I   unsigned int max_length          - maximum length of the buffer
    //
    // History:
    //
    // Date      Author          Reason
    // ====      ======          ======
    //
    // 19/08/98  Geoff Glasson   Channel now detects when the remote connection
    //                           is closed unexpectedly and throws an
    //                           CHANNEL_EX_COMMS_LOST exception.
    //
    // 12/01/97  Geoff Glasson   Added code to determine if the connection to the
    //                           client is down.  It throws an exception so that
    //                           higher level routines can figure out what to do.
    //
    // 27/11/97  Geoff Glasson   Original version
    //
    //*****************************************************************************
    int UdpSocket::read ( char                  *pBuffer,           // Buffer to read into
						  
						  unsigned int          *pLength,           // Number of bytes read
                          const unsigned int    pMaxLength,         // Maximum length of the buffer
						  struct sockaddr*			addrClient)
    {
        //
        // Do some validation.
        //
        TA_ASSERT( 0 != pBuffer, "NULL pointer passed for read buffer" );
        TA_ASSERT( 0 != pLength , "Zero length read requested" );
        TA_ASSERT( 0 < pMaxLength , "Buffer length is zero or less" );
		
		int nResLen = 0;

        try
        {
			
            //
            // Read max_length bytes from the socket.
            //
            *pLength = 0;
            int addrlen = sizeof(sockaddr);
			if(addrClient == NULL)
				addrClient = &m_clientAddress;
            #if !defined( WIN32 )
			const int lvStatus = ::recvfrom( socketId, ( void * )pBuffer, pMaxLength,0,addrClient,&addrlen );
            #else // defined( WIN32 )
                 const int lvStatus = recvfrom( socketId, pBuffer, pMaxLength, 0 ,addrClient,&addrlen);
            #endif // defined( WIN32 )

             if( 0 <= lvStatus )
             {
                 * pLength = lvStatus;
                 pBuffer[ * pLength ] = '\0';

                 LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                      TA_Base_Core::DebugUtil::DebugDebug, 
                  "Read %d bytes from socket", lvStatus );
            }
            else
            {
				#if !defined( WIN32 )
				
				#else

				#endif
				LOG_GENERIC (SourceInfo, 
					TA_Base_Core::DebugUtil::DebugInfo,
                     "No data returned from socket : Error Info - %d",
                      WSAGetLastError() );

            }

			nResLen = lvStatus;
        }


        //
        // Handle Windows exceptions.
        //
        catch ( const unsigned int )
        {
                TA_THROW( TA_Base_Core::TcpSocketException( "Unexpected Microsoft exception caught" ) );
        }

        //
        // Done.
        //
        return (nResLen);
    }


    //*****************************************************************************
    //
    // Method: me_write
    //
    // Purpose:
    //
    //     This method writes a number of bytes to the communications channel.
    //
    // Returns:
    //
    //     > 0                              - write succeeded
    //     -1                               - write failed
    //
    // Arguments:
    //
    // I   const char *buffer               - buffer to write
    // I   int length                       - number of bytes to write
    //
    // History:
    //
    // Date      Author          Reason
    // ====      ======          ======
    //
    // 27/11/97  Geoff Glasson   Original version
    //
    //*****************************************************************************
    int UdpSocket::write ( const char  *	pBuffer,                // Buffer to write
                           const int		pLength,                 // Number of bytes to write
						   sockaddr*		addrClient)
    {
        //
        // Write the buffer to the socket.
        //
        try
        {
            //
            // If the socket is not connected then we can't really write to
            // it.
            //
           

                LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                      TA_Base_Core::DebugUtil::DebugDebug, 
                  "Attempting to write %d bytes to socket", pLength );

            //
            // Write the data to the stream.  Under UNIX we will retry if there is a
            // write already in progream.
            //
            const char * lvPacket;
            const int lvMaxPacketLength = 1024;
            int  lvPacketLength;
            int lvProgressing = 1;
            int lvTransferredBytes = 0;
            lvPacket = pBuffer;
			int addrlen = sizeof(sockaddr);
			if(addrClient == NULL)
				addrClient = &m_clientAddress;
            int lvNumberOfBytesWritten = 0;
            do
            {

                //
                // Determine the amount of data to be transferred.  The maximum packet
                // size is defined by lv_max_packet_length.
                //
                lvPacketLength = ( pBuffer + pLength) - lvPacket;
                if ( lvMaxPacketLength < lvPacketLength )
                {
                    lvPacketLength = lvMaxPacketLength;
                }

                #if !defined( WIN32 )
                    LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                          TA_Base_Core::DebugUtil::DebugDebug, 
                      "About to write %d bytes to socket", lvPacketLength );

                lvNumberOfBytesWritten = ::sendto(socketId, lvPacket, lvPacketLength,addrClient,addrlen );
                int lvErrno = errno;
                if ( -1 == lvNumberOfBytesWritten )
                {

                        LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                              TA_Base_Core::DebugUtil::DebugWarn, 
                          "Failed to write to socket: %s", strerror( lvErrno ) );

                    //
                    // If a write is in progress, sleep for 0.1 second2.
                    //
                        struct timespec lvSleepTime = { 0,
                                            #if defined( SOLARIS )
                                                    ( unsigned long )( 0.1 * NANOSEC )
                                            #else // !defined( SOLARIS )
                                                    ( unsigned long )( 0.1 * 1000000000 )
                                            #endif // !defined( SOLARIS );
                                                };
                        nanosleep( & lvSleepTime, NULL );
                }
                #else // defined( WIN32 )
                //lvNumberOfBytesWritten = ::send( socketId, pBuffer, pLength, 0 );

                lvNumberOfBytesWritten = ::sendto( socketId, lvPacket, lvPacketLength, 0,addrClient,addrlen );
                int lvErrno = 0;
                if ( SOCKET_ERROR == lvNumberOfBytesWritten )
                {
                     lvErrno = WSAGetLastError();
                     if ( WSAEALREADY == lvErrno )
                     {
                             LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                                  TA_Base_Core::DebugUtil::DebugDebug, 
                              "Write in progress => waiting 1 millisecond before trying again" );

                         //
                         // If a write is in progress, sleep for one millisecond then
                         // try again.
                         //
                         Sleep( 1 );
                     }
                }
                #endif // WIN32
                    else
                {
                    if ( 0 != lvNumberOfBytesWritten )
                    {
                         lvTransferredBytes += lvNumberOfBytesWritten;
                    }
                    else
                    {
                             LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                                  TA_Base_Core::DebugUtil::DebugDebug, 
                              "Wrote %d bytes to the socket", lvNumberOfBytesWritten );

                    }

                    if ( lvPacketLength == lvNumberOfBytesWritten )
                    {
                        lvPacket += lvMaxPacketLength;
                    }
                }

                //
                // Determine if the transfer is progressing.
                //
                lvProgressing = 1;
                if ( ( EBADF == lvErrno ) || 
                     ( ENXIO == lvErrno ) || 
                     ( EPIPE == lvErrno ) ||
                     ( lvPacket > & pBuffer[ pLength - 1 ] ) )
                {
                    lvProgressing = 0;
                }

           #ifndef WIN32
                if ( ( ECONNRESET == lvErrno ) ||
                     ( ENOTCONN  == lvErrno ) )
            #else // !WIN32
                if ( ( WSAECONNRESET == lvErrno ) ||
                     ( WSAENOTCONN  == lvErrno ) )
            #endif // WIN32
                {
                    m_isDisconnected = true;
                    lvProgressing = 0;
                }

            } while ( 0 != lvProgressing );
        return lvTransferredBytes;
    }

    //
    // Deal with MS Visual C++ exceptions.
    //
    catch ( const unsigned int )
    {
             TA_THROW( TA_Base_Core::TcpSocketException( "Unknown Microsoft exception caught" ) );
    }
    }

    //*****************************************************************************
    //
    // Method: me_connect
    //
    // Purpose:
    //
    //     This method connects a communications channel to the requested service
    //     at the other end of the channel.
    //
    // Returns:
    //
    //     0                                - connection succeeded
    //     non-0                            - connection failed
    //
    // Arguments:
    //
    //     none
    //
    // History:
    //
    // Date      Author          Reason
    // ====      ======          ======
    //
    // 23/06/98  Geoff Glasson   Added code to detect connection timeout and throw
    //                           and exception when it occurs.
    //
    // 25/03/98  Geoff Glasson   Added additional error explanation
    //
    // 27/11/97  Geoff Glasson   Original version
    //
    //*****************************************************************************
   /* void TcpSocket::connect ()
    {

         //
         // This connection code has been adapted from Stevens' book Unix
         // Network Programming page 411.
         //
         try
         {
             const int lvSocketError = ::connect( getSocketId(), ( struct sockaddr * )getSocketHandle(),
                                             sizeof( struct sockaddr_in ) );
            if ( 0 == lvSocketError )
            {
                //
                // blocking connect succeeded (or non-blocking succeeded immediately) so reset 
                // m_isDisconnected flag.
                //
                m_isDisconnected = false;
                return;
            }

            //
            // Any error other than EINPROGRESS or WSAEWOULDBLOCK is a serious
            // error and must be handled.
            //
            #if !defined( WIN32 )
             if ( EINPROGRESS != errno )
             {
                 std::string lvMessage = "Unable to connect to socket.  Reason: '";
                 lvMessage += strerror( errno );
                 lvMessage += "'";
                 TA_THROW( TA_Base_Core::TcpSocketException( lvMessage.c_str() ) );
             }
            #else // defined( WIN32 )
             if ( WSAEWOULDBLOCK != WSAGetLastError() )
             {
                 LPVOID lpMsgBuf;
                 FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                FORMAT_MESSAGE_FROM_SYSTEM |
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                WSAGetLastError(),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                (LPTSTR) &lpMsgBuf,
                                0,
                                NULL);

                std::string lvMessage = "Unable to connect to socket.  Reason: '";
                lvMessage += ( char * )lpMsgBuf;
                lvMessage += "'";
                LocalFree( lpMsgBuf );
                TA_THROW( TA_Base_Core::TcpSocketException( lvMessage.c_str() ) );
            }
            #endif // defined( WIN32 )

            //
            // Now make sure that we connect.
            //
            fd_set lvReadSet;
            fd_set lvWriteSet;
            FD_ZERO( & lvReadSet );
            FD_SET( getSocketId(), & lvReadSet );
            lvWriteSet = lvReadSet;

            if( 0 == select( getSocketId() + 1, & lvReadSet, & lvWriteSet, NULL, & m_connectTimeout ) )
            {
                #if !defined( WIN32 )
                 errno = ETIMEDOUT;
                #else // defined( WIN32 )
                 WSASetLastError( WSAETIMEDOUT );
                #endif // defined( WIN32 )
                 TA_THROW( TA_Base_Core::TcpSocketException( "Connection timed out" ) );
             }

             if ( FD_ISSET( getSocketId(), & lvReadSet ) ||
                  FD_ISSET( getSocketId(), & lvWriteSet ) )
             {
                 int lvError( 0 );
                 int lvLength = sizeof( lvError );
                 #if ! defined( WIN32 )
                 if ( getsockopt( getSocketId(), SOL_SOCKET, SO_ERROR,
                             ( char * )& lvError, ( socklen_t * )( & lvLength ) )  < 0 )
                 #else // WIN32
                 if ( getsockopt( getSocketId(), SOL_SOCKET, SO_ERROR,
                             ( char * )& lvError, ( int * )( & lvLength ) )  < 0 )
                 #endif // WIN32
                 {
                     TA_THROW( TA_Base_Core::TcpSocketException( "Error getting socket status" ) );
                 }

                 if ( 0 != lvError )
                 {
                     TA_THROW( TA_Base_Core::TcpSocketException( "Socket status returned error, unable to connect." ) );   
                 }
             }
             else
             {
                 TA_THROW( TA_Base_Core::TcpSocketException( "Unable to connect" ) );
             }

             //
             // non-blocking succeeded so reset m_isDisconnected flag here also.
             //
             m_isDisconnected = false;
         }

         //
         // Deal with MS Visual C++ exceptions.
         //
         catch ( const unsigned int )
         {
           TA_THROW( TA_Base_Core::TcpSocketException( "Unknown Microsoft exception caught" ) );
         }
    }

    //*****************************************************************************
    //
    // Method: me_read_required
    //
    // Purpose:
    //
    //     This method determines if there is data waiting to be read from the
    //     communications channel.
    //
    // Returns:
    //
    //     0                                - no data waiting
    //     non-0                            - data waiting
    //
    // Arguments:
    //
    //     none
    //
    // History:
    //
    // Date      Author          Reason
    // ====      ======          ======
    //
    // 31/12/97  Geoff Glasson   Original version
    //
    //*****************************************************************************
    bool TcpSocket::isReadRequired(unsigned long timeoutMS /* = 0 */ 
	/*) const
    {

         try
         {

            fd_set lvIdset;
            FD_ZERO( & lvIdset );
            FD_SET( socketId, & lvIdset);
            struct timeval lvTimeout;
            lvTimeout.tv_sec = timeoutMS / 1000;
            lvTimeout.tv_usec = 1000 * (timeoutMS % 1000);

            const int lvNumReady = select( FD_SETSIZE, & lvIdset, ( fd_set * )0,
                                            ( fd_set * )0, & lvTimeout );
            if ( lvNumReady > 0 )
            {
                 return true;
            }
            else
            {
                return false;
            }
        }

        //
        // Deal with MS Visual C++ exceptions.
        //
        catch ( const unsigned int )
        {
            TA_THROW( TA_Base_Core::TcpSocketException( "Unknown Microsoft exception thrown" ) );
        }

        return false;
    }*/

    UdpSocket::UdpSocket( const int pSocketId, 
                          const sockaddr_in * pSocketAddress,
                   //       unsigned long connectTimeout,          // How long for non-blocking connects to timeout and throw failure exception?
                   //       unsigned long connectionTestTimeout,   // How long for connectionTest selects to timeout and throw failure exception?
                          unsigned long readingTimeout)          // How long for readong selects to timeout and throw failure exception?

    : isBlocking( true ),
      m_isDisconnected(true)
    {
        TA_ASSERT( 0 < pSocketId, "SocketId is zero or less" );
        TA_ASSERT( 0 != pSocketAddress, "SocketAddress is NULL"  );


        m_readingTimeout.tv_sec  = readingTimeout / 1000;
        m_readingTimeout.tv_usec = readingTimeout % 1000;

        #if defined( WIN32 )

        //
        // It is necessary to initialise winsock before using any of its
        // features, so we will do that here.  Of course this is only
        // necessary under "brain dead" operating systems.
        //
        // NOTE: Winsock should obviously be started before this point, however
        // since each TcpSocket object will call WSACleanup in its destructor
        // it is necessary that each TcpSocket object call WSAStartup in ALL
        // of its constructors.
        //
        int lvError = WSAStartup( MAKEWORD( 2, 2 ), & winsockData );
        if (0 != lvError)
        {
            TA_THROW( TA_Base_Core::TcpSocketException( "Couldn't initialise Windows sockets" ) );
        }

        #endif // !WIN32

        socketId = pSocketId;
        if ( 0 == pSocketAddress )
        {
            TA_THROW( TA_Base_Core::TcpSocketException( "Invalid socket address" ) );
        }

        memcpy( & socketHandle, pSocketAddress, sizeof( socketHandle ) );

        // can now assume the socket open
        m_isDisconnected = false;
    }

    //
    // Returns a string:
    //
    // host (ip address)
    //
    std::string UdpSocket::getClientName() const
    {

         //
         // Get the name of the client.
         //
         struct sockaddr_in lvClient;
         int lvClientSize = sizeof( lvClient );
         memset( & lvClient, 0, sizeof( lvClient ) );

        #if !defined( WIN32 )
         const int lvStatus = getpeername( getSocketId(), ( struct sockaddr * )& lvClient,
                                      ( socklen_t * )& lvClientSize );
        #else // defined( WIN32 )
         const int lvStatus = getpeername( getSocketId(), ( struct sockaddr * )& lvClient,
                                      & lvClientSize );
        #endif // defined( WIN32 )

        if ( 0 > lvStatus )
        {
            TA_THROW( TA_Base_Core::TcpSocketException( "Unable to determine peer name" ) );
        }

        //
        // Resolve the network address.
        //
        std::string lvAddress = inet_ntoa( lvClient.sin_addr );
        struct hostent *lvHostPtr = 0;
        int lvAddr = inet_addr( lvAddress.c_str() );
        #if defined( SOLARIS )
         struct hostent lvHost;
         char lvData[ 256 ];
         int lvError = 0;
         lvHostPtr = gethostbyaddr_r( ( char * )& lvAddr, sizeof( lvAddr ),
                                 AF_INET, & lvHost, lvData,
                                 sizeof( lvData ), & lvError );
        #else // !defined( SOLARIS )
         lvHostPtr = gethostbyaddr( ( const char * )& lvAddr, sizeof( lvAddr ),
                               AF_INET );
        #endif // !defined( SOLARIS )

        //
        // Now construct the string.
        //
        std::string lvAddressString;

        // If we can't resolve, just use the raw IP... an exception isn't required.
        if ( lvHostPtr == NULL )
        {
            lvAddressString.append( lvAddress );
        }
        else
        {
            lvAddressString.append( lvHostPtr->h_name );
        }

        lvAddressString.append( " (" );
        lvAddressString.append( lvAddress );
        lvAddressString.append( ")" );
        return lvAddressString;
    }

    void UdpSocket::getIpAddress ( struct in_addr * pAddress, const std::string & pHost ) const
    {
         //
        // Determine if we want to listen on ALL interfaces.
        //
        if ( 0 == pHost.compare( BIND_TO_ALL_INTERFACES ) )
        {
            #if defined( LINUX ) || defined( FREEBSD )
                pAddress->s_addr = INADDR_ANY;
            #elif defined( SOLARIS )
                pAddress->S_un.S_addr = INADDR_ANY;
            #else
                pAddress->S_un.S_addr = INADDR_ANY;
            #endif

            return;
        }

            //
         // Determine if we already have a dot notation address.
         //
        #if defined( LINUX ) || defined( FREEBSD )
    
         pAddress->s_addr = inet_addr( pHost.c_str() );
         if ( INADDR_NONE != pAddress->s_addr )

        #else // !defined( LINUX ) && !defined( FREEBSD )

         pAddress->S_un.S_addr = inet_addr( pHost.c_str() );

        #if defined( SOLARIS )

         if ( ( unsigned int )-1 != pAddress->S_un.S_addr )

        #else // !defined( SOLARIS )

         if ( INADDR_NONE != pAddress->S_un.S_addr )

        #endif // !defined( SOLARIS )
        #endif // !defined( LINUX )
        {
             return;
        }

        char lvHostBuffer[ 1024 ];
        memset( lvHostBuffer, 0, sizeof( lvHostBuffer ) );

        #if defined( SOLARIS )
         struct hostent lvHostData;
         const struct hostent * lvHost = & lvHostData;
         int lvError;
         if( 0 == gethostbyname_r( pHost.c_str(), & lvHostData, lvHostBuffer,
                              sizeof( lvHostBuffer ), & lvError ) )
        #else // !defined( SOLARIS )
         struct hostent *lvHost;
         lvHost = gethostbyname( pHost.c_str() );
         if (0 == lvHost)
        #endif // !defined( SOLARIS )
         {
             TA_THROW( TA_Base_Core::TcpSocketException( "Non-existent host" ) );
         }

         struct in_addr * lvAddress = * ( struct in_addr ** )lvHost->h_addr_list;
         memcpy( pAddress, lvAddress, sizeof( struct in_addr ) );
    }

    void UdpSocket::getServicePort( struct servent    * pServicePort, const std::string & pService ) const
    {
        //
        // Initialise the service port details.
        //
		
        memset( pServicePort, 0, sizeof( struct servent ) );
        //
        // Port has been specified as a number so get all the relevant details
        // using the port number.
        //

        if ( 0 != atoi( pService.c_str() ) )
        {
			
            struct servent *lvServicePtr;

            #if defined( SOLARIS )

             char lvServiceBuffer[1024];
             memset( lvServiceBuffer, 0, sizeof( lvServiceBuffer ) );
             lvServicePtr = getservbyport_r( atoi( pService.c_str() ), "tcp",
                                        pServicePort,
                                        lvServiceBuffer,
                                        sizeof( lvServiceBuffer ) );

            #else // !defined( SOLARIS )

             lvServicePtr = getservbyport( atoi( pService.c_str() ), "tcp" );

            #endif // !defined( SOLARIS )

            if ( 0 != lvServicePtr )
            {
                memcpy( pServicePort, lvServicePtr, sizeof( struct servent ) );
            }
            else
            {
                pServicePort->s_port = htons( atoi( pService.c_str() ) );
                pServicePort->s_proto = "tcp";
            }

        }

        //
        // Port name specified.
        //
        else
        {

			if ( 0 == atoi( pService.c_str() ) )
             {
				
                 struct servent *lvServicePtr;

                #if defined( SOLARIS )
                 char lvServiceBuffer[1024];
                 memset( lvServiceBuffer, 0, sizeof( lvServiceBuffer ) );
                 lvServicePtr = getservbyname_r( pService.c_str(), "tcp",
                                            pServicePort,
                                            lvServiceBuffer,
                                            sizeof( lvServiceBuffer ) );
				 
                #else // !defined( SOLARIS )
				 
                 lvServicePtr = getservbyname( pService.c_str(), "tcp" );
                #endif // !defined( SOLARIS )

                 if ( 0 == lvServicePtr )
                 {
                     TA_THROW( TA_Base_Core::TcpSocketException( "Unknown service" ) );
                 }
                 memcpy( pServicePort, lvServicePtr, sizeof( struct servent ) );

             }
       }
    }
	
	// client UDP initialization
	UdpSocket::UdpSocket (const std::string & strHost, const std::string & strService, sockaddr_in * destinationAddress)
	{
		
		#if defined( WIN32 )
		
		//
		// It is necessary to initialise winsock before using any of its
		// features, so we will do that here.  Of course this is only
		// necessary under "brain dead" operating systems.
		//
		

		int lvError = WSAStartup( MAKEWORD( 2, 2 ), & winsockData );
		
		if (0 != lvError)
		{
			
			TA_THROW( TA_Base_Core::TcpSocketException( "Couldn't initialise Windows sockets" ) );
		}
		
		#endif // !WIN32
		
		//
		// Get the service details.
		//
		
		
		struct servent lvService;
		getServicePort( & lvService, strService );
		serviceName = strService;
		
		//
		// Create the socket.
		//
		socketId = socket( AF_INET, SOCK_DGRAM, 0 );

		if ( 0 > socketId )
		{
			std::string lvMsg( "Unable to create server socket " );
			lvMsg += strService;
			lvMsg += ".  Reason: ";
			lvMsg += strerror( errno );
			TA_THROW( TA_Base_Core::TcpSocketException( lvMsg ) );
		}
		
		//
		// Set up socket name for future connection.
		//
		
		struct hostent *host;
		host= (struct hostent *) gethostbyname(strHost.c_str());

		memset( & socketHandle, 0, sizeof( socketHandle ) );
		socketHandle.sin_family = AF_INET;
		socketHandle.sin_port = lvService.s_port;
        socketHandle.sin_addr = *((struct in_addr *)host->h_addr);

		* destinationAddress = socketHandle;
		#if !defined( WIN32 )
		fcntl( socketId, F_SETFL, O_NONBLOCK );
		#else // defined( WIN32 )
		const unsigned int lvNonBlocking = 1;
		int nError = ioctlsocket( socketId, FIONBIO, ( unsigned long * )& lvNonBlocking );
		#endif // defined( WIN32 )

	}
}

