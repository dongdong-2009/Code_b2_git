/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(ITrainDriverCallUpdateObserver_H)
#define ITrainDriverCallUpdateObserver_H

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is implemented by clients who wish to receive Driver Calll update messages
     * from the local Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:45 PM
     */

    class ITrainDriverCallUpdateObserver
    {

    public:

        /**
         * This will receive a OccCallRequest message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processOccCallRequest( const TA_IRS_Bus::CallTypes::OccCallRequest& event ) = 0;


        /**
         * This will receive a OccCallReset message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processOccCallReset( const TA_IRS_Bus::CallTypes::OccCallReset& event ) = 0;

    protected:

        ITrainDriverCallUpdateObserver() {}

        virtual ~ITrainDriverCallUpdateObserver() {}

    };

}

#endif // !defined(ITrainDriverCallUpdateObserver_H)
