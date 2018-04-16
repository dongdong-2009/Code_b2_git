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
#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType AtcEvent::MESSAGE_TYPE = 62;


    AtcEvent::AtcEvent( const TrainMessageData& data,
                        const std::string& sendingTsi,
                        const std::string& destinationTsi )
            : AbstractTableEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "AtcEvent" );
        FUNCTION_EXIT;
    }


    AtcEvent::~AtcEvent()
    {
        FUNCTION_ENTRY( "~AtcEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType AtcEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }

}
