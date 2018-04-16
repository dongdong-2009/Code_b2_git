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
#include "bus/trains/TrainProtocolLibrary/src/AudioCabToCabEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType AudioCabToCabEvent::MESSAGE_TYPE = 26;
    const unsigned short AudioCabToCabEvent::EXPECTED_MESSAGE_LENGTH = 7;


    AudioCabToCabEvent::AudioCabToCabEvent( const TrainMessageData& data,
                                            const std::string& sendingTsi,
                                            const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "AudioCabToCabEvent" );
        FUNCTION_EXIT;
    }


    AudioCabToCabEvent::~AudioCabToCabEvent()
    {
        FUNCTION_ENTRY( "~AudioCabToCabEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType AudioCabToCabEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short AudioCabToCabEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}

