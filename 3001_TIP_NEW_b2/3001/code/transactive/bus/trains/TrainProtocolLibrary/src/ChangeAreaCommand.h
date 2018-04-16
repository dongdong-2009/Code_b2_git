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
#if !defined(CHANGEAREACOMMAND_H)
#define CHANGEAREACOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/MiscDataTypes.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeAreaCommandReceivedEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Command to change the trains area
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:02 PM
     */

    class ChangeAreaCommand : public TimsCommand
    {

    public:

        /**
         * Create a change area command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param area    whether the train should be in mainline or depot talkgroup
         */
        ChangeAreaCommand( CommonTypes::TrainIdType trainId,
                           unsigned char origin,
                           unsigned char status12 );


        /**
         * Destructor
         */
        virtual ~ChangeAreaCommand();


        /**
         * Gets the command number this command will send
         *
         * @return the message type
         */
        static TrainMessageType getStaticMessageType();


        /**
         * Returns a log string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a ChangeAreaCommandReceivedEvent if the response was a command received, an empty ChangeAreaCommandReceivedEventPtr otherwise.
         */
        ChangeAreaCommandReceivedEventPtr getCommandReceivedResponse();


    private:

        /**
         * Private default constructor
         */
        ChangeAreaCommand();


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

		/**
		 * The Status 12 information
		 */
		unsigned char m_status12;

    };

    
    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:03 PM
     */
    typedef  boost::shared_ptr< ChangeAreaCommand > ChangeAreaCommandPtr;

}

#endif // !defined(ChangeAreaCommand_H)
