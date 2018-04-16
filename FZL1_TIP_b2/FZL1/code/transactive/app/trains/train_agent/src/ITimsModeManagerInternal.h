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
#if !defined(ITimsModeManagerInternal_H)
#define ITimsModeManagerInternal_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:47 PM
     */
    class ITimsModeManagerInternal
    {

    public:

        /**
         * This will signal the train agent to take control of this train by sending a change mode command to it.
         *
         * @param trainId    The train ID to manually take
         */
        virtual void signalChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * This is received when a train at some location is not communicating with the correct train agent.
         * This agent will check if it should be the agent that the train is communicating with,
         * and it it is, it will spawn off a task to make the train communicate with this agent instead.
         *
         * @param event    The event to process
         */
        virtual void processNonDutyTrainTSI( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& event ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsModeManagerInternal_H)
