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
#include "bus/trains/TrainProtocolLibrary/src/AbstractCctvCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{

    AbstractCctvCommand::AbstractCctvCommand( CommonTypes::TrainIdType trainId,
                                              unsigned int length,
                                              TrainMessageType commandType,
                                              unsigned char origin )
            : TimsCommand( trainId, length, commandType, origin, true )
    {
        FUNCTION_ENTRY( "AbstractCctvCommand" );

        // Adds a CctvCommandReceivedEventReader::getStaticType() as a response
        addResponseType( CctvCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    AbstractCctvCommand::~AbstractCctvCommand()
    {
        FUNCTION_ENTRY( "~AbstractCctvCommand" );
        FUNCTION_EXIT;
    }


    CctvCommandReceivedEventPtr AbstractCctvCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        CctvCommandReceivedEventPtr cctvCommandReceivedEvent;

        if ( getResponse() )
        {
            cctvCommandReceivedEvent = boost::dynamic_pointer_cast< CctvCommandReceivedEvent > ( getResponse() );
        }

        FUNCTION_EXIT;
        return cctvCommandReceivedEvent;
    }
}
