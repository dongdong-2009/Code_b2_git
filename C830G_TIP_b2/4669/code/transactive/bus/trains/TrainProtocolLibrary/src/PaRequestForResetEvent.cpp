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
#include "bus/trains/TrainProtocolLibrary/src/PaRequestForResetEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaRequestForResetEvent::MESSAGE_TYPE = 22;
    const unsigned short PaRequestForResetEvent::EXPECTED_MESSAGE_LENGTH = 8;
    const unsigned short PaRequestForResetEvent::ANNOUNCEMENT_ID_POSITION = 5;


    PaRequestForResetEvent::PaRequestForResetEvent( const TrainMessageData& data,
                                                    const std::string& sendingTsi,
                                                    const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PaRequestForResetEvent" );

        m_announcementId = getByte( ANNOUNCEMENT_ID_POSITION );

        FUNCTION_EXIT;
    }


    PaRequestForResetEvent::~PaRequestForResetEvent()
    {
        FUNCTION_ENTRY( "~PaRequestForResetEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PaRequestForResetEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short PaRequestForResetEvent::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_announcementId;
    }


    std::string PaRequestForResetEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short PaRequestForResetEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

} //end of namespace
