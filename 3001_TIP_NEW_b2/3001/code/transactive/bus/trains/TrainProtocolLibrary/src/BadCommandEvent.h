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
#if !defined(BADCOMMANDEVENT_H)
#define BADCOMMANDEVENT_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"

namespace TA_IRS_Bus
{

    /**
     * Represents a bad command response
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:56 PM
     */

    class BadCommandEvent : public TimsEvent
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
        BadCommandEvent( const TrainMessageData& data,
                         const std::string& sendingTsi,
                         const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~BadCommandEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the original command number the train didnt like
         */
        TrainMessageType getOriginalCommand() const;


        /**
         * Gets what was wrong with the last command
         */
        ProtocolCommonTypes::EBadCommandError getErrorCode() const;


        /**
         * Formats and returns a log string.
         */
        virtual std::string getLogString() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        BadCommandEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the orignal message field
         */
        static const unsigned short ORIGINAL_POSITION;


        /**
         * Offset of the MPU mode
         */
        static const unsigned short ERROR_CODE_POSITION;


        /**
         * The original command the train is complaining about
         */
        TrainMessageType m_originalCommand;


        /**
         * The reason the train didnt like the last command
         */
        ProtocolCommonTypes::EBadCommandError m_errorCode;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:56 PM
     */
    typedef boost::shared_ptr< BadCommandEvent > BadCommandEventPtr;

}

#endif // !defined(BadCommandEvent_H)
