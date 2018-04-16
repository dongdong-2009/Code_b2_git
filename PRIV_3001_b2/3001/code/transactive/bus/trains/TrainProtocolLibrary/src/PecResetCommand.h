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
#if !defined(PEDRESETCOMMAND_H)
#define PEDRESETCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractPecCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to reset a PEC
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:24 PM
     */

    class PecResetCommand : public AbstractPecCommand
    {

    public:

        /**
         * Create a PEC command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param carNumber
         * @param pecNumber
         * @param responseExpected    Optionally this can be a response to a TIMS event, in this case we dont expect a response. Bad protocol design uses the same message for 2 functions.
         */
        PecResetCommand( CommonTypes::TrainIdType trainId,
                         unsigned char origin,
                         ProtocolPecTypes::EPecCarNumber carNumber,
                         ProtocolPecTypes::EPecNumber pecNumber,
						 ProtocolPecTypes::EPECFlagNumber rqsOrAck,
						 PecTypes::EPecTrainSource pecTrainSource,
                         bool responseExpected = true );


        /**
         * Destructor
         */
        virtual ~PecResetCommand();


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
         * @return a PecResetEvent if the response was a command received, an empty PecResetEventPtr otherwise.
         */
        const PecResetCommandReceivedEventPtr getPecResetResponse();


    private:

        /**
         * Private default constructor
         */
        PecResetCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;

        static const unsigned short PEC_RQSORACK_POSITION;

       	//ProtocolPecTypes::EPECFlagNumber m_rqsOrAck;

    };


    typedef boost::shared_ptr< PecResetCommand > PecResetCommandPtr;

}

#endif // !defined(PecResetCommand_H)
