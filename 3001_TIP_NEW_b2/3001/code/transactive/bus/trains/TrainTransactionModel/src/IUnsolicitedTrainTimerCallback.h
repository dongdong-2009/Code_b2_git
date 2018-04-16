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
#if !defined(IUnsolicitedTrainTimerCallback_H)
#define IUnsolicitedTrainTimerCallback_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_Bus
{

    /**
     * implemented by classes that use UnsolicitedTrainTimer
     * @author adamr
     * @version 1.0
     * @created 30-Jan-2008 2:53:44 PM
     */

    class IUnsolicitedTrainTimerCallback
    {

    public:

        /**
         * Handle a timer expiry
         *
         * @param trainId    The train the timer is for
         */
        virtual void handleTrainTimeout( CommonTypes::TrainIdType trainId ) = 0;

    };
}

#endif // !defined(IUnsolicitedTrainTimerCallback_H)
