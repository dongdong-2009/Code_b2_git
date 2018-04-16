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
#if !defined(PALIVERESETCOMMAND_H)
#define PALIVERESETCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Command to reset a pa broadcast
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:14 PM
     */
    class PaLiveResetCommand : public TimsCommand
    {

    public:

        /**
         * Create a change area command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param announcementId
         */
        PaLiveResetCommand( CommonTypes::TrainIdType trainId,
                        unsigned char origin,
                        unsigned char announcementId );


        /**
         * Destructor
         */
        virtual ~PaLiveResetCommand();


        /**
         * Sets the response to this command, if it is not the matching response then the command will not accept the response.
         *
         * @return if the command accepted the response, and if the response should be considered consumed or not
         *
         * @param response
         */
        virtual TrainCommand::TrainResponseResult setResponse( TrainEventPtr response );


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a PaCommandReceivedEvent if the response was a command received, an empty PaCommandReceivedEventPtr otherwise.
         */
        PaCommandReceivedEventPtr getCommandReceivedResponse();


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a PaTrainMessageCompletedEvent if the response was a command received, an empty PaTrainMessageCompletedEventPtr otherwise.
         */
        PaTrainMessageCompletedEventPtr getTrainMessageCompletedResponse();


        /**
         * Returns a log string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


    private:

        /**
         * Private default constructor
         */
        PaLiveResetCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * offset into the message of the request type field
         */
        static const unsigned short ANNOUNCEMENT_ID_POSITION;


        /**
         * the announcement ID
         */
        unsigned char m_announcementId;

    };

    
    typedef boost::shared_ptr< PaLiveResetCommand > PaLiveResetCommandPtr;

}

#endif // !defined(PaLiveResetCommand_H)
