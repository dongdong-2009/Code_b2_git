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
#if !defined(IOperationModeManagerServantInterface_H)
#define IOperationModeManagerServantInterface_H

#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:41 PM
     */
    class IOperationModeManagerServantInterface
    {

    public:

        /**
         * Called by the radio agent to notify the local train agent that it has entered or left radio fallback mode.
         * If not already in local duty mode, entering fallback will trigger local duty mode.
         *
         * @param fallback    Whether the radio system is in fallback or not (true is fallback, false is normal)
         */
        virtual void inFallback( bool fallback ) = 0;


        /**
         * Called by the ATS agent to notify the local train agent that it has entered or left double OCC ATS failure mode.
         * If not already in local duty mode, entering fallback will trigger local duty mode.
         *
         * @param doubleAtsFailure
         */
        virtual void inDoubleAtsFailure( bool doubleAtsFailure ) = 0;


        /**
         * Checks whether the agent is in local control due to radio fallback or local duty.
         * This is only applicable at station locations.
         *
         * This is called by the ATS agent to determine if it should be exchanging ATS data locally,
         * or using OCC ATS to do the data exchange.
         *
         * @return true if ATS should be in local mode
         */
        virtual bool radioInFallback() const = 0;


        /**
         * This gets the current agent operation mode.
         *
         * @return the current operation mode of the agent
         */
        virtual TA_IRS_Bus::TrainInformationTypes::AgentOperationMode getCurrentOperationMode() const = 0;

    };

} // TA_IRS_App

#endif // !defined(IOperationModeManagerServantInterface_H)
