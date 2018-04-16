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
#include "bus/trains/TrainProtocolLibrary/src/PecSelectedByDriverEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const TrainMessageType PecSelectedByDriverEvent::MESSAGE_TYPE = 54;


    PecSelectedByDriverEvent::PecSelectedByDriverEvent( const TrainMessageData& data,
                                                        const std::string& sendingTsi,
                                                        const std::string& destinationTsi )
            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecSelectedByDriverEvent" );
        FUNCTION_EXIT;
    }

    
    PecSelectedByDriverEvent::~PecSelectedByDriverEvent()
    {
        FUNCTION_ENTRY( "~PecSelectedByDriverEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecSelectedByDriverEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }

}
