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
#if !defined(IAtsMessageProcessor_H)
#define IAtsMessageProcessor_H

#include "app/trains/train_agent/src/AtsDataTypes.h"

namespace TA_IRS_App
{

    /**
     * This is implemented by the handler of ATS messages
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:39 PM
     */
    class IAtsMessageProcessor
    {

    public:

        /**
         * This will process the train information from the ATS Agent.
         *
         * @param trainInformation    The update from ATS
         */
        virtual void processAtsInformation( const AtsTrainInformation& trainInformation ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IAtsMessageProcessor_H)
