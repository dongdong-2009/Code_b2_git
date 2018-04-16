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
#include "bus/trains/TrainProtocolLibrary/src/PecReadyForConversationEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const TrainMessageType PecReadyForConversationEvent::MESSAGE_TYPE = 56;


    PecReadyForConversationEvent::PecReadyForConversationEvent( const TrainMessageData& data,
                                                                const std::string& sendingTsi,
                                                                const std::string& destinationTsi )
            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecReadyForConversationEvent" );
        FUNCTION_EXIT;
    }


    PecReadyForConversationEvent::~PecReadyForConversationEvent()
    {
        FUNCTION_ENTRY( "~PecReadyForConversationEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecReadyForConversationEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;

    }
}

