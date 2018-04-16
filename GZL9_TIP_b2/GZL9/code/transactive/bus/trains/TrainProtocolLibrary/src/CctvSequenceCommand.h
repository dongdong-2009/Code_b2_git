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
#if !defined(CctvSequenceCommand_H)
#define CctvSequenceCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractCctvCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to select a sequence
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:00 PM
     */

    class CctvSequenceCommand : public AbstractCctvCommand
    {

    public:

        /**
         * Create a carrier on command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param sequence    The sequence number, must be between 1 and 6 (inclusive)
         * @param sequenceMode    The sequence mode. Default value is the only one that should be used.
         */
        CctvSequenceCommand( CommonTypes::TrainIdType trainId,
                             unsigned char origin,
                             ProtocolCctvTypes::ECctvSequenceNumber sequence,
                             ProtocolCctvTypes::ECctvSequenceMode sequenceMode = ProtocolCctvTypes::SequenceForward );


        /**
         * Destructor
         */
        virtual ~CctvSequenceCommand();


        /**
         * Gets the camera set in this command
         *
         * @return the sequence number
         */
        ProtocolCctvTypes::ECctvSequenceNumber getSequenceNumber();


        /**
         * Gets the camera set in this command
         *
         * @return the sequence mode
         */
        ProtocolCctvTypes::ECctvSequenceMode getSequenceMode();


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
        CctvSequenceCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * position of sequence number field
         */
        static const unsigned short SEQUENCE_POSITION;


        /**
         * position of sequence mode field
         */
        static const unsigned short SEQUENCE_MODE_POSITION;


        /**
         * Sequence field
         */
        ProtocolCctvTypes::ECctvSequenceNumber m_sequence;


        /**
         * Sequence mode field
         */
        ProtocolCctvTypes::ECctvSequenceMode m_sequenceMode;

    };


    typedef  boost::shared_ptr< CctvSequenceCommand > CctvSequenceCommandPtr;

}

#endif // !defined(CctvSequenceCommand_H)
