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
#if !defined(TTISPRESTOREDMESSAGECOMMAND_H)
#define TTISPRESTOREDMESSAGECOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to display a TTIS pre defined text message
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:37 PM
     */

    class TtisPreStoredMessageCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Create a command.
         *
         * @exception ProtocolException for a message too long
         *
         * @param trainId
         * @param origin    Command origin
         * @param pidSelection
         * @param libraryVersion
         * @param perRun
         * @param messageId        
         */
        TtisPreStoredMessageCommand( CommonTypes::TrainIdType trainId,
                                     unsigned char origin,
                                     ProtocolTtisTypes::ETtisPid pidSelection,
                                     unsigned int libraryVersion,
									 unsigned char perRun ,
                                     unsigned short messageId  );


        /**
         * Destructor
         */
        virtual ~TtisPreStoredMessageCommand();


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
        TtisPreStoredMessageCommand();


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
         * Offset of the library version parameter
         */
        static const unsigned short LIBRARY_VERSION_POSITION;


        /**
         * Offset of the identifier type parameter
         */
        static const unsigned short PERRUN_POSITION;


        /**
         * Offset of the identifier offset parameter
         */
        static const unsigned short MESSAGEID_POSITION;





        ///**
        // * Offset of the start hour parameter
        // */
        //static const unsigned short START_HOUR_POSITION;


        ///**
        // * Offset of the start minute parameter
        // */
        //static const unsigned short START_MINUTE_POSITION;


        ///**
        // * Offset of the stop hour parameter
        // */
        //static const unsigned short STOP_HOUR_POSITION;


        ///**
        // * Offset of the stop minute parameter
        // */
        //static const unsigned short STOP_MINUTE_POSITION;


        /**
         * The PID selection
         */
        ProtocolTtisTypes::ETtisPid m_pidSelection;


        /**
         * library version parameter
         */
        unsigned int m_libraryVersion;


        /**
         * message per run
         */
        unsigned char m_perRun ;


        /**
         * identifier Id
         */
        unsigned short m_messageId ;

       

    };


    typedef boost::shared_ptr< TtisPreStoredMessageCommand > TtisPreStoredMessageCommandPtr;

}

#endif // !defined(TtisPreStoredMessageCommand_H)
