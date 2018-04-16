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
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType Oa1Event::MESSAGE_TYPE = 61;


    Oa1Event::Oa1Event( const TrainMessageData& data,
                        const std::string& sendingTsi,
                        const std::string& destinationTsi )
            : AbstractTableEvent( data,
                                  sendingTsi,
                                  destinationTsi )
    {
        FUNCTION_ENTRY( "Oa1Event" );
        FUNCTION_EXIT;
    }


    Oa1Event::~Oa1Event()
    {
        FUNCTION_ENTRY( "~Oa1Event" );
        FUNCTION_EXIT;
    }


    TrainMessageType Oa1Event::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }

}

