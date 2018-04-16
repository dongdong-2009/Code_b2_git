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

#if !defined(TrainCommand_H)
#define TrainCommand_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"
#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * This is a top level command sent via the TrainTransactionManager
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:48 PM
     */

    class TrainCommand
    {

    public:

        /**
         * This indicates whether a certain TrainEvent has signalled the end of a
         * transaction, also whether the event is consumed by this transaction, or should
         * be further passed on to other observers.
         * @author Adam Radics
         * @version 1.0
         * @created 22-Oct-2007 12:19:48 PM
         */

        struct TrainResponseResult
        {

        public:

            /**
             *
             * @param complete
             * @param consumed
             */
            TrainResponseResult( bool complete, bool consumed );


            /**
             * This is the flag to determine whether this transaction is now considered as
             * completed with the receipt of this event.
             */
            bool transactionComplete;


            /**
             * This is the flag to determine whether this event has been consumed by this
             * transaction, or whether it should be further passed on to registered observers.
             */
            bool eventConsumed;
        };


        /**
         * Constructor
         *
         * @param trainId
         * @param length    The command length
         * @param commandType
         */
        TrainCommand( CommonTypes::TrainIdType trainId,
                      unsigned int length,
                      TrainMessageType commandType );


        /** 
          * Destructor
          */
        virtual ~TrainCommand();


        /**
         * Gets the command Type
         */
        TrainMessageType getCommandType() const;


        /**
         * This gets the response command number for this command.
         * @return All valid response types, empty set for no response.
         */
        const TrainMessageTypeList& getResponseTypes() const;


        /**
         * Gets the train id this command is for
         */
        CommonTypes::TrainIdType getTrainId() const;


        /**
         * Gets the data to send
         * May be overidden by subclasses to perform functions like adding checksums etc.
         */
        virtual const TrainMessageData& getMessageData();


        /**
         * Gets the data/command length
         */
        unsigned int getLength() const;


        /**
         * Sets the response to this command, if it is not the matching response then the
         * command will not accept the response.
         * Some protocols may have sequence numbers or fields to match, this allows those
         * to be processed.
         *
         * @return if the command accepted the response, and if the response should be
         * considered consumed or not
         *
         * @param response
         */
        virtual TrainResponseResult setResponse( TrainEventPtr response );


        /**
         * Resets the response to this command to nothing.
         *
         * Used by scheduled commands - so the response is cleared after execution
         */
        virtual void resetResponse();


        /**
         * Gets the response event for this command (can be an empty event pointer)
         */
        TrainEventPtr getResponse();


        /**
         * Returns a loggable string with information about this command.
         * Can be overridden to provide more information.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


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
         * Adds a valid response type
         *
         * @param validResponseType
         */
        void addResponseType( TrainMessageType validResponseType );


    private:


        /**
         * Private default constructor
         */
        TrainCommand();

        /**
         * calculates the checksum on m_data (except for the last 2 bytes where the checksum goes)
         *
         * @return CRC16
		 move from TimsCommand to TrainCommand,hongzhi
         */
        unsigned short calculateChecksum();

    protected:


        /**
         * The raw data sent by the command
         */
        TrainMessageData m_data;


    private:


        /**
         * The train the command is destined for
         */
        CommonTypes::TrainIdType m_trainId;


        /**
         * The type of command
         */
        TrainMessageType m_commandType;


        /**
         * valid responses for this command
         */
        TrainMessageTypeList m_responseTypes;


        /**
         * The response event for this train command
         */
        TrainEventPtr m_response;

        /**
         * Position of the checksum (from the back of the message).
         * The last 2 bytes in every message is reserved for the checksum
         */
        static const unsigned short CHECKSUM_POSITION;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:48 PM
     */
    typedef boost::shared_ptr< TrainCommand > TrainCommandPtr;



    /**
     * @author adamr
     * @version 1.0
     * @created 07-Jan-2008 4:11:52 PM
     */

    struct TrainCommandContainer
    {

    public:

        /**
         * The command
         */
        TrainCommandPtr command;


        /**
         * The optional radio ID
         */
        std::string radioId;
    };


    /**
     * @author adamr
     * @version 1.0
     * @created 07-Jan-2008 4:11:52 PM
     */
    typedef std::vector< TrainCommandContainer > TrainCommandList;

}

#endif // !defined(TrainCommand_H)
