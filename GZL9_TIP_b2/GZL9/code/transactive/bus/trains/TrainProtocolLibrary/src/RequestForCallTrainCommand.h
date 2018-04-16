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
#if !defined(RequestForCallTrainCommand_H)
#define RequestForCallTrainCommand_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * To setup a train call
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:26 PM
     */

    class RequestForCallTrainCommand : public TimsCommand
    {

    public:

        /**
         * Whether to apply override or not
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:27 PM
         */
        enum ECallTrainRequestMode
        {
            CallTrainRequestIndividual = 0,
            CallTrainOverrideIndividual = 3,
            CallTrainRequestGroup = 4,
            CallTrainOverrideGroup = 7
        };


        /**
         * Create an end of voice call command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param requestMode
         */
        RequestForCallTrainCommand( CommonTypes::TrainIdType trainId,
                                    unsigned char origin,
                                    ECallTrainRequestMode requestMode );


        /**
         * Destructor
         */
        virtual ~RequestForCallTrainCommand();


        /**
         * Returns a log string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


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
        RequestForCallTrainCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * Offset of the request mode parameter
         */
        static const unsigned short REQUEST_MODE_POSITION;


        /**
         * whether or not override was applied on the request
         */
        ECallTrainRequestMode m_requestMode;

    };


    typedef boost::shared_ptr< RequestForCallTrainCommand > RequestForCallTrainCommandPtr;

}

#endif // !defined(RequestForCallTrainCommand_H)
