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
#if !defined(ITrainStateUpdateListener_H)
#define ITrainStateUpdateListener_H


#include "bus/trains/TrainTransactionModel/src/TrainState.h"

namespace TA_IRS_Bus
{

    /**
     * This is used to receive notifications from TrainStatusManager when a resource
     * changes for a train. This can be used by the business logic to perform agent
     * synchronisation, or to handle the case when data is updated.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:42 PM
     */

    class ITrainStateUpdateListener
    {

    public:

        /**
         * Implemented by the concrete class to handle state changes.
         *
         * @param state
         */
        virtual void processTrainStateChanged( TrainStatePtr state ) = 0;

    };
}

#endif // !defined(ITrainStateUpdateListener_H)
