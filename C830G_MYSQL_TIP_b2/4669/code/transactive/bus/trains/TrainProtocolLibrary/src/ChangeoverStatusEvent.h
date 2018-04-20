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
#if !defined(ChangeoverStatusEvent_H)
#define ChangeoverStatusEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"


namespace TA_IRS_Bus
{
    /**
    * Represents an incoming MPU changeover
    * @author Adam Radics
    * @version 1.0
    * @created 05-Feb-2008 3:20:04 PM
    */

    class ChangeoverStatusEvent : public TimsEvent
    {

    public:

        /**
         * The mode of the MPU. Standby or master
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:04 PM
         */
        enum EMpuMode
        {
            MpuStandby = 0,
            MpuMaster = 1
        };


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        ChangeoverStatusEvent( const TrainMessageData& data,
                               const std::string& sendingTsi,
                               const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~ChangeoverStatusEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Formats and returns a loggable string.
         */
        virtual std::string getLogString() const;


        /**
         * Gets the new MPU mode of this MPU.
         */
        EMpuMode getMpuMode() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        ChangeoverStatusEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the status field
         */
        static const unsigned short STATUS_POSITION;


        /**
         * Offset of the MPU mode
         */
        static const unsigned short MODE_POSITION;


        /**
         * status field value
         */
        static const unsigned char OK_STATUS;


        /**
         * This is the new mpu mode
         */
        EMpuMode m_mpuMode;

    };

    /**
    * @author Adam Radics
    * @version 1.0
    * @created 05-Feb-2008 3:20:04 PM
    */
    typedef boost::shared_ptr< ChangeoverStatusEvent > ChangeoverStatusEventPtr;

} //end of namespaece

#endif // !defined(ChangeoverStatusEvent_H)
