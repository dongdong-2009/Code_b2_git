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
#if !defined(ABSTRACTTTISCOMMAND_H)
#define ABSTRACTTTISCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisTypes.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Abstract TTIS command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:53 PM
     */

    class AbstractTtisCommand : public TimsCommand
    {

    public:

        /**
         *
         * @param trainId
         * @param length    The command length
         * @param commandType
         * @param origin    Command origin
         * @param originalCommand    The expected original command in the response message
         */
        AbstractTtisCommand( CommonTypes::TrainIdType trainId,
                             unsigned int length,
                             TrainMessageType commandType,
                             unsigned char origin,
                             ETtisOriginalCommand originalCommand );


        /**
         * Destructor
         */
        virtual ~AbstractTtisCommand();


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a TtisCommandReceivedEvent  if the response was a command received, an empty TtisCommandReceivedEventPtr otherwise.
         */
        TtisCommandReceivedEventPtr getCommandReceivedResponse();


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
        AbstractTtisCommand();


        /**
         * The original command the response event must contain
         */
        ETtisOriginalCommand m_originalCommand;

    };


    typedef boost::shared_ptr< AbstractTtisCommand > AbstractTtisCommandPtr;

}

#endif // !defined(AbstractTtisCommand_H)
