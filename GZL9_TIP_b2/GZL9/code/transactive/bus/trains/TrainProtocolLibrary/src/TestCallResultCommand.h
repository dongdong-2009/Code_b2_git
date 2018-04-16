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
#if !defined(TestCallResultCommand_H)
#define TestCallResultCommand_H

#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/MiscDataTypes.h"


namespace TA_IRS_Bus
{

    /**
     * This is an answer to a test call
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:29 PM
     */

    class TestCallResultCommand : public TimsCommand
    {

    public:

        /**
         * Create a Test Call Result command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param area    whether the train should be in mainline or depot talkgroup
         */
        TestCallResultCommand( CommonTypes::TrainIdType trainId,
                               unsigned char origin,
                               ETrainArea area );


        /**
         * Destructor
         */
        virtual ~TestCallResultCommand();


        /**
         * Returns a log string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


        /**
         * Sets the response to this command, if it is not the matching response then the command will not accept the response.
         *
         * @return if the command accepted the response, and if the response should be considered consumed or not
         *
         * @param response
         */
        virtual TrainCommand::TrainResponseResult setResponse( TrainEventPtr response );


    private:

        /**
         * Private default constructor
         */
        TestCallResultCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * offset into the message of the are field
         */
        static const unsigned short AREA_POSITION;


        /**
         * The area the train woke up in
         */
        ETrainArea m_area;

    };


    typedef boost::shared_ptr< TestCallResultCommand > TestCallResultCommandPtr;

}

#endif // !defined(TestCallResultCommand_H)
