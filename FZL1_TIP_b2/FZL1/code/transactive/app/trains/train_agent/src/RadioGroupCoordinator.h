/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(RadioGroupCoordinator_H)
#define RadioGroupCoordinator_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "app/trains/train_agent/src/IRadioGroupCoordinator.h"

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
//#include "bus/managed_agent/src/IOperationStateCallback.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class IStateUpdateMessageSender;
    class IMessageSender;
}

namespace TA_IRS_App
{

    /**
     * This class handles radio group resource management.
     * @author Robert V
     * @version 1.0
     * @created 01-Apr-2008 2:47:59 PM
     */
    class RadioGroupCoordinator : public IRadioGroupCoordinator
    {

    public:

        /**
         * Constructor with information
         *
         * @param configuration    The agent configuration
         * @param messageSender    Used to send update messages to clients
         * @param stateUpdateSender    used to send updates to other train agents
         */
        RadioGroupCoordinator( ITrainAgentConfiguration& configuration,
                               IMessageSender& messageSender,
                               IStateUpdateMessageSender& stateUpdateSender );


        /**
         * Destructor
         */
        virtual ~RadioGroupCoordinator();


        /**
         * This is second stage initialisation triggered by ManagedAgent.
         */
        void initialise();


        /**
         * Gets the current radio group list
         *
         * @return all radio groups
         */
        virtual TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap getRadioGroups();


        /**
         * Gets the current radio group list
         *
         * @return the matching radio group
         *
         * @exception InvalidParameterException if the group tsi doesnt exist
         *
         * @param groupTsi    The group
         */
        virtual TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup getRadioGroup( const std::string& groupTsi );


        /**
         * This will attempt to lock a radio group for use.
         * It will ensure that it is not already locked (either directly or indirectly).
         * It will then lock it, and will notify all other agents and applications of the new status.
         *
         * @exception GroupInUseException if the group is already locked
         *
         * @param groupTsi    The group to attempt to lock
         */
        virtual void lockRadioGroup( const std::string& groupTsi );


        /**
         * This will unlock a radio group that was previously locked.
         * It will notify all other agents and applications of the new status.
         *
         * @param groupTsi    The group to attempt to lock
         */
        virtual void unlockRadioGroup( const std::string& groupTsi );


        /**
         * This will sync the given radio group from a state update.
         *
         * @param radioGroup    The group to sync
         */
        void syncRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& radioGroup );


        /**
         * This will sync the given radio groups from a state update.
         *
         * @param radioGroups    The groups to sync
         */
        void syncRadioGroupList( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap& radioGroups );


        /**
         * The operation mode of the agent has changed.
         *
         * When going to control for the first time, current state must be sent to clients
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


    private:


        /**
         * Private default constructor
         */
        RadioGroupCoordinator();


        /**
         * This will load the initial radio groups from the database
         */
        void loadRadioGroups();


        /**
         * These are the radio groups, indexed by group TSI
         */
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap m_radioGroups;


        /**
         * This protects the group map and its contents
         */
        TA_Base_Core::NonReEntrantThreadLockable m_radioGroupLock;


        /**
         * Used to access agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * used to send messages to clients
         */
        IMessageSender& m_messageSender;


        /**
         * Used to send state updates to other agents
         */
        IStateUpdateMessageSender& m_stateUpdateSender;

    };

}  // TA_IRS_App

#endif // !defined(RadioGroupCoordinator_H)
