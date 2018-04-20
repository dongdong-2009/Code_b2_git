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
#if !defined(AbstractCctvCommand_H)
#define AbstractCctvCommand_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a CCTV command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:50 PM
     */

    class AbstractCctvCommand : public TimsCommand
    {

    public:

        /**
         *
         * @param trainId
         * @param length    The command length
         * @param commandType
         * @param origin    Command origin
         */
        AbstractCctvCommand( CommonTypes::TrainIdType trainId,
                             unsigned int length,
                             TrainMessageType commandType,
                             unsigned char origin );


        /**
         * Destructor
         */
        virtual ~AbstractCctvCommand();


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a CctvCommandReceivedEvent if the response was a command received, an empty CctvCommandReceivedEventPtr otherwise.
         */
        CctvCommandReceivedEventPtr getCommandReceivedResponse();


    private:

        /**
         * Private default constructor
         */
        AbstractCctvCommand();

    };


    typedef boost::shared_ptr< AbstractCctvCommand > AbstractCctvCommandPtr;

}

#endif // !defined(AbstractCctvCommand_H)
