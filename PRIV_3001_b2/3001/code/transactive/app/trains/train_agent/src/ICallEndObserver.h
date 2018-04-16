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
#if !defined(ICallEndObserver_H)
#define ICallEndObserver_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

namespace TA_IRS_App
{

    /**
     * Used to handle call end events,
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:39 PM
     */
    class ICallEndObserver
    {

    public:

        /**
         * This handles a call end event.
         * It can do any required processing to clean up on unexpected call termination.
         * This is called in its own thread, so processing can be done.
         *
         * @param callEnd    The details of the call that ended
         * @param trainList    The trains involved in the radio call
         */
        virtual void processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                          const TA_IRS_Bus::CommonTypes::TrainIdList& trainList ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ICallEndObserver_H)
