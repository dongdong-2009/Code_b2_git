/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{

    const unsigned short TimsEvent::MESSAGE_TYPE_POSITION = 1;
    const unsigned short TimsEvent::TRAIN_ID_POSITION = 2;
    const unsigned short TimsEvent::MESSAGE_LENGTH_POSITION = 3;
    const unsigned short TimsEvent::ORIGIN_POSITION = 4;
	const unsigned short TimsEvent::PEC_TRAIN_SOURCE_POSITION = 6;
    const unsigned short TimsEvent::CHECKSUM_POSITION = 2;
    const unsigned char TimsEvent::MAX_TRAIN_ID = 150;
    const unsigned short TimsEvent::MAX_MESSAGE_LENGTH = 140;


    TimsEvent::~TimsEvent()
    {
        FUNCTION_ENTRY( "~TimsEvent" );
        FUNCTION_EXIT;
    }


    TimsEvent::TimsEvent( const TrainMessageData& data,
                          const std::string& sendingTsi,
                          const std::string& destinationTsi )
            : TrainEvent( data,
                          sendingTsi,
                          destinationTsi ),
              m_trainId( 0 ),
              m_messageType( 0 ),
              m_length( 0 ),
              m_origin( ProtocolCommonTypes::MpuMain ),
			  m_pecTrainSource( PecTypes::LocalTrain ),
              m_crc( 0 )
    {
        FUNCTION_ENTRY( "TimsEvent" );

        // check the data integrity
        verifyChecksum( data );

        m_messageType = getByte( MESSAGE_TYPE_POSITION );
        m_trainId = getByte( TRAIN_ID_POSITION );
        m_length = getByte( MESSAGE_LENGTH_POSITION );
        m_origin = static_cast<ProtocolCommonTypes::ETimsOrigin>( getByte( ORIGIN_POSITION ) );
		m_pecTrainSource = static_cast<PecTypes::EPecTrainSource>( getByte( PEC_TRAIN_SOURCE_POSITION ) );
        m_crc = getWord( data.size() - CHECKSUM_POSITION );

        // validate the length field against the data length
        if ( getDataLength() != m_length )
        {
            std::ostringstream lengthstr;
            lengthstr << "Actual: " << getDataLength()
                      << " Specified: " << static_cast<unsigned int>( m_length );

            TA_THROW( ProtocolException( "Message length field incorrect", "Length", lengthstr.str() ) );
        }

        // validate the length is what the expected length is for this message type
        // only validate the length if the concrete class returns a value
        unsigned short expectedLength = getExpectedMessageLength();

        if ( ( expectedLength <= MAX_MESSAGE_LENGTH ) &&
             ( getDataLength() != expectedLength ) )
        {
            std::ostringstream expectedLengthstr;
            expectedLengthstr << "Actual: " << static_cast<unsigned int>( m_length )
                              << " Expected: " << getExpectedMessageLength();

            TA_THROW( ProtocolException( "Message Length incorrect", "Length", expectedLengthstr.str() ) );
        }

        // check the train ID is within the valid range
        if ( ( m_trainId == 0 ) ||
             ( m_trainId > MAX_TRAIN_ID ) )
        {
            std::ostringstream trainIdstr;
            trainIdstr << m_trainId;

            TA_THROW( ProtocolException( "Train ID invalid", "TrainId", trainIdstr.str() ) );
        }

        // validate the origin field is correct
        if ( ( m_origin != ProtocolCommonTypes::MpuMain ) &&
             ( m_origin != ProtocolCommonTypes::MpuBackup )&&
			 ( m_origin != ProtocolCommonTypes::DriverCallAcsu) )
        {
            std::ostringstream originstr;
            originstr << static_cast<int>( m_origin );

            TA_THROW( ProtocolException( "Invalid message origin", "origin", originstr.str() ) );
        }

        FUNCTION_EXIT;
    }


    CommonTypes::TrainIdType TimsEvent::getTrainId() const
    {
        FUNCTION_ENTRY( "getTrainId" );
        FUNCTION_EXIT;
        return m_trainId;
    }


    TrainMessageType TimsEvent::getEventType() const
    {
        FUNCTION_ENTRY( "getEventType" );
        FUNCTION_EXIT;
        return m_messageType;
   }


    ProtocolCommonTypes::ETimsOrigin TimsEvent::getOrigin() const
    {
        FUNCTION_ENTRY( "getOrigin" );
        FUNCTION_EXIT;
        return m_origin;
    }

	PecTypes::EPecTrainSource TimsEvent::getPecTrainSource() const
	{
		FUNCTION_ENTRY( "getPecTrainSource" );
		FUNCTION_EXIT;
		return m_pecTrainSource;
	}


    std::string TimsEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // Append a string of the format:
        // "[Origin=XX]" to the parent log string
        std::ostringstream logStream;
        logStream << TrainEvent::getLogString();
        logStream << "[Origin=" << std::setw(2) << std::setfill('0')
                  << static_cast<int>( m_origin )
				  << "] [pecTrainSource=" << std::setw(2) << std::setfill('0')
				  << static_cast<int>( m_pecTrainSource ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned char TimsEvent::getByte( unsigned short position ) const
    {
        FUNCTION_ENTRY( "getByte" );

        if ( position < getDataLength() )
        {
            const TrainMessageData& data = getData();

            return data[position];
        }
        else
        {
            std::ostringstream positionstr;
            positionstr << "Requested: " << position
                        << "Length: " << getDataLength();

            TA_THROW( ProtocolException( "Invalid byte position", "position", positionstr.str() ) );
        }

        FUNCTION_EXIT;
        return 0;
    }


    unsigned short TimsEvent::getWord( unsigned short position ) const
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


    unsigned long TimsEvent::getLong( unsigned short position ) const
    {
        FUNCTION_ENTRY( "getLong" );

        unsigned long resultWord = 0;

        // little endian ordering on multi bytes
        unsigned char lowestByte = getByte( position );
        unsigned char lowerByte = getByte( position + 1 );
        unsigned char higherByte = getByte( position + 2 );
        unsigned char highestByte = getByte( position + 3 );

        resultWord = (( static_cast<unsigned long>(highestByte) << 24 )
            | ( static_cast<unsigned long>(higherByte) << 16 ) 
            | ( static_cast<unsigned long>(lowerByte) << 8 ) 
            | static_cast<unsigned long>(lowestByte) );

        FUNCTION_EXIT;
        return resultWord;
    }


    TrainMessageData TimsEvent::getBytes( unsigned short position, unsigned short length ) const
    {
        FUNCTION_ENTRY( "getBytes" );

        TrainMessageData returnVector;

        // gets the bytes from position to position + length
        if ( static_cast<unsigned int>( position + length ) <= getDataLength() )
        {

            const TrainMessageData& data = getData();

            // iterator arithmetic should be at least as smart as pointer arithmetic...
            returnVector.insert( returnVector.begin(),
                                 data.begin() + position,
                                 data.begin() + position + length );
        }
        else
        {
            std::ostringstream positonstr;
            positonstr << "Requested " << length << " bytes starting at position " << position
                       << " from message length " << getDataLength();
            
            TA_THROW( ProtocolException( "Invalid byte position", "position", positonstr.str() ) );
        }

        FUNCTION_EXIT;
        return returnVector;
    }


    unsigned short TimsEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );

        // this is for a variable length message - the expected length wont be validated
        // if derived classes use this default implementation

        FUNCTION_EXIT;
        return MAX_MESSAGE_LENGTH + 1;
    }


    unsigned int TimsEvent::getUserDataLength() const
    {
        FUNCTION_ENTRY( "getUserDataLength" );
        FUNCTION_EXIT;
        return getDataLength() - CHECKSUM_POSITION;
    }


    void TimsEvent::verifyChecksum( const TrainMessageData& data ) const
    {
        FUNCTION_ENTRY( "verifyChecksum" );
	//	FUNCTION_EXIT;

       
        //extracts the checksum from the message
        unsigned short checksum = getWord( data.size() - CHECKSUM_POSITION );

        // GET the 16-bit Checksum
        // calculates a checksum on the message
        unsigned short calculatedCheckSum = FrameCheckSequence::calculate( data, data.size() - CHECKSUM_POSITION );

        //compares the 2, throws an exception if mismatch.

        if ( checksum != calculatedCheckSum )
        {
            std::ostringstream checsumstr;
            checsumstr << "Received " << std::hex << std::setw(4) << std::setfill('0') << checksum
                       << " Calculated " << std::hex << std::setw(4) << std::setfill('0') << calculatedCheckSum;

            TA_THROW( ProtocolException( "Invalid checksum", "Checksum", checsumstr.str() ) );
        }

        FUNCTION_EXIT;
    }
}
