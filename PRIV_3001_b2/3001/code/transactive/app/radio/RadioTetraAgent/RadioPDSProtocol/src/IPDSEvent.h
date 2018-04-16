/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $Source: //depot/3001_TIP/3001/transactive/app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h $
  * @author:  Lucky Cuizon
  * @version: $Revision : 1 $
  *
  * Last modification: $2011/24/09 11:31:45$
  * Last modified by:  $Author: Lucky Cuizon $
  */

#ifndef IPDS_EVENT_H
#define IPDS_EVENT_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include "core/exceptions/src/TransactiveException.h"
/**
* This class encapsulates the data of a radio pds event.  Radio events are sent back
* to indicate the progress of radio methods (sent requests).
*/

namespace TA_IRS_App
{
	class IPDSEventException : public TA_Base_Core::TransactiveException
	{
	public:
		IPDSEventException( const std::string& errorText ) throw();
		virtual ~IPDSEventException() throw();
	};

	struct SRadioPDSBuffer
	{
		unsigned char ucMsgID;
		unsigned char ucTrainID;
		unsigned char ucOrigin;
		std::vector<unsigned char> radioPDSMessage;
	};

	class IPDSEvent
	{
	public:

		IPDSEvent();
		IPDSEvent( const IPDSEvent& IPDSEvent );
		virtual ~IPDSEvent();

	public:

		/**
		* Resets the command to be empty.
		*/
		virtual void clear();
		/**
		* This is used to append incoming bytes to the message
		* @param    RecvBytes
		*/
		void appendData(const std::string& clientName, const std::vector<unsigned char>& data);

		/**
		* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
		*/
		virtual bool isComplete() const  = 0;
		/**
		* Return the specified length of the message
		*/

		virtual void setEventLength(unsigned int length) = 0;
		virtual unsigned long getEventLength() const = 0;

		/**
		 * Set the client Name
		 */
		virtual void setClientName(const std::string& clientName ) = 0;
		virtual std::string getClientName() const = 0;

		/**
		 * Set DestinationTsi
		 */
		virtual void setDestinationTsi( const std::string& destinationTsi) = 0;
		virtual std::string getDestination() const = 0;

		

		

		/**
		* Return the specified length of the message header
		*/
		virtual unsigned long getEventHeaderLength() const = 0;

		/**
		* Return the specified length of the message parameters 
		* as defined in the first bytes of the message.
		*/
		unsigned long getParameterLength() const;

		/**
		 * Return the rawData
		 */
		std::vector<unsigned char> getRawData() const;

		
		std::string getBufferDump();
        size_t getBufferLength() const;

		
	protected:
		std::vector<unsigned char>   m_buffer;

		SRadioPDSBuffer m_structRadioPDS;

		/**
		 * Verify CheckSum
		 */
		void verifyChecksum(); 

		unsigned short getWord( unsigned short position ) const;
		unsigned char getByte( unsigned short position ) const;


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

		/**
		 * Checksum position
		 */
		static const unsigned short CHECKSUM_POSITION;
		
		std::string m_clientName;

		

	};

	inline size_t IPDSEvent::getBufferLength() const
	{
		return m_buffer.size();
	}

	typedef boost::shared_ptr< IPDSEvent > IPDSEventPtr;
}
#endif //end Of IPDS_EVENT_H