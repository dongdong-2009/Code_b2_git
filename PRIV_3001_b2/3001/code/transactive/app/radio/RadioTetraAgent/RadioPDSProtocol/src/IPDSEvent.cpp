
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/FrameCheckSequence.h"


#include <sstream>
#include <iomanip>

namespace TA_IRS_App
{
	IPDSEventException::IPDSEventException(const std::string &errorText) throw()
		: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY("IPDSEventException");
		FUNCTION_EXIT;
	}
	
	IPDSEventException::~IPDSEventException()throw()
	{
		FUNCTION_ENTRY("~IPDSEventException");
		FUNCTION_EXIT;
	}
	
	const unsigned short IPDSEvent::CHECKSUM_POSITION   = 2;

	IPDSEvent::IPDSEvent()
	{
		FUNCTION_ENTRY( "IPDSEvent" );
		FUNCTION_EXIT;
	}

	IPDSEvent::IPDSEvent( const IPDSEvent& IPDSEvent )
	{
		FUNCTION_ENTRY( "IPDSEvent" );
		//Make a copy of the buffer
		m_buffer = IPDSEvent.m_buffer;
		FUNCTION_EXIT;
	}

	IPDSEvent::~IPDSEvent()
	{
		FUNCTION_ENTRY( "~IPDSEvent" );
		FUNCTION_EXIT;
	}

	/**
	 * Reset the command to be empty
	 */
	void IPDSEvent::clear()
	{
		FUNCTION_ENTRY( "clear" );
		m_buffer.clear();
		FUNCTION_EXIT;
	}

	/**
	 * This is used to append incoming bytes to the message
	 * @param    RecvBytes
	 */
	void IPDSEvent::appendData(const std::string& clientName, const std::vector<unsigned char>& data )
	{
		FUNCTION_ENTRY( "appendData" );
		
		setClientName(clientName);
		m_buffer = data;
		verifyChecksum();
	

		FUNCTION_EXIT;
	}


	/**
	 * Return the specified length of the message parameters 
	 * as defined in the first bytes of the message.
	 */
	//TODO:
	unsigned long IPDSEvent::getParameterLength() const
	{
		FUNCTION_ENTRY("getParameterLength");
		FUNCTION_EXIT;
		unsigned long getLength, getHeaderLength;
		getLength = getEventLength(); //7
		getHeaderLength = getEventHeaderLength(); //6
		//return getEventLength() - getEventHeaderLength();
		return getLength;
	}

	/**
	 * Returns the actual number of bytes currently in the message.
	 */
	unsigned long IPDSEvent::getCurrentLength() const
	{
		FUNCTION_ENTRY("getCurrentLength");
		unsigned long value = m_buffer.size();
		FUNCTION_EXIT;
		return value;
	}

	/**
	 * Write a long value to the buffer.  Ensure written as little endian, to make compatible with read function.
	 */
	void IPDSEvent::writeBufferLong(unsigned int position, unsigned long value)
	{
		FUNCTION_ENTRY("writeBufferLong");
		m_buffer[position+0] = (unsigned char)(value & 0xff);
		m_buffer[position+1] = (unsigned char)((value>>8) & 0xff);
		m_buffer[position+2] = (unsigned char)((value>>16) & 0xff);
		m_buffer[position+3] = (unsigned char)((value>>24) & 0xff);
		FUNCTION_EXIT;
	}

	/**
	 * Read a long value from the buffer.  Ensure read as little endian.
	 */
	unsigned long IPDSEvent::readBufferLong(unsigned int position) const
	{
		FUNCTION_ENTRY("readBufferLong");
		unsigned long value;

		value = readBufferShort(position+2);
		value = value * 0x00010000; 
		value = value + readBufferShort(position);

		FUNCTION_EXIT;
		return value;
	}

	/**
	 * Read a short value from the buffer.  Ensure read as little endian.
	 */
	unsigned short IPDSEvent::readBufferShort(unsigned int position) const
	{
		FUNCTION_ENTRY("readBufferShort");
		TA_ASSERT((position+2)<=m_buffer.size(),"Attempt to read buffer beyond received characters.");

		unsigned short value;

		value = m_buffer[position];
		value = value * 0x0100;
		value = value + m_buffer[position+1];

		FUNCTION_EXIT;
		return value;
	}

	unsigned char IPDSEvent::readBufferByte(unsigned int position) const
	{
		FUNCTION_ENTRY("readBufferByte");
		TA_ASSERT((position+1)<=m_buffer.size(),"Attempt to read buffer beyond received characters.");
		unsigned char value  ;
		value = m_buffer[ position ] ;
		FUNCTION_EXIT;
		return value;
	}

	std::string IPDSEvent::getBufferDump()
	{
		FUNCTION_ENTRY("getBufferDump");
		std::vector<unsigned char>::iterator it;
		std::string				dump;
		char   numText[40] = {0};

		for (it = m_buffer.begin();it != m_buffer.end(); it++)
		{
			sprintf(numText," %.2X",(*it));
			dump += numText;
		}
		FUNCTION_EXIT;
		return dump;
	}

	std::vector<unsigned char> IPDSEvent::getRawData() const
	{
		FUNCTION_ENTRY( "getRawData" );
		FUNCTION_EXIT;
		return m_buffer;
	}

	void IPDSEvent::verifyChecksum()
	{
		FUNCTION_ENTRY( "verifyChecksum" );

		unsigned short checksum = getWord( m_buffer.size() - CHECKSUM_POSITION );

		// GET the 16-bit Checksum
		// calculates a checksum on the message
		unsigned short calculatedCheckSum = TA_IRS_App::FrameCheckSequence::calculate( m_buffer, m_buffer.size() - CHECKSUM_POSITION );

		//compares the 2, throws an exception if mismatch.

		if ( checksum != calculatedCheckSum )
		{
			std::ostringstream checsumstr;
			
			checsumstr << "Received " << std::hex << std::setw(4) << std::setfill('0') << checksum
				<< " Calculated " << std::hex << std::setw(4) << std::setfill('0') << calculatedCheckSum;

			std::string strChecksum(checsumstr.str());
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Checksum is invalid : %s", strChecksum.c_str());

			TA_THROW( IPDSEventException( "Invalid checksum "));
		}

		FUNCTION_EXIT;
	}

	unsigned char IPDSEvent::getByte( unsigned short position ) const
	{
		FUNCTION_ENTRY( "getByte" );

		if ( position < m_buffer.size() )
		{
			return m_buffer[position];
		}
		else
		{
			std::ostringstream positionstr;
			positionstr << "Requested: " << position << "Length: " << m_buffer.size();

			//  TA_THROW( ProtocolException( "Invalid byte position", "position", positionstr.str() ) );

			//  TA_THROW( ProtocolException( "Invalid byte position", "position", positionstr.str() ) );
		}	
		
		FUNCTION_EXIT;
		return 0;
	}

	unsigned short IPDSEvent::getWord(unsigned short position) const
	{
		FUNCTION_ENTRY( "getWord" );

		unsigned int resultWord = 0;

		// little endian ordering on multi bytes
		unsigned char lowerByte = getByte( position );
		unsigned char upperByte = getByte( position + 1 );

		resultWord = ( ( static_cast<unsigned short>( upperByte )  << 8 ) 
			| static_cast<unsigned short>( lowerByte ) );

		FUNCTION_EXIT;
		return resultWord;
	}

}
