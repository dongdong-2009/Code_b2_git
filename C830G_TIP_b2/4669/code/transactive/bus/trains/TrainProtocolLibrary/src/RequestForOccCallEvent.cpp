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
#include "bus/trains/TrainProtocolLibrary/src/RequestForOccCallEvent.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType RequestForOccCallEvent::MESSAGE_TYPE = 63;
    const unsigned short RequestForOccCallEvent::EXPECTED_MESSAGE_LENGTH = 7;


    RequestForOccCallEvent::RequestForOccCallEvent( const TrainMessageData& data,
                                                    const std::string& sendingTsi,
                                                    const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "RequestForOccCallEvent" );
        FUNCTION_EXIT;
    }


    RequestForOccCallEvent::~RequestForOccCallEvent()
    {
        FUNCTION_ENTRY( "~RequestForOccCallEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType RequestForOccCallEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short RequestForOccCallEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}  //end of namespace

