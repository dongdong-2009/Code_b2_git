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
#if !defined(TimsCommand_H)
#define TimsCommand_H


#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/BadCommandEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"


namespace TA_IRS_Bus
{
    /**
     * Generic Train Command for the TIMS protocol.
     * Provides the basic TIMS fields, and methods to set data.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:30 PM
     */

    class TimsCommand : public TrainCommand
    {

    public:


        /**
         *
         * @param trainId
         * @param length    The command length
         * @param commandType
         * @param origin    Command origin
         * @param responseExpected    If there is a response expected for this command, set this to true
         */
        TimsCommand( CommonTypes::TrainIdType trainId,
                     unsigned int length,
                     TrainMessageType commandType,
                     unsigned char origin,
                     bool responseExpected = true );


        /**
         * Destructor
         */
        virtual ~TimsCommand();


        /**
         * Gets the data to send
         *
         * @return the data to send to the train
         */
        virtual TrainMessageData getData();


        /**
         * Returns a loggable string with information about this command.
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


        /**
         * Returns the origin set in the message
         */
        unsigned char getOrigin() const;


        /**
         * If this command resulted in a bad command, return the event
         *
         * @retrurn a BadCommandEvent if the response was a bad command, empty pointer otherwise.
         */
        BadCommandEventPtr getBadCommandResponse();


        /**
         * If this command received an MPU changeover during its execution, this will return the changeover status event.
         *
         * @retrurn a ChangeoverStatusEvent if the response was an MPU changeover, empty pointer otherwise.
         */
        ChangeoverStatusEventPtr getChangeoverStatusEventResponse();


    protected:


        /**
         * Sets the byte at the given address.
         *
         * @exception ProtocolException if the address is not valid.
         *
         * @param position
         * @param value
         */
        void setByte( unsigned short position, unsigned char value );


        /**
         * Sets the word at the given address.
         *
         * @exception ProtocolException if the address is not valid.
         *
         * @param position
         * @param value
         */
        void setWord( unsigned short position, unsigned short value );


        /**
         * Grows a message to be longer
         * Sets the length field as well
         *
         * @param newLength
         */
        void setNewLength( unsigned char newLength );


    private:

        /**
         * Private default constructor
         */
        TimsCommand();


        /**
         * calculates the checksum on m_data (except for the last 2 bytes where the checksum goes)
         *
         * @return CRC16
         */
        unsigned short calculateChecksum();


        /**
         * The longest a message can be
         */
        static const unsigned short MAX_MESSAGE_LENGTH;


        /**
         * Position of the message flag
         */
        static const unsigned short MESSAGE_FLAG_POSITION;


        /**
         * Value of the message flag
         */
        static const unsigned char MESSAGE_FLAG_VALUE;


        /**
         * Position of the message type flag
         */
        static const unsigned short MESSAGE_TYPE_POSITION;


        /**
         * Position of the train id
         */
        static const unsigned short TRAIN_ID_POSITION;


        /**
         * Position of the message length
         */
        static const unsigned short LENGTH_POSITION;


        /**
         * Position of the message origin
         */
        static const unsigned short ORIGIN_POSITION;


        /**
         * Position of the checksum (from the back of the message).
         * The last 2 bytes in every message is reserved for the checksum
         */
        static const unsigned short CHECKSUM_POSITION;


        /**
         * The MFT/server origin
         */
        unsigned char m_origin;

    };


    typedef boost::shared_ptr< TimsCommand > TimsCommandPtr;

}

#endif // !defined(TimsCommand_H)
