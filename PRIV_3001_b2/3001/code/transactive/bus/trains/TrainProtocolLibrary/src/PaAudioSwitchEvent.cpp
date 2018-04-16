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
#include "bus/trains/TrainProtocolLibrary/src/PaAudioSwitchEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaAudioSwitchEvent::MESSAGE_TYPE = 23;
    const unsigned short PaAudioSwitchEvent::EXPECTED_MESSAGE_LENGTH = 7;


    PaAudioSwitchEvent::PaAudioSwitchEvent( const TrainMessageData& data,
                                            const std::string& sendingTsi,
                                            const std::string& destinationTsi )
        : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PaAudioSwitchEvent" );
        FUNCTION_EXIT;
    }


    PaAudioSwitchEvent::~PaAudioSwitchEvent()
    {
        FUNCTION_ENTRY( "~PaAudioSwitchEvent" );
        FUNCTION_EXIT;
    }


    const TrainMessageType PaAudioSwitchEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short PaAudioSwitchEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}
