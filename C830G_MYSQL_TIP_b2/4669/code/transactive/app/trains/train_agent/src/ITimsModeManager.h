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
#if !defined(ITimsModeManager_H)
#define ITimsModeManager_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:46 PM
     */
    class ITimsModeManager
    {

    public:

        /**
         * This will manually force train communications over to this train agent.
         * It is only to be used in failure scenarios, and incorrect use could result in the situation automatically normalising (or attempting to normalise)
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         *
         * @param trainId    The train ID to manually take
         * @param sessionId    The operator taking control
         */
        virtual void manuallyTakeTrainControl( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsModeManager_H)
