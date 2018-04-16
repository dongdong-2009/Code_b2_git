#pragma once

#include <string>

/**
* This class encapulates the data associated with a radio method.
* @author luo bihui
* @version 1.0
* @generator Enterprise Architect
* @created 10-Dec-2010 08:32:15
*/

namespace TA_IRS_App
{
	class ITcpMethod
	{
		public:

		protected:

			/*
			* ITcpMethod
			*
			* If the methodHeaderLength is 0, then it will not be set. The constructor also assumes the
			* methodHeaderLength is a Long. If this is not the case, pass 0 and set with the virtual
			* method explicitly
			*
			*/
			ITcpMethod(unsigned long methodLength, unsigned long methodHeaderLength,
				unsigned long methodHeaderLengthStartPosition);

		public:
			virtual ~ITcpMethod();

		public:

			unsigned long getMessageDataSize() const;

			/*
			* getMessageData
			*
			* Get a constant pointer to the message buffer so it can be 
			* accessed by the TCP/IP socket for transmission.
			*
			*/
			const unsigned char * getMessageData() const;
			/**
			* Return the specified length of the message header
			*/
			virtual unsigned long getMethodHeaderLength() const = 0;
			/**
			* Return the specified length of the message header
			*/
			virtual unsigned long getShortStringLength() const = 0;
            
            /**
             * Get the message data in printable format
             */
            void getLogText(std::string& hexText, std::string& consoleText) const;

		protected:

			void setParameterLong(unsigned int position,unsigned long value);
			void setParameterShort(unsigned int position,unsigned short value);
			void setParameterByte(unsigned int position,unsigned char value);
			void setParameterShortString(unsigned int position,std::string value);
            void setParameterLongString(unsigned int position,std::string value,bool includeEosNull=false);

			/**
			* Used to adjust the message size for dynamically sized messages.
			* @param    newsize    New size for message
			*/
			void setMessageSize(unsigned int newsize);

			void setByte(unsigned int position, unsigned char value);
			/**
			* Set the short value into the buffer with endian conversion
			* @param    position
			* @param    value
			*/
			void setShort(unsigned int position, unsigned short value);
			void setLong(unsigned int position, unsigned long value);
			void setShortString(unsigned int position,std::string value);
			void setLongString(unsigned int position,std::string value,bool includeEosNull);

		protected: 
		
			unsigned char*  m_messageBuffer;
			unsigned int    m_messageSize;
			unsigned long   m_messageHeaderMessageLengthStartPostion;

		private:

			/*
			* Disable the assignment and copy constructors
			*/
			ITcpMethod& operator=(const ITcpMethod &);
			ITcpMethod( const ITcpMethod& itcpMethod);
	};
};
