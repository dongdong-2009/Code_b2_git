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
#if !defined(ITimsCctvManagerTaskCallback_H)
#define ITimsCctvManagerTaskCallback_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_App
{

    /**
     * Used by the carrier off task to call back on the CCTV manager
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:45 PM
     */
    class ITimsCctvManagerTaskCallback
    {

    public:

        /**
         * This will actually send the carrier off to the train. It is called from the carrier off work item.
         *
         * @param trainId    The train to carrier off
         * @param owningTrainId    If the carrier off is due to conflict, which train is the owning train
         */
        virtual void performCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsCctvManagerTaskCallback_H)
