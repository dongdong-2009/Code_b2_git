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


#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{

    const unsigned short TrainCommand::CHECKSUM_POSITION = 2;

    TrainCommand::TrainCommand( CommonTypes::TrainIdType trainId,
                                unsigned int length,
                                TrainMessageType commandType )
          : m_trainId( trainId ),
            m_commandType( commandType )
    {
        FUNCTION_ENTRY( "TrainCommand" );

        // Initialise members, zero out the data vector
        m_data.insert( m_data.begin(), length, 0 );

        FUNCTION_EXIT;
    }

    
    TrainCommand::~TrainCommand()
    {
        FUNCTION_ENTRY( "TrainCommand" );
        FUNCTION_EXIT;
    }


    TrainMessageType TrainCommand::getCommandType() const
    {
        FUNCTION_ENTRY( "getCommandType" );
        FUNCTION_EXIT;
        return m_commandType;
    }


    const TrainMessageTypeList& TrainCommand::getResponseTypes() const
    {
        FUNCTION_ENTRY( "getResponseTypes" );
        FUNCTION_EXIT;
        return m_responseTypes;
    }


    CommonTypes::TrainIdType TrainCommand::getTrainId() const
    {
        FUNCTION_ENTRY( "getTrainId" );
        FUNCTION_EXIT;
        return m_trainId;
    }


    const TrainMessageData& TrainCommand::getMessageData()
    {
        FUNCTION_ENTRY( "getData" );
        // calculates the checksum
        unsigned short checkSum = calculateChecksum();
		
        // sets it in the data
        setWord( m_data.size() - CHECKSUM_POSITION, checkSum );

        FUNCTION_EXIT;
        return m_data;
    }


    unsigned int TrainCommand::getLength() const
    {
        FUNCTION_ENTRY( "getLength" );
        FUNCTION_EXIT;
        return m_data.size();
    }


    TrainCommand::TrainResponseResult TrainCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        
        // check if the response is for this train
        if ( getTrainId() != response->getTrainId() )
        {
            FUNCTION_EXIT;
            return TrainResponseResult( false, false );
        }

        // Get the valid response types from this train command object
        const TrainMessageTypeList& messageSet = getResponseTypes();

        // get the message type from the event
        TrainMessageTypeList::const_iterator iter = messageSet.find( response->getEventType() );

        // if the response type is not in the list of valid responses for this command
        if ( iter == messageSet.end() )
        {
            FUNCTION_EXIT;
            return TrainResponseResult( false, false );
        }

        // if the event type is within the valid response types for this command,
        // set m_response to the given response event
        m_response = response;

        FUNCTION_EXIT;
        return TrainResponseResult( true, true );
    }


    void TrainCommand::resetResponse()
    {
        FUNCTION_ENTRY( "resetResponse" );

        m_response.reset();

        FUNCTION_EXIT;
    }


    TrainEventPtr TrainCommand::getResponse()
    {
        FUNCTION_ENTRY( "getResponse" );
        FUNCTION_EXIT;
        return m_response;
    }


    std::string TrainCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // Creates a log string of the format:
        //"[TrainID=XX][Command=XX][ReplyEvent=XX, YY,ZZ or None]
        std::ostringstream logStream;
        logStream << "[TrainId=" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( m_trainId )
        << "][Command=" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( m_commandType )
        << "][ReplyEvent=";

        if ( m_responseTypes.size() > 0 )
        {
            std::set<TrainMessageType>::const_iterator iter = m_responseTypes.begin();
            logStream << static_cast<unsigned int>( *iter );
            ++iter;

            for ( ; iter != m_responseTypes.end(); ++iter )
            {
                logStream << "," << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( *iter );
            }
        }
        else
        {
            logStream << "None";
        }

        logStream << "]" ;

        FUNCTION_EXIT;
        return logStream.str();
    }


    void TrainCommand::addResponseType( TrainMessageType validResponseType )
    {
        FUNCTION_ENTRY( "addResponseType" );

        m_responseTypes.insert( validResponseType );

        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult::TrainResponseResult( bool complete, bool consumed )
      : transactionComplete( complete ),
        eventConsumed( consumed )
          
    {
        FUNCTION_ENTRY( "TrainResponseResult" );
        FUNCTION_EXIT;
    }

	//hongzhi, move checksum from timsCommand to Traincommand
    unsigned short TrainCommand::calculateChecksum()
    {
        FUNCTION_ENTRY( "calculateChecksum" );
        FUNCTION_EXIT;
        return FrameCheckSequence::calculate( m_data, m_data.size() - CHECKSUM_POSITION );
    }

	//hongzhi, move checksum from timsCommand to Traincommand
    void TrainCommand::setByte( unsigned short position, unsigned char value )
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
	
	//hongzhi, move checksum from timsCommand to Traincommand
    void TrainCommand::setWord( unsigned short position, unsigned short value )
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
}
