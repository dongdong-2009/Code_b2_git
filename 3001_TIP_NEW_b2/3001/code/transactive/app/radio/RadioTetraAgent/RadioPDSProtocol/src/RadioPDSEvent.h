
/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   RadioPDSEvent.h
* @author:  Lucky
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* RadioPDSEvent module wrapped data coming from the client.
*/


#ifndef RadioPDSEvent_H
#define RadioPDSEvent_H


#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"
#include <string>

namespace TA_IRS_App
{
	
	class RadioPDSEventReader;

	class RadioPDSEvent : public IPDSEvent
	{
	public:

		RadioPDSEvent( const std::string& clientName, 
			           const std::vector<unsigned char>& data );
					 
		RadioPDSEvent(const RadioPDSEvent& RadioPDSEvent);

		typedef unsigned short   TransactionIDType;
		typedef unsigned char   EventType;
		typedef unsigned char   TrainIDType; 
		typedef unsigned char   StatusType;
		typedef unsigned char   EventOrigin;
		
		virtual ~RadioPDSEvent();

		/**
		* Resets the command to be empty.
		*/
		virtual void clear();
		/**
		* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
		*/
		virtual bool isComplete() const;

		/**
		* Get the session this event is intended for
		*/
		TrainIDType getTrainId() const;
		/**
		* Returns the transaction Id for the event which will correspond to the Transaction ID of the associated Method called.
		*/
		TransactionIDType getTransactionId() const;
		/**
		* Returns the status code for the event
		*/
		StatusType getStatus() const;
		/**
		* Returns boolean to show if status indicates more data to follow.
		*/
		bool isStatusMoreData() const;
		/**
		* Returns the status text for the event
		*/
		std::string getTextStatus() const;
		/**
		* Returns the radio event type for the event.
		*/
		EventType getEventType() const;

		/**
		 * Returns the Radio event Origin
		 */
		EventOrigin getOrigin() const;

		/**
		 * Returns the Data Header
		 */
		std::vector<unsigned char> getDataHeader() const;

		/**
		 * Return the struct Data of SRadioPDSBuffer
		 */
		SRadioPDSBuffer getRadioPDSBuffer() const;
		/**
		* Return the specified length of the message as defined in the first bytes.
		*/
		virtual unsigned long getEventLength() const;
		/**
		* Set the specified length of the messages.
		*/
		virtual void setEventLength(unsigned int lengthEvent);
		
		/**
		* Return the specified length of the message header
		*/
		virtual unsigned long getEventHeaderLength() const;

		/**
		* Provides access to the appropriate RadioEventReader for the incoming message.  The reader will be automatically destroyed when the RadioEvent is destroyed.
		*/
		RadioPDSEventReader* getEventReader();
		/**
		* Return the Message ID of the specified position
		*/
		std::string getMessageID(int startPosition) const;
		/**
		* Check if the message ID is correct
		*/
		bool checkMessageID(std::string eventMessageID, std::string checkMessage) const;

		/**
		* Create an event reader that is correct for the type of message.
		*/
		RadioPDSEventReader* createEventPDSReader();

		/**
		 * Set Client Name
		 */
		virtual void setClientName(const std::string& clientName );

		/**
		 * Get Client Name
		 */
		virtual std::string getClientName() const;

		/**
		 * Set DestinationTsi
		 */
		virtual void setDestinationTsi(const std::string& destinationTsi);
		
		/**
		 * Get Destination Tsi
		 */
		virtual std::string getDestination() const;

	private:
		

		RadioPDSEventReader*      m_eventReader;
		unsigned int eventLength;

		std::string m_clientName;
		std::string m_destinationTsi;

		
		/**
		 * Event Flag Position
		 */
		static const unsigned short EVENT_FLAG_POSITION;
		
		/**
		 * Event Message Type Position
		 */
		static const unsigned short EVENT_MESSAGE_TYPE_POSITION;

		/**
		 * Event Train ID Position
		 */
		static const unsigned short EVENT_TRAIN_ID_POSITION;

		/**
		 * Event Length Position
		 */
		static const unsigned short	EVENT_LENGTH_POSITION;

		/**
		 * Event Origin Position
		 */
		static const unsigned short EVENT_ORIGIN_POSITION;

		/**
		 * Event Destination Position
		 */
		static const unsigned short EVENT_DESTINATION_POSITION;

		/**
		 * Event Data first Position
		 */
		static const unsigned short EVENT_DATA_FIRST_POSITION;

		/**
		 * Event Status Position
		 */
		static const unsigned short EVENT_STATUS_POSITION;

		/**
		 * Header length
		 */
		static const unsigned short EVENT_HEADER_LENGTH;

		/**
		 * Event Header Transaction
		 */
		static const unsigned short EVENT_HEADER_TRANSACTION_POSITION;

		/**
		 * Event Message Type for Begin
		 */
		static const unsigned char EVENT_MESSAGE_TYPE_BEGIN;

		/**
		 * Event Message Type for Frame Repeat
		 */
		static const unsigned char EVENT_MESSAGE_TYPE_FRAME_REPEAT;

		

	};

	typedef boost::shared_ptr< RadioPDSEvent > RadioPDSEventPtr;
}
#endif