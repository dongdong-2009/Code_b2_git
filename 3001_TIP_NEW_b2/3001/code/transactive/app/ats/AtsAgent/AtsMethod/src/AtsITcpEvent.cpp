
#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpEvent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <string>
#include <vector>

using namespace TA_IRS_App;


AtsITcpEvent::AtsITcpEvent()
{
	 FUNCTION_ENTRY("Constructor");
	 FUNCTION_EXIT;
}


AtsITcpEvent::AtsITcpEvent(const AtsITcpEvent& tcpEvent)
{
	FUNCTION_ENTRY("Constructor");
	// Make a copy of the buffer
	m_buffer = tcpEvent.m_buffer;
	FUNCTION_EXIT;
}



AtsITcpEvent::~AtsITcpEvent()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}


/**
* Resets the command to be empty.
*/
void AtsITcpEvent::clear()
{
	FUNCTION_ENTRY("clear");
	m_buffer.clear();
	FUNCTION_EXIT;
}


/**
* This is used to append incoming bytes to the message
* @param    RecvBytes
*/
void AtsITcpEvent::appendData(unsigned char data)
{
	m_buffer.push_back(data);
}

short AtsITcpEvent::getParameterShort(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterShort");
	FUNCTION_EXIT;
	return readBufferShort(position+getEventHeaderLength());
}

unsigned long AtsITcpEvent::getParameterLong(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterLong");
	FUNCTION_EXIT;
	return readBufferLong(position+getEventHeaderLength());
}

unsigned char AtsITcpEvent::getParameterChar(unsigned int position) const
{
	FUNCTION_ENTRY("getParameterChar");
	//position = position + getEventHeaderLength();

	TA_ASSERT(position<=m_buffer.size(),"Attempt to read buffer beyond received characters.");

	FUNCTION_EXIT;
	return m_buffer[position];
}

std::string AtsITcpEvent::getParameterShortString(unsigned int position) const
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
unsigned long AtsITcpEvent::getParameterLength() const
{
	FUNCTION_ENTRY("getParameterLength");
	FUNCTION_EXIT;
	unsigned long getLength, getHeaderLength;
	getLength = getEventLength(); //7
	getHeaderLength = getEventHeaderLength(); //5
	return getEventLength() - getEventHeaderLength();
}


/**
* Returns the actual number of bytes currently in the message.
*/
unsigned long AtsITcpEvent::getCurrentLength() const
{
	FUNCTION_ENTRY("getCurrentLength");
	unsigned long value = m_buffer.size();
	FUNCTION_EXIT;
	return value;
}


/**
* Write a long value to the buffer.  Ensure written as little endian, to make compatible with read function.
*/
void AtsITcpEvent::writeBufferLong(unsigned int position, unsigned long value)
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
unsigned long AtsITcpEvent::readBufferLong(unsigned int position) const
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
unsigned short AtsITcpEvent::readBufferShort(unsigned int position) const
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

unsigned char  AtsITcpEvent ::readBufferByte ( unsigned int position) const 
{
   FUNCTION_ENTRY("readBufferByte");
	TA_ASSERT((position+1)<=m_buffer.size(),"Attempt to read buffer beyond received characters.");
	unsigned char value  ;
	value = m_buffer[ position ] ;
	FUNCTION_EXIT;
	return value;

}


std::string AtsITcpEvent::getBufferDump()
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

unsigned char AtsITcpEvent::getFlag()
{
	return m_buffer[0];
}
unsigned char AtsITcpEvent::getCommand()
{
	return m_buffer[1];
}
unsigned char AtsITcpEvent::getTrainId()
{
	return m_buffer[2];
}

unsigned char AtsITcpEvent::getLength()
{
	return m_buffer[3];
}

std::vector<unsigned char> AtsITcpEvent::getCRC()
{
	std::vector<unsigned char> temp;
	temp.push_back(m_buffer[4]);
	temp.push_back(m_buffer[5]);
	return temp;
}