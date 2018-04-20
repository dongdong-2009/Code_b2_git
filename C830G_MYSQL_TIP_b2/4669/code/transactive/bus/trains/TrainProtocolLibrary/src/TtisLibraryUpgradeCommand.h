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
#if !defined(TtisLibraryUpgradeCommand_H)
#define TtisLibraryUpgradeCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"


namespace TA_IRS_Bus
{

    /**
     * Command to upgrade a TTIS library
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:36 PM
     */

    class TtisLibraryUpgradeCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Create a command.
         *
         * @param trainId
         * @param origin    Command origin
         */
        TtisLibraryUpgradeCommand( CommonTypes::TrainIdType trainId, unsigned char origin );


        /**
         * Destructor
         */
        virtual ~TtisLibraryUpgradeCommand();


    private:

        /**
         * Private default constructor
         */
        TtisLibraryUpgradeCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;

    };


    typedef boost::shared_ptr< TtisLibraryUpgradeCommand > TtisLibraryUpgradeCommandPtr;

}

#endif // !defined(TtisLibraryUpgradeCommand_H)
