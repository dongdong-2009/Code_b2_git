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


#if !defined(ITRAINSTATEUPDATEOBSERVER_H)
#define ITRAINSTATEUPDATEOBSERVER_H


#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is implemented by clients who wish to receive state update messages from
     * the Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:47 PM
     */

    class ITrainStateUpdateObserver
    {

    public:

        /**
         * This will receive a NonDutyTrainTSI message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processNonDutyTrainTSI( const StateUpdateTypes::NonDutyTrainTSI& event,
                                             unsigned long senderEntity ) = 0;


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event,
                                                unsigned long senderEntity ) = 0;


        /**
         * This will receive a RadioCallDetailsSet message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processRadioCallDetailsSet( const StateUpdateTypes::RadioCallDetailsSet& event,
                                                 unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainSelectionMap message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainSelectionMap( const StateUpdateTypes::TrainSelectionMap& event,
                                               unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainRadioGroup message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainRadioGroup( const RadioGroupTypes::TrainRadioGroup& event,
                                             unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainCommsSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCommsSyncState( const StateUpdateTypes::TrainCommsSyncState& event,
                                                 unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainCctvSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCctvSyncState( const StateUpdateTypes::TrainCctvSyncState& event,
                                                unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainAlarmSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainAlarmSyncState( const StateUpdateTypes::TrainAlarmSyncState& event,
                                                 unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainCallSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCallSyncState( const StateUpdateTypes::TrainCallSyncState& event,
                                                unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainPaSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainPaSyncState( const StateUpdateTypes::TrainPaSyncState& event,
                                              unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainPecSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainPecSyncState( const StateUpdateTypes::TrainPecSyncState& event,
                                               unsigned long senderEntity ) = 0;


        /**
         * This will receive a TrainTisSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainTisSyncState( const StateUpdateTypes::TrainTisSyncState& event,
                                               unsigned long senderEntity ) = 0;


    protected:

        /**
         * Constructor
         */
        ITrainStateUpdateObserver() 
		{

		}

        /**
         * Destructor
         */
        virtual ~ITrainStateUpdateObserver() 
		{
		
		}

    };

}

#endif // !defined(ITrainStateUpdateObserver_H)
