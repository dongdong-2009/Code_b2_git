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
#if !defined(OCCCALLRESETCOMMAND_H)
#define OCCCALLRESETCOMMAND_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"


namespace TA_IRS_Bus
{

    /**
     * To reset an OCC call
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:11 PM
     */

    class OccCallResetCommand : public TimsCommand
    {

    public:

        /**
         * Create an end of voice call command.
         *
         * @param trainId
         * @param origin    Command origin
         */
        OccCallResetCommand( CommonTypes::TrainIdType trainId, unsigned char origin );


        /**
         * Destructor
         */
        virtual ~OccCallResetCommand();


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
        OccCallResetCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;

    };


    typedef boost::shared_ptr< OccCallResetCommand > OccCallResetCommandPtr;

}

#endif // !defined(OccCallResetCommand_H)
