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
#if !defined(OccOperationModeManager_H)
#define OccOperationModeManager_H

#include "app/trains/train_agent/src/OperationModeManager.h"

namespace TA_IRS_App
{

    /**
     * This is the operation mode manager with OCC specific logic
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:54 PM
     */
    class OccOperationModeManager : public OperationModeManager
    {

    public:

        /**
         * Constructor with required information
         *
         * @param configuration    The agent configuration object
         * @param stateUpdateSender    Used to send updates to peer agents
         * @param messageSender    Used to send updates to clients
         * @param statusManager    Used to query train state
         * @param dutyAgent    Used to access the local duty agent
         */
        OccOperationModeManager( ITrainAgentConfiguration& configuration,
                                 IStateUpdateMessageSender& stateUpdateSender,
                                 IMessageSender& messageSender,
                                 ITrainStatusManager& statusManager,
                                 IDutyAgentInterface& dutyAgent );


        /**
         * Destructor
         */
        virtual ~OccOperationModeManager();


        /**
         * This processes a change of state for the train agent subsystem at a location.
         *
         * @param dutyState    The duty state that changed
         */
        virtual void processSubsystemStateChange( const SubsystemDutyState& dutyState );


    protected:


        /**
         * This will do any initialisation required when the agent is in control mode.
         */
        virtual void controlModeInitialisation();
        virtual void syncWithDutyAgent();//limin++, CL-21120


    private:


        /**
         * Private default constructor
         */
        OccOperationModeManager();
         //DTL-2198 Removed m_previousSubsystemStates, due to it is no reasonable to update this cache on both  syncWithDutyAgent and processSubsystemStateChange
        //SubsystemDutyStateList m_previousSubsystemStates;//limin++, CL-21120

    };

}  // TA_IRS_App

#endif // !defined(OccOperationModeManager_H)
