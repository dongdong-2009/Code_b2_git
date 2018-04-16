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
#include "bus/trains/TrainProtocolLibrary/src/PaResetEvent.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
    const TrainMessageType PaResetEvent::MESSAGE_TYPE = 14;
    const unsigned short PaResetEvent::EXPECTED_MESSAGE_LENGTH = 8;
    const unsigned short PaResetEvent::ANNOUNCEMENT_ID_POSITION = 5;


    PaResetEvent::PaResetEvent( const TrainMessageData& data,
                                const std::string& sendingTsi,
                                const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PaResetEvent" );

        m_announcementId = getByte( ANNOUNCEMENT_ID_POSITION );

        FUNCTION_EXIT;
    }


    PaResetEvent::~PaResetEvent()
    {
        FUNCTION_ENTRY( "~PaResetEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PaResetEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short PaResetEvent::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_announcementId;

    }


    std::string PaResetEvent::getLogString() const
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


    unsigned short PaResetEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}  //end of namespace
