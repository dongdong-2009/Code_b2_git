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


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"
#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{
    const unsigned short TimsCommand::MAX_MESSAGE_LENGTH = 140;
    const unsigned short TimsCommand::MESSAGE_FLAG_POSITION = 0;
    const unsigned char TimsCommand::MESSAGE_FLAG_VALUE = 0x7;
    const unsigned short TimsCommand::MESSAGE_TYPE_POSITION = 1;
    const unsigned short TimsCommand::TRAIN_ID_POSITION = 2;
    const unsigned short TimsCommand::LENGTH_POSITION = 3;
    const unsigned short TimsCommand::ORIGIN_POSITION = 4;
    const unsigned short TimsCommand::CHECKSUM_POSITION = 2;


    TimsCommand::TimsCommand( CommonTypes::TrainIdType trainId,
                              unsigned int length,
                              TrainMessageType commandType,
                              unsigned char origin,
                              bool responseExpected )
            : TrainCommand( trainId,
                            length,
                            commandType ),
              m_origin( origin )
    {
        FUNCTION_ENTRY( "TimsCommand" );

        setByte( MESSAGE_FLAG_POSITION, MESSAGE_FLAG_VALUE );
        setByte( MESSAGE_TYPE_POSITION, commandType );
        setByte( TRAIN_ID_POSITION, trainId );
        setByte( LENGTH_POSITION, static_cast<unsigned char>( length ) );
        setByte( ORIGIN_POSITION, origin );

        if ( true == responseExpected )
        {
            // adds a bad command to the list of responses
            addResponseType( BadCommandEvent::getStaticType() );
            
            // adds a changeover event to the list of responses
            addResponseType( ChangeoverStatusEvent::getStaticType() );
        }

        FUNCTION_EXIT;
    }


    TimsCommand::~TimsCommand()
    {
        FUNCTION_ENTRY( "~TimsCommand" );
        FUNCTION_EXIT;
    }


    TrainMessageData TimsCommand::getData()
    {
        FUNCTION_ENTRY( "getData" );

        // calculates the checksum
        unsigned short checkSum = calculateChecksum();

        // sets it in the data
        setWord( m_data.size() - CHECKSUM_POSITION, checkSum );

        FUNCTION_EXIT;
        return m_data;
    }


    std::string TimsCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << TrainCommand::getLogString();
        logStream << "[Origin=" << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>( m_origin ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    TrainCommand::TrainResponseResult TimsCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        BadCommandEventPtr badCommand = boost::dynamic_pointer_cast<BadCommandEvent>( response );

        // if this event is a bad command,
        // and the original command does not match this command number.
        if ( ( badCommand ) &&
             ( getCommandType() != badCommand->getOriginalCommand() ) )
        {
            // The response is not for this command

            FUNCTION_EXIT;
            return TrainResponseResult( false, false );
        }

        ChangeoverStatusEventPtr changeOverStatus = boost::dynamic_pointer_cast<ChangeoverStatusEvent>( response );

        // if this is a changeover event
        if ( changeOverStatus )
        {
            // then it ends all commands for this train
            TrainResponseResult result = TrainCommand::setResponse( response );

            // however, the event is not consumed
            result.eventConsumed = false;

            FUNCTION_EXIT;
            return result;
        }

        FUNCTION_EXIT;
        return TrainCommand::setResponse( response );
    }


    unsigned char TimsCommand::getOrigin() const
    {
        FUNCTION_ENTRY( "getOrigin" );
        FUNCTION_EXIT;
        return m_origin;
    }


    BadCommandEventPtr TimsCommand::getBadCommandResponse()
    {
        FUNCTION_ENTRY( "getBadCommandResponse" );

        BadCommandEventPtr badCommand;

        if ( getResponse() )
        {
            badCommand = boost::dynamic_pointer_cast<BadCommandEvent>( getResponse() );
        }

        FUNCTION_EXIT;
        return badCommand;
    }


    ChangeoverStatusEventPtr TimsCommand::getChangeoverStatusEventResponse()
    {
        FUNCTION_ENTRY( "getChangeoverStatusEventResponse" );

        ChangeoverStatusEventPtr changeoverStatusEvent;

        if ( getResponse() )
        {
            changeoverStatusEvent = boost::dynamic_pointer_cast<ChangeoverStatusEvent>( getResponse() );
        }

        FUNCTION_EXIT;
        return changeoverStatusEvent;
    }


    void TimsCommand::setByte( unsigned short position, unsigned char value )
    {
        FUNCTION_ENTRY( "setByte" );

        if ( position < m_data.size() )
        {
            m_data[position] = value;
        }
        else
        {
            std::ostringstream positionstream;
            positionstream << position;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "setByte() - specified position is greater than data.size() (%d >= %d)",
                         position, m_data.size() );

            TA_THROW( ProtocolException( "Invalid param" , "position", positionstream.str() ) );
        }

        FUNCTION_EXIT;
    }


    void TimsCommand::setWord( unsigned short position, unsigned short value )
    {
        FUNCTION_ENTRY( "setWord" );

        unsigned char lowerByte, upperByte;

        upperByte = static_cast< unsigned char >( value >> 8 ); // retrieve the higher byte
        lowerByte = static_cast< unsigned char >( value & 0xff ); // retrieve the lower byte

        // little endian, so the lower byte goes first.
        setByte( position, lowerByte );
        setByte( position + 1, upperByte );

        FUNCTION_EXIT;
    }


    void TimsCommand::setNewLength( unsigned char newLength )
    {
        FUNCTION_ENTRY( "setNewLength" );

        TA_ASSERT( newLength >= getLength(), "setNewLength can only be used to grow command length, not shrink it" );

        unsigned char length = m_data.size();
        m_data.insert( m_data.end(), newLength - length, 0 );

        setByte( LENGTH_POSITION, newLength );

        FUNCTION_EXIT;
    }


    unsigned short TimsCommand::calculateChecksum()
    {
        FUNCTION_ENTRY( "calculateChecksum" );
        FUNCTION_EXIT;
        return FrameCheckSequence::calculate( m_data, m_data.size() - CHECKSUM_POSITION );
    }

}  // end of namespace
