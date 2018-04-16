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
#include "bus/trains/TrainProtocolLibrary/src/PecRequestForResetEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const TrainMessageType PecRequestForResetEvent::MESSAGE_TYPE = 57;


    PecRequestForResetEvent::PecRequestForResetEvent( const TrainMessageData& data,
                                                      const std::string& sendingTsi,
                                                      const std::string& destinationTsi )
            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecRequestForResetEvent" );
        FUNCTION_EXIT;
    }

    
    PecRequestForResetEvent::~PecRequestForResetEvent()
    {
        FUNCTION_ENTRY( "~PecRequestForResetEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecRequestForResetEvent::getStaticType()
    {
        FUNCTION_ENTRY( "PecRequestForResetEvent" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }

}
