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
#if !defined(TtisResetCommand_H)
#define TtisResetCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to reset a tis message
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:38 PM
     */

    class TtisResetCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Create a command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param pidSelection
         * @param messageType
         */
        TtisResetCommand( CommonTypes::TrainIdType trainId,
                          unsigned char origin,
                          ProtocolTtisTypes::ETtisPid pidSelection,
                          ProtocolTtisTypes::ETtisResetMessageType messageType );


        /**
         * Destructor
         */
        virtual ~TtisResetCommand();


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
        TtisResetCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * Offset of the PID parameter
         */
        static const unsigned short PID_POSITION;


        /**
         * Offset of the message type parameter
         */
        static const unsigned short RESET_TYPE_POSITION;


        /**
         * The PID selection
         */
        ProtocolTtisTypes::ETtisPid m_pidSelection;


        /**
         * The type of message to reset
         */
        ProtocolTtisTypes::ETtisResetMessageType m_messageType;

    };


    typedef boost::shared_ptr< TtisResetCommand > TtisResetCommandPtr;

}

#endif // !defined(TtisResetCommand_H)
