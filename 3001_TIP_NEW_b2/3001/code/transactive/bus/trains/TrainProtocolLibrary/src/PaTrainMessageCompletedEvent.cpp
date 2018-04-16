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
#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType PaTrainMessageCompletedEvent::MESSAGE_TYPE = 24;
    const unsigned short PaTrainMessageCompletedEvent::EXPECTED_MESSAGE_LENGTH = 9;
    const unsigned short PaTrainMessageCompletedEvent::COMPLETION_STATUS_POSITION = 5;
    const unsigned short PaTrainMessageCompletedEvent::ANNOUNCEMENT_ID_POSITION = 6;


    PaTrainMessageCompletedEvent::PaTrainMessageCompletedEvent( const TrainMessageData& data,
                                                                const std::string& sendingTsi,
                                                                const std::string& destinationTsi )
        : TimsEvent( data, sendingTsi, destinationTsi )
    {
        // read the fields
        m_completionStatus = static_cast< ProtocolPaTypes::EMessageCompletionStatus >( getByte( COMPLETION_STATUS_POSITION ) );
        m_announcementId = getByte( ANNOUNCEMENT_ID_POSITION );

        // validate the completion status
        switch ( m_completionStatus )
        {
            // valid values

            case ProtocolPaTypes::Successful:
                // fall through

            case ProtocolPaTypes::InterruptedByOcc:
                // fall through

            case ProtocolPaTypes::InterruptedByPsc:
                // fall through

            case ProtocolPaTypes::InterruptedByDriver:
                break;

            // invalid values
            default:
                std::ostringstream statusStr;
                statusStr << static_cast<int>( m_completionStatus );
                TA_THROW( ProtocolException( "Invalid status", "Status 4", statusStr.str() ) );
        }
    }


    PaTrainMessageCompletedEvent::~PaTrainMessageCompletedEvent()
    {
        FUNCTION_ENTRY( "~PaTrainMessageCompletedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PaTrainMessageCompletedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    ProtocolPaTypes::EMessageCompletionStatus PaTrainMessageCompletedEvent::getCompletionStatus() const
    {
        FUNCTION_ENTRY( "getCompletionStatus" );
        FUNCTION_EXIT;
        return m_completionStatus;

    }


    unsigned short PaTrainMessageCompletedEvent::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_announcementId;
    }


    std::string PaTrainMessageCompletedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Status=X][AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[Status=" << static_cast<int>( m_completionStatus ) << "]"
                  << "[AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short PaTrainMessageCompletedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

} //end of namespace
