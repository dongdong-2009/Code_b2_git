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
#include "bus/trains/TrainProtocolLibrary/src/PaReadyForLiveAnnouncementEvent.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    const TrainMessageType PaReadyForLiveAnnouncementEvent::MESSAGE_TYPE = 20;
    const unsigned short PaReadyForLiveAnnouncementEvent::EXPECTED_MESSAGE_LENGTH = 8;
    const unsigned short PaReadyForLiveAnnouncementEvent::ANNOUNCEMENT_ID_POSITION = 5;


    PaReadyForLiveAnnouncementEvent::PaReadyForLiveAnnouncementEvent( const TrainMessageData& data, const std::string& sendingTsi, const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PaReadyForLiveAnnouncementEvent" );

        m_announcementId = getByte( ANNOUNCEMENT_ID_POSITION );

        FUNCTION_EXIT;
    }


    PaReadyForLiveAnnouncementEvent::~PaReadyForLiveAnnouncementEvent()
    {
        FUNCTION_ENTRY( "~PaReadyForLiveAnnouncementEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PaReadyForLiveAnnouncementEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;

    }


    unsigned short PaReadyForLiveAnnouncementEvent::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_announcementId;

    }


    std::string PaReadyForLiveAnnouncementEvent::getLogString() const
    {
        FUNCTION_ENTRY( "PaReadyForLiveAnnouncementEvent" );

        // appends more information in the format:
        // "[AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short PaReadyForLiveAnnouncementEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;

    }
}
