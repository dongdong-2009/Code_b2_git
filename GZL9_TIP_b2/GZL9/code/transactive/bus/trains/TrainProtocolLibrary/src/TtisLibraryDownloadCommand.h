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
#if !defined(TtisLibraryDownloadCommand_H)
#define TtisLibraryDownloadCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"


namespace TA_IRS_Bus
{
    /**
     * Command to download a TTIS library
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:35 PM
     */

    class TtisLibraryDownloadCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Represents a start or abort
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:36 PM
         */
        enum ETtisLibraryDownloadCommand
        {
            TtisStartDownload = 0,
            TtisAbortDownload = 1
        };


        /**
         * Create a command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param commandType
         */
        TtisLibraryDownloadCommand( CommonTypes::TrainIdType trainId,
                                    unsigned char origin,
                                    ETtisLibraryDownloadCommand commandType = TtisStartDownload );


        /**
         * Destructor
         */
        virtual ~TtisLibraryDownloadCommand();


        /**
         * Returns a loggable string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();

    private:

        /**
         * Private default constructor
         */
        TtisLibraryDownloadCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * Offset of the start/abort command
         */
        static const unsigned short COMMAND_POSITION;


        /**
         * The command type,ie start or stop
         */
        ETtisLibraryDownloadCommand m_commandType;

    };


    typedef boost::shared_ptr< TtisLibraryDownloadCommand > TtisLibraryDownloadCommandPtr;

}

#endif // !defined(TtisLibraryDownloadCommand_H)
