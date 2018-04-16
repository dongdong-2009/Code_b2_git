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
#if !defined(IStateUpdateMessageSender_H)
#define IStateUpdateMessageSender_H

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

namespace TA_IRS_App
{

    /**
     * Interface used to send state updates
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:44 PM
     */
    class IStateUpdateMessageSender
    {

    public:

        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& state ) = 0;


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& state ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IStateUpdateMessageSender_H)
