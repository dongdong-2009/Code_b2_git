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
#if !defined(PECRESETCOMMANDRECIEVEDCOMMAND_H)
#define PECRESETCOMMANDRECIEVEDCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractPecCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to answer a PEC
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:21 PM
     */

    class PecResetCommandReceivedCommand : public AbstractPecCommand
    {

    public:

        /**
         * Create a PEC Reset Received command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param carNumber
         * @param pecNumber
         */
        PecResetCommandReceivedCommand( CommonTypes::TrainIdType trainId,
                          unsigned char origin,
                          ProtocolPecTypes::EPecCarNumber carNumber,
                          ProtocolPecTypes::EPecNumber pecNumber,
						  TA_IRS_Bus::ProtocolPecTypes::EPecCommandResult status, PecTypes::EPecTrainSource pecTrainSource );


        /**
         * Destructor
         */
        virtual ~PecResetCommandReceivedCommand();


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
        PecResetCommandReceivedCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;

    };


    typedef boost::shared_ptr< PecResetCommandReceivedCommand > PecResetCommandReceivedCommandPtr;

}

#endif // !defined(PecAnswerCommand_H)
