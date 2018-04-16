
/*#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventBegin.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventFrameRepeat.h"
*/

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventBegin.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventFrameRepeat.h"


#include "core/utilities/src/DebugUtil.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	const unsigned short RadioPDSEvent::EVENT_FLAG_POSITION               = 0;
	const unsigned short RadioPDSEvent::EVENT_MESSAGE_TYPE_POSITION		  = 1;
	const unsigned short RadioPDSEvent::EVENT_HEADER_TRANSACTION_POSITION = 1;
	const unsigned short RadioPDSEvent::EVENT_TRAIN_ID_POSITION			  = 2;
	const unsigned short RadioPDSEvent::EVENT_LENGTH_POSITION			  = 3;
	const unsigned short RadioPDSEvent::EVENT_ORIGIN_POSITION			  = 4;
	const unsigned short RadioPDSEvent::EVENT_DESTINATION_POSITION		  = 5;
	const unsigned short RadioPDSEvent::EVENT_DATA_FIRST_POSITION		  = 6;
	const unsigned short RadioPDSEvent::EVENT_STATUS_POSITION			  = 6;
	const unsigned short RadioPDSEvent::EVENT_HEADER_LENGTH				  = 6;
	const unsigned char RadioPDSEvent::EVENT_MESSAGE_TYPE_BEGIN           = 40;
	const unsigned char RadioPDSEvent::EVENT_MESSAGE_TYPE_FRAME_REPEAT    = 41;


	RadioPDSEvent::RadioPDSEvent( const std::string& clientName, 
		                          const std::vector<unsigned char>& data )

		: IPDSEvent(),
		  m_eventReader(NULL),
		  eventLength(0)
	{
		FUNCTION_ENTRY( "RadioPDSEvent" );

		setClientName(clientName);
	
		m_buffer = data;
		verifyChecksum();
		
		m_structRadioPDS.ucMsgID         = getEventType();
		m_structRadioPDS.ucTrainID       = getTrainId();
		m_structRadioPDS.ucOrigin        = getOrigin();
		m_structRadioPDS.radioPDSMessage = getDataHeader();
		
		FUNCTION_EXIT;
	}

	RadioPDSEvent::~RadioPDSEvent()
	{
		FUNCTION_ENTRY( "~RadioPDSEvent" );

		if( m_eventReader != NULL )
		{
			delete m_eventReader;
			m_eventReader = NULL;
		}
		FUNCTION_EXIT;
	}

	RadioPDSEvent::RadioPDSEvent(const RadioPDSEvent& RadioPDSEvent)
		: IPDSEvent( RadioPDSEvent ),
		  m_eventReader(NULL),
		  eventLength(0)
	{
		FUNCTION_ENTRY( "RadioPDSEvent" );
		FUNCTION_EXIT;
	}

	/**
	 * Resets the command to be empty.
	 */
	void RadioPDSEvent::clear()
	{
		FUNCTION_ENTRY( "clear" );
		IPDSEvent::clear();
		if ( m_eventReader != NULL )
		{
			delete m_eventReader;
			m_eventReader = NULL;
		}
		FUNCTION_EXIT;
	}

	/**
	 * This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
	 */
	bool RadioPDSEvent::isComplete() const
	{
		FUNCTION_ENTRY( "isComplete" );
		FUNCTION_EXIT;

		//Check message size is complete
		if( m_buffer.size() < getEventLength() )
			return false;
		else
			return true;
	}
	
	/**
	 * Returns the Event type
	 */
	RadioPDSEvent::EventType RadioPDSEvent::getEventType()const
	{
		FUNCTION_ENTRY( "getEventType" );
		FUNCTION_EXIT;
		return readBufferByte(EVENT_MESSAGE_TYPE_POSITION);
	}

	/**
	 * Set Destination TSI
	 */
	void RadioPDSEvent::setDestinationTsi( const std::string& destinationTsi )
	{
		FUNCTION_ENTRY( "setDestinationTsi" );
		m_destinationTsi = destinationTsi;
		FUNCTION_EXIT
	}

	/**
	 * Returns the DestinationTsi
	 */
	std::string RadioPDSEvent::getDestination() const
	{
		FUNCTION_ENTRY( "getDestination" );
		FUNCTION_EXIT;
		return m_destinationTsi;
	}
	/**
	 * Returns the Event Origin
	 */
	RadioPDSEvent::EventOrigin RadioPDSEvent::getOrigin() const
	{
		FUNCTION_ENTRY( "getOrigin" );
		FUNCTION_EXIT;
		return readBufferByte(EVENT_ORIGIN_POSITION);
	}

	/**
	 * Returns the train ID
	 */
	RadioPDSEvent::TrainIDType RadioPDSEvent::getTrainId() const
	{
		FUNCTION_ENTRY( "getTrainId" );
		FUNCTION_EXIT;
		return readBufferByte(EVENT_TRAIN_ID_POSITION);
	}

	std::vector<unsigned char> RadioPDSEvent::getDataHeader() const
	{
		FUNCTION_ENTRY( "getDataHeader" );
		std::vector<unsigned char> dataHeader;
		for (int position = EVENT_DATA_FIRST_POSITION; 
			 position < m_buffer.size(); ++position )
		{
			dataHeader.push_back(m_buffer[position]);
		}
		return dataHeader;
		
	}
	/**
	 * Return the specified length of the message as defined in the first bytes.
	 */
	unsigned long RadioPDSEvent::getEventLength() const
	{
		FUNCTION_ENTRY( "getEventLength" );
		FUNCTION_EXIT;

		//return readBufferByte(EVENT_LENGTH_POSITION);
		return eventLength;
	}

	/**
	 * Returns the status code for the event
	 */
	RadioPDSEvent::StatusType RadioPDSEvent::getStatus() const
	{
		FUNCTION_ENTRY( "getStatus" );
		FUNCTION_EXIT;

		return readBufferByte(EVENT_STATUS_POSITION);
	}

	unsigned long RadioPDSEvent::getEventHeaderLength() const
	{
		FUNCTION_ENTRY( "getEventHeaderLength" );
		FUNCTION_EXIT;
		return EVENT_HEADER_LENGTH;
	}

	

	/**
	 * Set the length of the Event
	 */
	void RadioPDSEvent::setEventLength(unsigned int lengthEvent)
	{
		FUNCTION_ENTRY( "setEventLength" );
		FUNCTION_EXIT;
		eventLength = lengthEvent;
	}

	/**
	 * Returns the instance of RadioPDSEventReader.
	 */
	RadioPDSEventReader* RadioPDSEvent::getEventReader()
	{
		FUNCTION_ENTRY( "getEventReader" );
		
		
		if ( m_eventReader==NULL )
		{
			m_eventReader = createEventPDSReader();
		}

		FUNCTION_EXIT;
		return m_eventReader;
	}

	RadioPDSEventReader* RadioPDSEvent::createEventPDSReader()
	{
		FUNCTION_ENTRY( "createEventPDSReader" );
		if(!isComplete())
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				"Mismatch in EventLength and RecievedLength. transaction: %lu, event: %lu",
				getTransactionId(), getEventType());
			FUNCTION_EXIT;
			return NULL;
		}

		RadioPDSEventReader* eventReader = NULL;

		RadioPDSEventPtr data( new RadioPDSEvent(*this) );
		switch( getEventType() )
		{
			case EVENT_MESSAGE_TYPE_BEGIN:
				eventReader = new RadioPDSEventBegin(data);
				break;

			case EVENT_MESSAGE_TYPE_FRAME_REPEAT:
				eventReader = new RadioPDSEventFrameRepeat(data);
				break;
			default:;
		}

		if (eventReader != NULL)
		{
			if (!eventReader->isEventReaderValid())
			{
				delete eventReader;
				eventReader = NULL;
			}
		}

		FUNCTION_EXIT;
		return eventReader;

	}

	RadioPDSEvent::TransactionIDType RadioPDSEvent::getTransactionId() const
	{
		FUNCTION_ENTRY( "getTransactionId" );
		FUNCTION_EXIT;
		return readBufferShort(EVENT_HEADER_TRANSACTION_POSITION);
	}

	void RadioPDSEvent::setClientName(const std::string& clientName )
	{
		FUNCTION_ENTRY( "setClientName" );
		m_clientName = clientName;
		FUNCTION_EXIT;
	}

	std::string RadioPDSEvent::getClientName() const
	{
		FUNCTION_ENTRY( "getClientName" );
		FUNCTION_EXIT;
		return m_clientName;
	}

	SRadioPDSBuffer RadioPDSEvent::getRadioPDSBuffer() const
	{
		FUNCTION_ENTRY( "getRadioPDSBuffer" );
		FUNCTION_EXIT;
		return m_structRadioPDS;
	}
}
