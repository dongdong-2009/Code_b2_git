/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * This is a callback implemented by an observer to be notified when
 * the OA1 or ATC timer has expired for a train.
 */

#if !defined(IActivityTimerHandler_H)
#define IActivityTimerHandler_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:37 PM
     */
    class IActivityTimerHandler
    {

    public:

        /**
         * Processes the OA1 timeout for the given train
         *
         * @param trainId
         */
        virtual void processOa1Timeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * Processes the ATC timeout for the given train
         *
         * @param trainId
         */
        virtual void processAtcTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IActivityTimerHandler_H)
