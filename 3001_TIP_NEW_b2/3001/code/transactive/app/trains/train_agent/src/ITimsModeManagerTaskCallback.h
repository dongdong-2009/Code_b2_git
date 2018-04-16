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
#if !defined(ITimsModeManagerTaskCallback_H)
#define ITimsModeManagerTaskCallback_H

#include "app/trains/train_agent/src/EChangeModeCommandResult.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * This provides an interface for the ChangeModeTask and ChangeAreaTask to call back to perform their operation.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:47 PM
     */
    class ITimsModeManagerTaskCallback
    {

    public:

        /**
         * This will execute the Change Mode task for the given train. (Send the messages and process the response).
         *
         * @return the result of the operation. If there was a failure, the error string will hold details.
         *
         * @param trainId    The train ID the action will be performed on
         * @param errorDetails    Optional error details if errors were encountered
         */
        virtual EChangeModeCommandResult performChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            std::string& errorDetails ) = 0;


        /**
         * This will execute the Change Area task for the given train. (Send the messages and process the response).
         *
         * @return the result of the operation. If there was a failure, the error string will hold details.
         *
         * @param trainId    The train ID the action will be performed on
         * @param errorDetails    Optional error details if errors were encountered
         */
        virtual EChangeModeCommandResult performChangeArea( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            std::string& errorDetails ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsModeManagerTaskCallback_H)
