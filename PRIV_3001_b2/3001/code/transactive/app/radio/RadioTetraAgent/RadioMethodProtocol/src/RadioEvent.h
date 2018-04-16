#pragma once

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/ITcpEvent.h"

namespace TA_IRS_App
{
	class RadioEventReader;

class RadioEvent : public ITcpEvent 
{
public:
	RadioEvent(void);
	RadioEvent(const RadioEvent& radioEvent);
	typedef unsigned short   TransactionIDType;
    typedef unsigned char   TrainIDType; 
	typedef unsigned char   StatusType;
	typedef unsigned char   EventType;

public:
	virtual ~RadioEvent(void);
		public:
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
		* Return the specified length of the message as defined in the first bytes.
		*/
		virtual unsigned long getEventLength() const;
		/**
		* Set the specified length of the messages.
		*/
		virtual void setEventLength(unsigned int eventLength);
		/**
		* Return the specified length of the message header
		*/
		virtual unsigned long getEventHeaderLength() const;
		/**
		* Provides access to the appropriate RadioEventReader for the incoming message.  The reader will be automatically destroyed when the RadioEvent is destroyed.
		*/
		RadioEventReader* getEventReader();
		/**
		* Return the Message ID of the specified position
		*/
		std::string getMessageID(int startPosition) const;
		/**
		* Check if the message ID is correct
		*/
		bool checkMessageID(std::string eventMessageID, std::string checkMessage) const;
		

private:
		/**
		* Create an event reader that is correct for the type of message.
		*/
		RadioEventReader* createEventReader();

		RadioEventReader*            m_eventReader;
		unsigned int eventLength;
};
};
