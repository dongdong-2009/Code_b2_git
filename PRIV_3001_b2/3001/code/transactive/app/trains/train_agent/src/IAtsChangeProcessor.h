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
#if !defined(IAtsChangeProcessor_H)
#define IAtsChangeProcessor_H

#include "app/trains/train_agent/src/AtsDataTypes.h"

namespace TA_IRS_App
{

    /**
     * This is used to process changes in ATS information. The new and old information will be given.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:38 PM
     */

    class IAtsChangeProcessor
    {

    public:

        /**
         * This will process the changes between the new and old ats information
         *
         * @param oldInformation    The previous information
         * @param newInformation    The latest updated information
         */
        virtual void processAtsUpdate( const AtsTrainInformation& oldInformation,
                                       const AtsTrainInformation& newInformation ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IAtsChangeProcessor_H)
