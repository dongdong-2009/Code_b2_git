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
#include "bus/trains/TrainProtocolLibrary/src/TrainEventFactoryImpl.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

// Common/periodic train events
#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"
#include "bus/trains/TrainProtocolLibrary/src/TestCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/BadCommandEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeAreaCommandReceivedEvent.h"

// Driver/OCC Call events
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/RequestForOccCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfOccCallEvent.h"

// CCTV events
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"

// TTIS events
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfTtisDownloadEvent.h"

// PA events
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaReadyForLiveAnnouncementEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"

// PEC events
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecReadyForConversationEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecSelectedByDriverEvent.h"

// Random events we dont really care about, handled anyway
#include "bus/trains/TrainProtocolLibrary/src/AudioCabToCabEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaAudioSwitchEvent.h"

// Should never be received, but handle anyway
#include "bus/trains/TrainProtocolLibrary/src/PecRequestForResetEvent.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


#define FIRST_HANDLE_MESSAGE_TYPE( MessageTypeClass ) if ( MessageTypeClass::getStaticType() == messageType )\
{\
    TrainEventPtr incomingEvent( new MessageTypeClass( data, radioId, destinationRadioId ) );\
    FUNCTION_EXIT;\
    return incomingEvent;\
}\

#define HANDLE_MESSAGE_TYPE( MessageTypeClass ) else FIRST_HANDLE_MESSAGE_TYPE( MessageTypeClass )


namespace TA_IRS_Bus
{

    TrainEventFactoryImpl::TrainEventFactoryImpl()
    {
        FUNCTION_ENTRY( "TrainEventFactoryImpl" );
        FUNCTION_EXIT;
    }


    TrainEventFactoryImpl::~TrainEventFactoryImpl()
    {
        FUNCTION_ENTRY( "~TrainEventFactoryImpl" );
        FUNCTION_EXIT;
    }


    TrainEventPtr TrainEventFactoryImpl::createEvent( const TrainMessageData& data,
                                                      const std::string& radioId,
                                                      const std::string& destinationRadioId ) const
    {
        FUNCTION_ENTRY( "createEvent" );

        // first get the message type from the event
        TrainMessageType messageType = getMessageTypeFromEvent( data );

        // Common/periodic train events
        FIRST_HANDLE_MESSAGE_TYPE( AtcEvent )
        HANDLE_MESSAGE_TYPE( Oa1Event )
        HANDLE_MESSAGE_TYPE( TestCallEvent )
        HANDLE_MESSAGE_TYPE( BadCommandEvent )
        HANDLE_MESSAGE_TYPE( ChangeoverStatusEvent )
        HANDLE_MESSAGE_TYPE( ChangeAreaCommandReceivedEvent )

        // Driver/OCC Call events
        HANDLE_MESSAGE_TYPE( CallTrainCommandReceivedEvent )
        HANDLE_MESSAGE_TYPE( RequestForOccCallEvent )
        HANDLE_MESSAGE_TYPE( EndOfOccCallEvent )

        // CCTV events
        HANDLE_MESSAGE_TYPE( CctvCommandReceivedEvent )

        // TTIS events
        HANDLE_MESSAGE_TYPE( TtisCommandReceivedEvent )
        HANDLE_MESSAGE_TYPE( EndOfTtisDownloadEvent )

        // PA events
        HANDLE_MESSAGE_TYPE( PaCommandReceivedEvent )
        HANDLE_MESSAGE_TYPE( PaReadyForLiveAnnouncementEvent )
        HANDLE_MESSAGE_TYPE( PaRequestForResetEvent )
        HANDLE_MESSAGE_TYPE( PaResetEvent )
        HANDLE_MESSAGE_TYPE( PaTrainMessageCompletedEvent )

        // PEC events
        HANDLE_MESSAGE_TYPE( PecActivatedEvent )
        HANDLE_MESSAGE_TYPE( PecCommandReceivedEvent )
        HANDLE_MESSAGE_TYPE( PecReadyForConversationEvent )
        HANDLE_MESSAGE_TYPE( PecResetEvent )
        HANDLE_MESSAGE_TYPE( PecSelectedByDriverEvent )

        // Random events we dont really care about, handled anyway
        HANDLE_MESSAGE_TYPE( AudioCabToCabEvent )
        HANDLE_MESSAGE_TYPE( PaAudioSwitchEvent )
        
        // Should never be received, but handle anyway
        HANDLE_MESSAGE_TYPE( PecRequestForResetEvent )

        std::ostringstream messageTypeStr;
        messageTypeStr << std::setw(2) << std::setfill('0') << messageType;
        TA_THROW( ProtocolException( "Message Type Not Known", "MessageType", messageTypeStr.str() ) );
        
        FUNCTION_EXIT;
        // this is just to keep the compiler from complaining
        // execution should never reach here
        return TrainEventPtr( static_cast<TrainEvent*>( NULL ) );
    }


    TrainMessageType TrainEventFactoryImpl::getMessageTypeFromEvent( const TrainMessageData& data ) const
    {
        FUNCTION_ENTRY( "getMessageTypeFromEvent" );

        if ( TimsEvent::MESSAGE_TYPE_POSITION < data.size() )
        {
            FUNCTION_EXIT;
            return data[ TimsEvent::MESSAGE_TYPE_POSITION ];
        }
        else
        {
            std::ostringstream positionstr;
            positionstr << "Requested: " << TimsEvent::MESSAGE_TYPE_POSITION
                        << "Length: " << data.size();

            TA_THROW( ProtocolException( "Invalid byte position", "position", positionstr.str() ) );
        }

        FUNCTION_EXIT;
        return 0;
    }
}
