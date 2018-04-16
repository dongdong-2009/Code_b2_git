#pragma once
///////////////////////////////////////////////////////////
//
//  ITcpEvent.h
//  Implementation of the Class ITcpEvent
//  Generated by Enterprise Architect
//  Created on:      04-Nov-2003 13:28:33
//  Original author: Andy Parker
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#include <vector>
#include <string>

/**
* This class encapsulates the data of a radio event.  Radio events are sent back
* to indicate the progress of radio methods (sent requests).
* @author luo bihui
* @version 1.0
* @generator Enterprise Architect
* @created 10-Dec-2010 13:28:32
*/

namespace TA_IRS_App
{
	class ITcpEvent
	{

	public:

		ITcpEvent();
		ITcpEvent(const ITcpEvent& ITcpEvent);
		virtual ~ITcpEvent();

	public:
		/**
		* Resets the command to be empty.
		*/
		virtual void clear();
		/**
		* This is used to append incoming bytes to the message
		* @param    RecvBytes
		*/
		void appendData(unsigned char data);
		/**
		* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
		*/
		virtual bool isComplete() const  = 0;
		/**
		* Return the specified length of the message
		*/

		
		virtual unsigned long getEventLength() const = 0;
		/**
		* Return the specified length of the message header
		*/
		virtual unsigned long getEventHeaderLength() const = 0;

		/**
		* Read the specified parameter value.  Note position starts at 0 indicating 
		* start of parameter block not header start.
		*/
		short getParameterShort(unsigned int position) const;
		unsigned long getParameterLong(unsigned int position) const;
		unsigned char getParameterChar(unsigned int position) const;
		std::string getParameterShortString(unsigned int position) const;
		/**
		* Return the specified length of the message parameters 
		* as defined in the first bytes of the message.
		*/
		unsigned long getParameterLength() const;
		std::string getBufferDump();
        size_t getBufferLength() const;

		virtual bool crcCheck() const = 0;
		virtual void peelOffCrc() = 0;

		virtual bool isCompleteWithCrc() = 0;
	protected:
		std::vector<unsigned char>   m_buffer;

		/**
		* Returns the actual number of bytes currently in the message.
		*/
		unsigned long getCurrentLength() const;

		/**
		* Access the local buffer
		*/
		void writeBufferLong(unsigned int position, unsigned long value);
		unsigned long  readBufferLong(unsigned int position) const;
		unsigned short readBufferShort(unsigned int position) const;
		unsigned char  readBufferByte ( unsigned int position) const ;

	private:

	private:

	};

    inline size_t ITcpEvent::getBufferLength() const
    {
        return m_buffer.size();
    }
};
