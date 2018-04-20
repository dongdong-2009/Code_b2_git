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

#include "bus/trains/TrainProtocolLibrary/src/ChangeAreaCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"
#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType ChangeAreaCommand::MESSAGE_TYPE = 75;
    const unsigned short ChangeAreaCommand::MESSAGE_LENGTH = 8;
    const unsigned short ChangeAreaCommand::AREA_POSITION = 5;


    ChangeAreaCommand::ChangeAreaCommand( CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          ETrainArea area )
        : TimsCommand( trainId,  MESSAGE_LENGTH,  MESSAGE_TYPE, origin,  true )
    {
        FUNCTION_ENTRY( "ChangeAreaCommand" );

        m_area = area;

        setByte( AREA_POSITION, static_cast<unsigned char>( m_area ) );

        addResponseType( ChangeAreaCommandReceivedEvent::getStaticType() );
		//hongzhi, add 61,62 to receive command
		addResponseType( Oa1Event::getStaticType() );
		addResponseType( AtcEvent::getStaticType() );

        FUNCTION_EXIT;
    }

    
    ChangeAreaCommand::~ChangeAreaCommand()
    {
        FUNCTION_ENTRY( "~ChangeAreaCommand" );
        FUNCTION_EXIT;
    }


    TrainMessageType ChangeAreaCommand::getStaticMessageType()
    {
        FUNCTION_ENTRY( "getStaticMessageType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string ChangeAreaCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        //append the following:
        //"[Area=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Area=" << static_cast<int>( m_area ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    ChangeAreaCommandReceivedEventPtr ChangeAreaCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        ChangeAreaCommandReceivedEventPtr changeAreaCommandReceivedEvent;

        if ( getResponse() )
        {
            changeAreaCommandReceivedEvent = boost::dynamic_pointer_cast<ChangeAreaCommandReceivedEvent>( getResponse() );
        }

        FUNCTION_EXIT;
        return changeAreaCommandReceivedEvent;
    }

}
