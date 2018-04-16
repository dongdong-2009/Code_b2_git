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
#if !defined(PaCommandReceivedEvent_H)
#define PaCommandReceivedEvent_H


#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a response to a PA command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:13 PM
     */

    class PaCommandReceivedEvent : public TimsEvent
    {

    public:

        /**
         * Original command for the PA reply
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:13 PM
         */
        enum EPaOriginalCommand
        {
            PaLiveAnnouncement = 0,
            PaPreRecordedAnnouncement = 1,
            PaCyclicAnnouncement = 3,
            PaReset = 4
        };


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        PaCommandReceivedEvent( const TrainMessageData& data,
                                const std::string& sendingTsi,
                                const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~PaCommandReceivedEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the original request mode
         */
        ProtocolPaTypes::EPaRequestMode getRequestMode() const;


        /**
         * Gets the original command
         */
        EPaOriginalCommand getOriginalCommand() const;


        /**
         * Gets the result of the original command
         */
        ProtocolPaTypes::EPaCommandResult getCommandResult() const;


        /**
         * Gets the predefined library version
         */
        unsigned short getLibraryVersion() const;


        /**
         * Gets the original announcement id
         */
        unsigned short getAnnouncementId() const;


        /**
         * Formats and returns a loggable string.
         */
        virtual std::string getLogString() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        PaCommandReceivedEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the request mode field
         */
        static const unsigned short REQUEST_MODE_POSITION;


        /**
         * Offset of the original command field
         */
        static const unsigned short ORIGINAL_COMMAND_POSITION;


        /**
         * Offset of the result field
         */
        static const unsigned short RESULT_POSITION;


        /**
         * Offset of the library version field
         */
        static const unsigned short LIBRARY_VERSION_POSITION;


        /**
         * Offset of the announcement id field
         */
        static const unsigned short ANNOUNCEMENT_ID_POSITION;


        /**
         * The original request mode
         */
        ProtocolPaTypes::EPaRequestMode m_requestMode;


        /**
         * The original command
         */
        EPaOriginalCommand m_originalCommand;


        /**
         * The result of the original command
         */
        ProtocolPaTypes::EPaCommandResult m_commandResult;


        /**
         * the PA pre recorded library version
         */
        unsigned short m_libraryVersion;


        /**
         * The original announcement ID
         */
        unsigned char m_announcementId;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:13 PM
     */
    typedef boost::shared_ptr< PaCommandReceivedEvent > PaCommandReceivedEventPtr;

}

#endif // !defined(PaCommandReceivedEvent_H)
