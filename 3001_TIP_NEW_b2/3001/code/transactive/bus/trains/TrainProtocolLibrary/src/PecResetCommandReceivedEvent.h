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
#if !defined(PECRESETCOMMANDRECIEVEDEVENT_H)
#define PECRESETCOMMANDRECIEVEDEVENT_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractPecEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a PEC activated event.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:20 PM
     */

    class PecResetCommandReceivedEvent : public AbstractPecEvent
    {

    public:

        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        PecResetCommandReceivedEvent( const TrainMessageData& data,
                           const std::string& sendingTsi,
                           const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~PecResetCommandReceivedEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


    private:

        /**
         * Private default constructor
         */
        PecResetCommandReceivedEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:20 PM
     */
    typedef boost::shared_ptr< PecResetCommandReceivedEvent > PecResetCommandReceivedEventPtr;

}

#endif // !defined(PecActivatedEvent_H)
