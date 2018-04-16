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
#if !defined(ChangeModeCommand_H)
#define ChangeModeCommand_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/MiscDataTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to change the trains operating mode
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:03 PM
     */

    class ChangeModeCommand : public TimsCommand
    {

    public:


        /**
         * Create a change mode command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param mode    the mode of the train
         */
        ChangeModeCommand( CommonTypes::TrainIdType trainId,
                           unsigned char origin,
                           ETrainMode mode );

        /**
         * Destructor
         */
        virtual ~ChangeModeCommand();


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
        ChangeModeCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * offset into the message of the mode field
         */
        static const unsigned short MODE_POSITION;


        /**
         * The mode to set the train to
         */
        ETrainMode m_mode;

    };

    
    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:03 PM
     */
    typedef  boost::shared_ptr< ChangeModeCommand > ChangeModeCommandPtr;

}

#endif // !defined(ChangeModeCommand_H)
