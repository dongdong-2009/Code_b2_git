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
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const TrainMessageType PecResetEvent::MESSAGE_TYPE = 52;
	//const unsigned short PecResetEvent::EXPECTED_MESSAGE_LENGTH = 10;
	//const unsigned short PecResetEvent::RESULT_POSITION = 7;


    PecResetEvent::PecResetEvent( const TrainMessageData& data,
                                  const std::string& sendingTsi,
                                  const std::string& destinationTsi )
            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecResetEvent" );
        FUNCTION_EXIT;
    }


    PecResetEvent::~PecResetEvent()
    {
        FUNCTION_ENTRY( "~PecResetEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecResetEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }
}

