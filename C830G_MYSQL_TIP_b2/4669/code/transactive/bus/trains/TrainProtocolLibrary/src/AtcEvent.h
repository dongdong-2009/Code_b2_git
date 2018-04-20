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
#if !defined(AtcEvent_H)
#define AtcEvent_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTableEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Represents an incoming ATC table
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:54 PM
     */

    class AtcEvent : public AbstractTableEvent
    {

    public:
        
        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        AtcEvent( const TrainMessageData& data,
                  const std::string& sendingTsi,
                  const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~AtcEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


    private:

        /**
         * Private default constructor
         */
        AtcEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:54 PM
     */
    typedef boost::shared_ptr< AtcEvent > AtcEventPtr;

}  //end of namespace

#endif // !defined(AtcEvent_H)
