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
#if !defined(CctvCarrierOnCommand_H)
#define CctvCarrierOnCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractCctvCommand.h"


namespace TA_IRS_Bus
{

    /**
     * Command to turn carrier on
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:58 PM
     */

    class CctvCarrierOnCommand : public AbstractCctvCommand
    {

    public:

        /**
         * Create a carrier on command.
         *
         * @param trainId
         * @param origin    Command origin
         */
        CctvCarrierOnCommand( CommonTypes::TrainIdType trainId, unsigned char origin );


        /**
         * Destructor
         */
        virtual ~CctvCarrierOnCommand();


    private:

        /**
         * Private default constructor
         */
        CctvCarrierOnCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;

    };


    typedef  boost::shared_ptr< CctvCarrierOnCommand > CctvCarrierOnCommandPtr;

}

#endif // !defined(CctvCarrierOnCommand_H)
