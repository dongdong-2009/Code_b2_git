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
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const TrainMessageType PecActivatedEvent::MESSAGE_TYPE = 53;


    PecActivatedEvent::PecActivatedEvent( const TrainMessageData& data,
                                          const std::string& sendingTsi,
                                          const std::string& destinationTsi )
            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecActivatedEvent" );
        FUNCTION_EXIT;
    }


    PecActivatedEvent::~PecActivatedEvent()
    {
        FUNCTION_ENTRY( "~PecActivatedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecActivatedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;

    }

}

