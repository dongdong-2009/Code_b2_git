/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/AtsModbusTcpConnection.h $
  * @author  A. Ivankovic
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * A TCP/IP connection to a Modbus device.
  *
  * Note: In failure scenarios calls to the IModbus method implementations of this
  * class will block the calling thread for the TCP/IP connection time out period.
  * If this is an issue for the application, the Modbus communication threads need
  * to be isolated from the application code that needs to be responsive even
  * under failure scenarios.
  *
  */

#ifndef ATS_MODBUS_TCP_CONNECTION_H
#define ATS_MODBUS_TCP_CONNECTION_H

#include <string>
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"

#include    "bus/modbus_comms/src/ModbusConnection.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"

namespace TA_IRS_App
{
    /**
     * Clone of ModbusTcpConnection with to use AtsTcpSocket rather than the
     * TA_Base_Core::TcpSocket. See the doc in that class for more details.
     *
     * Unfortunately, merely overriding the ModbusTcpConnection was not
     * sufficient, as it held the m_socket member as a TA_Base_Core::TcpSocket
     */
    class AtsModbusTcpConnection : public TA_Base_Bus::ModbusConnection
    {

    public:
        /**
          * AtsModbusTcpConnection
          *
          * Constructor.
          *
          * @param host The name or IP address of the Modbus device to be
          * communicated to via a TCP/IP connection.
          */
        AtsModbusTcpConnection( const std::string& host,
                                const std::string& service,
                                long socketTimeout = 2000,
                                bool tcpNoDelay = true );

        /**
          * ~AtsModbusTcpConnection
          *
          * Destructor.
          */
        virtual ~AtsModbusTcpConnection();

		        /**
          * writeHoldingRegister
          *
          * Writes a word to the specified holding register. The STE RTU does not
		  * support this functionality.
          *
          * @param address  The address of the register to be written to.
          * @param value    The value to write.
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegister( int address, WORD value );



        /**
          * writeHoldingRegister
          *
          * Writes to the specified holding register but also applies a combination
          * of an AND mask and an OR mask during the write. The algorithm used
          * is as follows:
          * Result = (Current Contents AND And_Mask) OR (Or_Mask AND And_Mask)
		  *
		  * The STE RTU does not support this functionality.
          *
          * @param address  The address of the register to be written to.
          * @param andMask  The AND mask to use.
          * @param mask     The OR mask to use.
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegister( int address, WORD andMask, WORD orMask );



        /**
          * writeHoldingRegister
          *
          * Writes to a specific bit within a holding register. The bit address
          * ranges from 1 through to 16 inclusive. The STE RTU does not support
		  * this functionality.
		  *
          * @param address  The address of the register to be written to.
          * @param bit      The bit number to set.
          * @param value    The value to set the bit to (true=1, false=0).
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegisterBit( int address, int bit, bool value );

        /**
          * enable
          *
          * Enables this object for Modbus device communications. This object
          * will subsequently initiate a connection to the actual modbus device
          * as required to respond to communication method calls.
          */
        virtual void enable();

        /**
          * disable
          *
          * Disables Modbus communications via the object. The current connection
          * to the Modbus device is disconnected. Any subsequent communication method
          * calls on this object will fail (until it is enabled again). The disable
          * method is useful in redundancy scenarios where a IModbus instance is
          * being used as a "hot standby" but should not take up any Modbus device
          * connection resources.
          */
        virtual void disable();

		 /**
          * ensureConnected
          *
          * Connects to the Modbus device if a connection is not current.
          *
          * @exception TcpSocketException Thrown if a TCP/IP connection problem is
          * encountered.
          */
        virtual void ensureConnected();

		/**
          * isConnected
          *
          * Checks the device connection and returns true if the device is connected
		  *
          * @return			True if the device is connected, false otherwise
          */
		virtual bool isConnected();

    protected:
        /**
          * close
          *
          * Closes the socket connection if open and destroys the socket object.
          * This method does not change the enable status.
          */
        void close();

        /**
          * writeRequest
          *
          * Writes Modbus request data to the communication connection.
          *
          * @param request      The data buffer defining the request.
          */
        virtual void writeRequest( const std::string& request );

        /**
          * readReply
          *
          * Reads the reply to a Modbus request.
          *
          * @param reply        A buffer to store the reply data in. Note that the reply
          *                     data does not include any transport layer specific header
          *                     or footer data.
		  * @param expectedSize The expected number of bytes in the reply packet assuming
		  *                     the reply isn't an exception/error response.
          */
        virtual void readReply( std::string& reply, int expectedSize );

		/**
          * appendByteCount
          *
          * Appends a byte count to the supplied request packet. The byte count
          * is calculated based on the number of data items and the size of
          * each item in bytes. Note that a zero byte count is appended if the
          * calculated count exceeds the capacity of a byte. This does not strictly
          * match the Modbus protocol but allows for communication to non-Modicon
          * devices that can handle large packet sizes.
          *
          * @param request      The request packet to append to.
          * @param dataCount    The number of data elements that will follow.
          * @param elementSize  The size in bytes of each data element.
          */
        virtual void appendByteCount( std::string & request, int dataCount, 
                              int elementSize = sizeof(char) );


        /**
          * write
          *
          * Wraps the TcpSocket write function with some exception
          * processing code.
          *
          * @param buffer   The buffer to be written to the socket.
          * @param length   The length of the buffer to be written.
          * @return         The number of characters actually written.
          */
		int write( const char* buffer, const unsigned int length );



        /**
          * read
          *
          * Wraps the TcpSocket read function with some exception
          * processing code.
          *
          * @param buffer       The buffer to be read from the socket.
          * @param bytesRead    The actual number of bytes read.
          * @param length       The maximum allowed length of the read buffer returned.
          * @return             The number of characters actually read.
          */
		int read( char* buffer, const unsigned int bufferLength );

    private:
        AtsModbusTcpConnection( const AtsModbusTcpConnection& );
        AtsModbusTcpConnection& operator=( const AtsModbusTcpConnection& );
        
        void drainReadBuffer();

    public:
        static const int DEFAULT_PORT;
        static const std::string DEFAULT_SERVICE;

    protected:
        static const unsigned int MBAP_HEADER_SIZE;
        static const int MODBUS_PROTOCOL_ID;
		static const int HEADER_SIZE;
        static const int FOOTER_SIZE;

        /** The transaction ID in the modbus header is sent as
          * 16 bits, this is used to make sure we dont have a 
          * transaction ID greater than this.
          */
        static const int MAX_TRANSACTION_ID;

        int m_transactionID;
        long                                    m_socketTimeout;
        int                                     m_tcpNoDelay;
		volatile bool                           m_closed;
        std::string                             m_host;
        std::string                             m_service;
        ACE_SOCK_Connector                      m_sockConnector;
        ACE_SOCK_Stream                         m_sockStream;
        TA_Base_Core::ReadWriteThreadLockable   m_socketStreamLock;
		char m_replyBuffer[ 16384 ];

    };
}

#endif // ATS_MODBUS_TCP_CONNECTION_H
