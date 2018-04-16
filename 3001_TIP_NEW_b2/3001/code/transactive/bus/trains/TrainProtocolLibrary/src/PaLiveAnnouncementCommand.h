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
#if !defined(PALIVEANNOUNCEMENTCOMMAND_H)
#define PALIVEANNOUNCEMENTCOMMAND_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"


//forward declaration

namespace TA_IRS_Bus
{

    class PaCommandReceivedEvent;
    typedef boost::shared_ptr< PaCommandReceivedEvent > PaCommandReceivedEventPtr;
}


namespace TA_IRS_Bus
{

    /**
     * Command to set up a live announcement
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:16 PM
     */

    class PaLiveAnnouncementCommand : public TimsCommand
    {

    public:

        /**
         * Create a change area command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param requestMode
         * @param announcementId
         */
        PaLiveAnnouncementCommand( CommonTypes::TrainIdType trainId,
                                   unsigned char origin,
                                   ProtocolPaTypes::EPaRequestMode requestMode,
                                   unsigned char announcementId );


        /**
         * Destructor
         */
        virtual ~PaLiveAnnouncementCommand();


        /**
         * Sets the response to this command, if it is not the matching response then the command will not accept the response.
         *
         * @return if the command accepted the response, and if the response should be considered consumed or not
         *
         * @param response
         */
        virtual TrainCommand::TrainResponseResult setResponse( TrainEventPtr response );


        /**
         * If this command resulted in a command received, return the event reader.
         *
         * @return a PaCommandReceivedEventReader if the response was a command received, NULL otherwise.
         */
        PaCommandReceivedEventPtr getCommandReceivedResponse();


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
        PaLiveAnnouncementCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * offset into the message of the request type field
         */
        static const unsigned short REQUEST_MODE_POSITION;


        /**
         * offset into the message of the request type field
         */
        static const unsigned short ANNOUNCEMENT_ID_POSITION;


        /**
         * Whether a request or override
         */
        ProtocolPaTypes::EPaRequestMode m_requestMode;


        /**
         * the announcement ID
         */
        unsigned char m_announcementId;
    };


    typedef boost::shared_ptr< PaLiveAnnouncementCommand > PaLiveAnnouncementCommandPtr;

}

#endif // !defined(PaLiveAnnouncementCommand_H)
