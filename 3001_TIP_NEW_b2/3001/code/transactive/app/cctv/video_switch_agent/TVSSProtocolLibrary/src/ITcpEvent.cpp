
#include "app/cctv/video_switch_agent/TVSSProtocolLibrary/src/ITcpEvent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <string>
#include <vector>

using namespace TA_IRS_App;


ITcpEvent::ITcpEvent()
{

}


ITcpEvent::ITcpEvent(const ITcpEvent& tcpEvent)
{
	// Make a copy of the buffer
	m_buffer = tcpEvent.m_buffer;
}



ITcpEvent::~ITcpEvent()
{
}


/**
* Resets the command to be empty.
*/
void ITcpEvent::clear()
{
   m_buffer.clear();
}


/**
* This is used to append incoming bytes to the message
* @param    RecvBytes
*/
void ITcpEvent::appendData(unsigned char data)
{
	m_buffer.push_back(data);
}

short ITcpEvent::getParameterShort(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterShort");
	FUNCTION_EXIT;
	return readBufferShort(position+getEventHeaderLength());
}

unsigned long ITcpEvent::getParameterLong(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterLong");
	FUNCTION_EXIT;
	return readBufferLong(position+getEventHeaderLength());
}

unsigned char ITcpEvent::getParameterChar(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterChar");
	position = position + getEventHeaderLength();

	TA_ASSERT(position<=m_buffer.size(),"Attempt to read buffer beyond received characters.");

	FUNCTION_EXIT;
	return m_buffer[position];
}

std::string ITcpEvent::getParameterShortString(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterShortString");
	std::string result;

	for (unsigned int iLoop=0; iLoop<32; iLoop++)
	{
		result += getParameterChar(position+iLoop);
	}
	
	FUNCTION_EXIT;
	return result;
}

/**
* Return the specified length of the message parameters 
* as defined in the first bytes of the message.
*/
unsigned long ITcpEvent::getParameterLength() const
{
	FUNCTION_ENTRY("getParameterLength");
	FUNCTION_EXIT;
	return getEventLength() - getEventHeaderLength();
}


/**
* Returns the actual number of bytes currently in the message.
*/
unsigned long ITcpEvent::getCurrentLength() const
{
	FUNCTION_ENTRY("getCurrentLength");
	unsigned long value = m_buffer.size();
	FUNCTION_EXIT;
	return value;
}


/**
* Write a long value to the buffer.  Ensure written as little endian, to make compatible with read function.
*/
void ITcpEvent::writeBufferLong(unsigned int position, unsigned long value)
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
unsigned long ITcpEvent::readBufferLong(unsigned int position) const
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
unsigned short ITcpEvent::readBufferShort(unsigned int position) const
{
	FUNCTION_ENTRY("readBufferShort");
	TA_ASSERT((position+2)<=m_buffer.size(),"Attempt to read buffer beyond received characters.");

	unsigned short value;

	value = m_buffer[position+1];
	value = value * 0x0100;
	value = value + m_buffer[position];
	
	FUNCTION_EXIT;
	return value;
}

unsigned char  ITcpEvent ::readBufferByte ( unsigned int position) const 
{
   FUNCTION_ENTRY("readBufferByte");
	TA_ASSERT((position+1)<=m_buffer.size(),"Attempt to read buffer beyond received characters.");
	unsigned char value  ;
	value = m_buffer[ position ] ;
	FUNCTION_EXIT;
	return value;

}


std::string ITcpEvent::getBufferDump()
{
	FUNCTION_ENTRY("getBufferDump");
	std::vector<unsigned char>::iterator it;
	std::string				dump;
	char   numText[40];

	for (it = m_buffer.begin();it != m_buffer.end(); it++)
	{
		sprintf(numText,"%.2X",(*it));
		dump += numText;
	}
	FUNCTION_EXIT;
	return dump;
}
