
#if !defined( __UdpSocket_h__ )
#define __UdpSocket_h__

#if !defined( WIN32 )
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//TD 14774
//zhou yuan++
#define WIN32_LEAN_AND_MEAN 
//++zhou yuan
#include <winsock2.h>
#endif // !WIN32
#include <string>
#include <time.h>
#include "core/exceptions/src/TcpSocketException.h"
#include "ISocket.h"

namespace TA_Base_Core
{

    const std::string BIND_TO_ALL_INTERFACES_ ("BIND_TO_ALL_INTERFACES");


	/**
  		*
  		* Class: TcpSocket
  		*
  		* Purpose:
  		*
  		*    This class implements a TCP/IP socket.  The socket can be either
  		*    blocking or non-blocking.  The socket can be constructed using one
  		*    of two methods:
  		*
  		*    1 - connect to known host and service
  		*    2 - duplicate an existing socket, which may have been created using
  		*        the accept() system call.
  		*
  		* Exceptions:
  		*
  		*    This class throws TcpSocketExceptions when an error occurs.  The
  		*    message contained within the exception explains the problem.  The
  		*    TcpSocketException class implements the what() method which returns
  		*    the message explaining the problem.
  		*
  		* Usage:
  		*
  		*    To create a socket to a specified host and service:
  		*
  		*        TcpSocket lvSocket( "hostname", "service name" );
  		*        lvSocket.connect();
  		*
  		*    To create a socket object from an existing socket:
  		*
  		*        TcpSocket lvSocket( socketId, socketAddress );
  		*        lvSocket.connect();
  		*
  		* @see also TcpSocketException
  		*/
	class UdpSocket : public ISocket
	{
	
	private:

			#ifdef WIN32
			WSAData winsockData;
			#endif // WIN32

			//
			// Socket id.
			//
			int socketId;

			//
			// Socket.
			//
			struct sockaddr_in socketHandle;

			//
			// Service that socket is connected to.
			//
			std::string serviceName;

            // Flag used to help stillConnected() determine the connection
            // status.  This is set whenever ECONNRESET or ENOTCONN are seen
            // when operating on supposedly connected sockets.  The problem 
            // with stillConnected() is that the select may return 0 if the
            // select call times out (1sec).  This doesn't necessarily mean
            // it is disconnected.  
            //
            // mutable to avoid removing const for stillConnected()
            mutable bool m_isDisconnected;

			//
			// Disabled methods.
			//
			UdpSocket();
			UdpSocket (const UdpSocket &rhs);
			UdpSocket &operator= (const UdpSocket &rhs);

			/**
  			* This attribute indicates that the socket is a blocking socket.
  			*/
			bool isBlocking;

            //
            // Maximum time in seconds a non-blocking conect will wait for 
            // for success before assuming connect failed and throwing exception
            // to indicate failure to client code.
            //
           // struct timeval m_connectTimeout;
           // struct timeval m_connectionTestTimeout;
            struct timeval m_readingTimeout;


			/**
  			* This method gets the internet address for the specified host.  The
  			* host can be specified as an canonical name or a dot string.  If the
  			* method is called using an invalid dot string, the method assumes
  			* that the string actually contains a host name.
  			*
  			* @param struct in_addr * pAddress - resulting host address
  			* @param const std::string & pHost - host name or IP address
  			*
  			* @throws TcpSocketException when the host name is not known.
  			*
  			*/
			void getIpAddress( struct in_addr * pAddress,
   				                const std::string & pHost ) const;

			/**
   			*
   			* This method gets the service port required for the socket
   			* connection.  The supplied server port can be a name or a number.
   			*
   			* @param struct servent * pServicePort - resulting service details
   			* @param const std::string & pService  - service name
   			*
   			*/
			void getServicePort( struct servent * pServicePort,
   				                  const std::string & pService ) const;

		public:

   				/**
   				* This method returns the underlying socket id ( or file handle )
  			* which can be used by system calls.
  			*
  			* @return int socketId - the socket id assigned to the socket
  			*
  			*/
			inline int getSocketId() const
    		{
        		return socketId;
    		}

			/**
  			* This method returns the socket handle of the socket which can be
  			* used in system calls.
  			*
  			* @return const struct sockadd_in * - socket handle data
  			*
  			*/
			inline const struct sockaddr_in *getSocketHandle() const
    	    {
        	    return & socketHandle;
    	    }

	       /**
           * This is the constructor used by most applications because it allows
           * the socket to be created ready to be connected to the specified host
           * and service port.
           *
           * @exception TcpSocketException - this is thrown under the following
           *                                 conditions:
           *
           *            1 - the host name was not specified or cannot be
           *                determined
           *            2 - the service name was not specified or cannot be
           *                determined
           *
           */
           UdpSocket( const std::string & host, 
                      const std::string & service,
           			  const bool pBlocking = true, 
                      unsigned long readingTimeout = DEFAULT_READING_TIMEOUT );
			
		   UdpSocket (const std::string & strHost,
					  const std::string & strService,
					  sockaddr_in		* destinationAddress);


		  /**
           * This is a special constructor used by TcpServerSocket.  It is a
		   * special form of socket that has no host name qualifier.
           *
           * @exception TcpSocketException - this is thrown under the following
           *                                 conditions:
           *
           *            1 - the service name was not specified or cannot be
           *                determined
           *
           */
           UdpSocket( const std::string & service,
           			  const bool pBlocking = true, 
                 //     unsigned long connectTimeout = DEFAULT_CONNECT_TIMEOUT, 
                 //     unsigned long connectionTestTimeout = DEFAULT_CONNECTION_TEST_TIMEOUT, 
                      unsigned long readingTimeout = DEFAULT_READING_TIMEOUT );


		   virtual ~UdpSocket ();

		  /**
  		  * Constructor used to duplicate an existing socket.
  		  *
  		  * @exception TcpSocketException - this is thrown under the following
  		  *                                 conditions:
  		  *
  		  *            1 - the socket does not exist
  		  */
		  UdpSocket( const int pSocket, 
                     const sockaddr_in * pAddress,
                 //    unsigned long connectTimeout = DEFAULT_CONNECT_TIMEOUT, 
                 //    unsigned long connectionTestTimeout = DEFAULT_CONNECTION_TEST_TIMEOUT, 
                     unsigned long readingTimeout = DEFAULT_READING_TIMEOUT );

		 /**
  		 * This method writes a buffer of data to the socket.  This method
  		 * will block immediately if the socket is a blocking socket, and will
  		 * return immediately if the socket is a non-blocking socket.
  		 *
  		 * @param const char * pBuffer       - address of the buffer to be written
  		 * @param unsigned int pBufferLength - number of bytes in the buffer
  		 *
  		 * @return int - the number of bytes that were written
  		 *
  		 * @exception TcpSocketException     - this exception is thrown when
  		 *                                     the socket address is
  		 *                                     incomplete
  		 *
  		 */
		  int write( const char * pBuffer, const int pLength,struct sockaddr* addrClient=NULL);

		 /**
  		 * This method reads a buffer of data from the socket.  This method
  		 * will block immediately if the socket is a blocking socket, and will
  		 * return immediately if the socket is a non-blocking socket.
  		 *
  		 * @param const char * pBuffer       - address of the buffer to be
  		 *                                     store the received data
  		 * @param unsigned int *pActual      - number of bytes actually read
  		 *
  		 * @return int - zero on success, non-zero when the method failed and
  		 *               the operating system error code is returned
  		 *
  		 * @exception TcpSocketException     - this exception is thrown when
  		 *                                     Microsoft exceptions are
  		 *                                     caught.  The Microsoft
  		 *                                     exceptions are ignored because
  		 *                                     we have insufficient
  		 *                                     documentation regarding their use.
  		 *
  		 */
		 int read ( char *pBuffer, unsigned int *pActual, const unsigned int pBufferLength ,struct sockaddr* addrClient =NULL);


		/**
 		* Connect to the socket.
 		*
 		* @exception TcpSocketException     - the exception is thrown when a
 		*                                     connection to the remote socket
 		*                                     cannot be made.
 		*/
		 void connect () {};

		/**
  		* Determine if socket is connected to the specified service.
  		*
  		* @param server                    - name of the service to check
  		*
  		* @return bool                     - true if socket connected to
  		*                                    service
  		*                                  - false if socket not connected to
  		*                                    service
  		*/
		inline bool isConnectedTo ( const std::string & service )
    	{
        	return( service == serviceName );
    	}

		/**
 		* Determine if read required.
        * @param timeoutMilliSec            - waits (blocks) for the specified time,
        *                                     returns immediately if zero (the default).
 		*
 		* @return bool                      - true if read required
 		*                                   - false if read not required
 		*/
		bool isReadRequired (unsigned long timeOutMilliSec = 0) const {return true;};

		/**
 		* Get the name of the connected client.
 		*
 		* @return std::string               - name of the connected client.
 		*                                     The string is empty if the
 		*                                     socket is not connected.
 		*/
    		std::string getClientName() const;

		/**
 		* Get the name of the service port.
 		*
 		* @return std::string - name of the service port that the socket is
 		*                       connected to.
 		*/
		inline std::string getServiceName() const
    	{
        	return serviceName;
    	}

		/**
 		* Determine if the socket is blocking.
 		*
 		* @return bool                      - true if blocking socket
 		*                                   - false if non-blocking socket
 		*/
		inline bool isBlockingSocket() const
    	{
        	return isBlocking;
    	}

		/**
 		* Determine if the socket is still connected to the remote end point.
 		*
 		* @return bool                      - true if socket still connected
 		*                                   - false if socket not connected
 		*/
		bool stillConnected() const {return true;};

		/**
  		* close
  		*
  		* This method closes the socket.
  		*/
		void close();

		// added,
		// maybe there is updated ISocket code that suffice on the 2 function above
		// ask later
		virtual int read ( char *pBuffer, unsigned int *pActual, const unsigned int pBufferLength ) {return 0;}
		virtual int write( const char * pBuffer, const int pLength ){return 0;}


	private:
			sockaddr m_clientAddress;
	};
}
#endif // !__TcpSocket_h__
