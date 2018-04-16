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
#if !defined(TtisScheduleDownloadCommand_H)
#define TtisScheduleDownloadCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"


namespace TA_IRS_Bus
{

    /**
     * Command to download a TTIS schedule
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:39 PM
     */

    class TtisScheduleDownloadCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Create a command.
         *
         * @param trainId
         * @param origin    Command origin
         */
        TtisScheduleDownloadCommand( CommonTypes::TrainIdType trainId, unsigned char origin );


        /**
         * Destructor
         */
        virtual ~TtisScheduleDownloadCommand();


    private:

        /**
         * Private default constructor
         */
        TtisScheduleDownloadCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;

    };


    typedef boost::shared_ptr< TtisScheduleDownloadCommand > TtisScheduleDownloadCommandPtr;

}

#endif // !defined(TtisScheduleDownloadCommand_H)
