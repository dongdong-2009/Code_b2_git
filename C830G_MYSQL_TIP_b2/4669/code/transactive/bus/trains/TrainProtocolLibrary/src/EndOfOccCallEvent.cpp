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
#include "bus/trains/TrainProtocolLibrary/src/EndOfOccCallEvent.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType EndOfOccCallEvent::MESSAGE_TYPE = 70;
    const unsigned short EndOfOccCallEvent::EXPECTED_MESSAGE_LENGTH = 7;


    EndOfOccCallEvent::EndOfOccCallEvent( const TrainMessageData& data,
                                          const std::string& sendingTsi,
                                          const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "EndOfOccCallEvent" );
        FUNCTION_EXIT;
    }


    EndOfOccCallEvent::~EndOfOccCallEvent()
    {
        FUNCTION_ENTRY( "~EndOfOccCallEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType EndOfOccCallEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    unsigned short EndOfOccCallEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

} //end of namespace
