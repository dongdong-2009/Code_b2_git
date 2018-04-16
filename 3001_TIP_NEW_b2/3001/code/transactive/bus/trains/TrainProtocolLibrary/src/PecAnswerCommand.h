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
#if !defined(PECANASWERCOMMAND_H)
#define PECANASWERCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractPecCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Command to answer a PEC
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:21 PM
     */

    class PecAnswerCommand : public AbstractPecCommand
    {

    public:

        /**
         * Create a PEC command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param carNumber
         * @param pecNumber
         */
        PecAnswerCommand( CommonTypes::TrainIdType trainId,
                          unsigned char origin,
                          ProtocolPecTypes::EPecCarNumber carNumber,
						  ProtocolPecTypes::EPecNumber pecNumber, PecTypes::EPecTrainSource pecTrainSource );


        /**
         * Destructor
         */
        virtual ~PecAnswerCommand();


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
         * @return a PecCommandReceivedEvent if the response was a command received, an empty PecCommandReceivedEventPtr otherwise.
         */
        PecCommandReceivedEventPtr getCommandReceivedResponse();


    private:

        /**
         * Private default constructor
         */
        PecAnswerCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;

    };


    typedef boost::shared_ptr< PecAnswerCommand > PecAnswerCommandPtr;

}

#endif // !defined(PecAnswerCommand_H)
