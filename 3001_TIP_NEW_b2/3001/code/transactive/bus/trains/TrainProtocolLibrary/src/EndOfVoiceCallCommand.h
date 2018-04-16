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
#if !defined(ENDOFVOICECALLCOMMAND_H)
#define ENDOFVOICECALLCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Command to end a voice call
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:07 PM
     */

    class EndOfVoiceCallCommand : public TimsCommand
    {

    public:

        /**
         * Create an end of voice call command.
         *
         * @param trainId
         * @param origin    Command origin
         */
        EndOfVoiceCallCommand( CommonTypes::TrainIdType trainId, unsigned char origin );


        /**
         * Destructor
         */
        virtual ~EndOfVoiceCallCommand();


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a CallTrainCommandReceivedEvent if the response was a command received, an empty CallTrainCommandReceivedEventPtr otherwise.
         */
        CallTrainCommandReceivedEventPtr getCommandReceivedResponse();


    private:

        /**
         * Private default constructor
         */
        EndOfVoiceCallCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;

    };


    typedef boost::shared_ptr< EndOfVoiceCallCommand > EndOfVoiceCallCommandPtr;

}

#endif // !defined(EndOfVoiceCallCommand_H)
