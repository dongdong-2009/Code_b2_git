/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2012/06/12 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(TrainAgent_H)
#define TrainAgent_H

//#include "bus/managed_agent/src/AbstractManagedAgent.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/trains/TrainProtocolLibrary/src/TrainEventFactoryImpl.h"


namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
    class CtaAuditing;
    class CtaAlarms;
    class TrainAgentEntityData;
    typedef boost::shared_ptr< TrainAgentEntityData > TrainAgentEntityDataPtr;
}

namespace TA_Base_Bus
{
    class CtaRights;
}

namespace TA_IRS_Bus
{
    class TrainTransactionManager;
}

namespace TA_IRS_App
{
    class TrainAgentConfiguration;
    class MessageSender;
    class RightsChecker;
    class AuditSender;
    class AlarmUtility;
    class TrainAgentStatusManager;
    class StateUpdateManager;
    class AtsAgentInterface;
    class AtsMessageProcessor;
    class DutyAgentInterface;
    class CctvAgentInterface;
    class RadioAgentInterface;
    class OperationModeManager;
    class RadioGroupCoordinator;
    class TrainInformation;
    class TimsModeManager;
    class TimsStatusManager;
    class TimsCctvManager;
    class TimsPecManager;
    class TimsDriverCallManager;
    class TimsTisManager;
    class TimsPaManager;
    class TrainOperationModeCorbaServant;
    class TrainStateUpdateCorbaServant;
    class TrainRadioGroupCorbaServant;
    class TrainInformationCorbaServant;
    class TrainCctvCorbaServant;
    class TrainPecCorbaServant;
    class TrainDriverCallCorbaServant;
    class TrainTisCorbaServant;
    class TrainPaCorbaServant;
    class DutyAgentObserver;
    class AtsAgentObserver;
    class RadioAgentObserver;
    class RadioMessageProcessor;
}


namespace TA_IRS_App
{

    /**
     * This is the main train agent class, it ties all the components of the train agent together.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:18 PM
     */
    class TrainAgent : public TA_Base_Bus::IGenericAgentUser
    {

    public:

        /**
         * Constructor
         */
        TrainAgent(int argc, char* argv[]);;


        /**
         * Destructor
         */
        ~TrainAgent();


        /**
         * This allows for bulk entity creation. It is called first (before any calls to createEntity)
         * to allow agents to optimise entity creation. If true is returned,
         * the entities are already created if false is returned, then createEntity is called
         * one by one for each entity. Also entityCreationComplete will be called at the end of the creation
         * Note that even if using this method, createEntity should still be implemented as it is used for entity updates 
         * 
         * @param agentEntityData the agent's entity data
         * @return true if entity creation was done, false if createEntity should be used
         * 
         * @param entityDataList    a list of shared pointers to all entities
         * @param createdEntities    the entities created to return to generic agent
         */
        virtual bool createAllEntities( const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities );


        /**
         * This method takes Entity Configuration data stored in the database and generates an object
         * that implements the IEntity interface. The returned objects are stored and managed within Managed Agent.
         * The Agent can get a pointer to the objects via the getEntityMap method in ManagedAgent.
         * If create entity throws an exception when Managed Agent is creating its vector of entities
         * (in the ManagedAgent::doEntityConfiguration method), the Managed Agent construction will fail. 
         *
         * @return The constructed entity
         * 
         * @param entityData    data retrieved from the database.
         */
        virtual TA_Base_Bus::IEntity* createEntity( TA_Base_Core::IEntityDataPtr entityData );

		
		// operations required by IGenericAgentUser
		virtual void checkOperationMode()
		{
			//do nothing...
		};

		/**
		* receiveSpecialisedMessage
		*
		* This method is called whenever the Agent receives a state update or
		* state update request (depending on their mode of operation)
		*/
		void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/** revert to 1309 function*/
		/**
		* agentSetMonitor
		*
		* This method is called when the agent is placed into monitor mode. It is 
		* responsible for making any necessary state changes in the agent to place it in
		* monitor mode.
		*/
        void agentSetMonitor();
		
		/**
		* agentSetControl
		*
		* This method is called when the agent is placed into control mode. It is 
		* responsible for making any necessary state changes in the agent to place it in
		* control mode.
		*/	
		void agentSetControl();

		/**
		* registerForStateUpdates
		*
		* This method is called upon initialisation. It is to be used to register the 
		* agent for any necessary state updates.
		*/
        void registerForStateUpdates();
		
        /**
		* run
		*
		* This method is called by the agent mainline, and is used start any necessary
		* threads before calling GenericAgent::run which will block until the process
		* is terminated.
		*/
        void run();		
		/** revert finished*/


        /**
         * 
         * @param group
         */
        virtual void notifyGroupOffline( const std::string& group );


        /**
         * 
         * @param group
         */
        virtual void notifyGroupOnline( const std::string& group );


        /**
         * This will stop all processing, and delete all objects.
         */
        virtual void agentTerminate();


        /**
         * processOnlineUpdate  This is pure virtual method that must be implemented by underlying agent.
         * When there is a configuration update of the type and key matching one registered by this class,
         * this method will be invoked to process the update accordingly. 
         * 
         * @param updateEvent    updateEvent
         */
        virtual void processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

    
    private:

        
        /**
         * This will stop anything that is currently running or processing so the agent can shut down
         */
        void stopAllActivity();


        /**
         * This is called once the activity has been stopped, and the agent is shutting down.
         * Delete order is reverse of creation order
         */
        void deleteAllObjects();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         * 
         * @param trainAgentEntityData    The specific agent entity data
         */
        void createGroupOne( TA_Base_Core::TrainAgentEntityDataPtr trainAgentEntityData );


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupTwo();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupThree();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupFour();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupFive();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         * 
         * @param trainAgentEntityData    The specific agent entity data
         */
        void createGroupSix( TA_Base_Core::TrainAgentEntityDataPtr trainAgentEntityData );


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupSeven();


        /**
         * This creates one of the initialisation groups based on creation dependency order.
         * This is created at agent startup.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void createGroupEight();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupOne();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupTwo();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupThree();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupFour();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupFive();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupSix();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupSeven();


        /**
         * This deletes one of the initialisation groups based on creation dependency order.
         * This is done at agent shutdown.
         * See the design, diagram "Train Agent Initialisation - Internal Component Dependencies" for a graphical view of component dependencies.
         */
        void deleteGroupEight();


        void agentSetControlInThread();


		/**
		* The Generic Agent
		*/
		TA_Base_Bus::GenericAgent *m_genericAgent;

        /**
         * This is the protocol specific event factory that will be used to create train event objects from raw data
         */
        TA_IRS_Bus::TrainEventFactoryImpl m_trainEventFactoryImpl;


        /**
         * This is set to false once the agent has gone into control or monitor mode for the first time.
         * It is a flag to indicate whether this agent is the first agent starting up at a location.
         */
        bool m_firstRunMode;


        /**
         * A reference to the thread pool
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * Stores all agent configuration items
         */
        TrainAgentConfiguration* m_configuration;


        /**
         * Used to send both comms messages to clients of this agent, and state updates to other train agents
         */
        MessageSender* m_messageSender;


        /**
         * Used to check rights
         */
        TA_Base_Bus::CtaRights* m_ctaRights;


        /**
         * This is a wrapper aroung ItaRights that contains all train agent access controlled actions
         */
        RightsChecker* m_rightsChecker;


        /**
         * Used to send audit messages
         */
        TA_Base_Core::CtaAuditing* m_ctaAuditing;


        /**
         * A wrapper around ItaAuditing that will build and submit train agent specific audit messages
         */
        AuditSender* m_auditSender;


        /**
         * Used to raise and close alarms
         */
        TA_Base_Core::CtaAlarms* m_ctaAlarms;


        /**
         * A wrapper around ItaAlarms that will format and submit train agent specific alarms
         */
        AlarmUtility* m_alarmUtility;


        /**
         * Used to store, manage, and access train states
         */
        TrainAgentStatusManager* m_trainAgentStatusManager;


        /**
         * This handles peer agent synchronisation
         */
        StateUpdateManager* m_stateUpdateManager;


        /**
         * An object used to access the local ats agent
         */
        AtsAgentInterface* m_atsAgentInterface;


        /**
         * This is used by object wanting to subscribe for ats updates
         */
        AtsMessageProcessor* m_atsMessageProcessor;


        /**
         * An object used to access the local duty agent
         */
        DutyAgentInterface* m_dutyAgentInterface;


        /**
         * An object used to access the local video switch agent
         */
        CctvAgentInterface* m_cctvAgentInterface;


        /**
         * An object used to access the local radio agent
         */
        RadioAgentInterface* m_radioAgentInterface;


        /**
         * This is used to send train commands and receive responses and unsolicited messages from trains
         */
        TA_IRS_Bus::TrainTransactionManager* m_trainTransactionManager;


        /**
         * This is used by object wanting to subscribe for radio updates
         */
        RadioMessageProcessor* m_radioMessageProcessor;


        /**
         * The object used to determine and manage the mode this agent runs in
         */
        OperationModeManager* m_operationModeManager;


        /**
         * This manages the availability and use of train GOA groups
         */
        RadioGroupCoordinator* m_radioGroupCoordinator;


        /**
         * This is used by clients to query train information
         */
        TrainInformation* m_trainInformation;


        /**
         * This handles the TIMS operation mode on each train
         */
        TimsModeManager* m_timsModeManager;


        /**
         * This handles the overall status of TIMS on each train
         */
        TimsStatusManager* m_timsStatusManager;


        /**
         * This handles all TIMS CCTV operations
         */
        TimsCctvManager* m_timsCctvManager;


        /**
         * This handles all TIMS PEC operations
         */
        TimsPecManager* m_timsPecManager;


        /**
         * This handles all TIMS Driver Call operations
         */
        TimsDriverCallManager* m_timsDriverCallManager;


        /**
         * This handles all TIMS TIS operations
         */
        TimsTisManager* m_timsTisManager;


        /**
         * This handles all TIMS PA operations
         */
        TimsPaManager* m_timsPaManager;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainOperationModeCorbaServant* m_trainOperationModeCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainStateUpdateCorbaServant* m_trainStateUpdateCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainRadioGroupCorbaServant* m_trainRadioGroupCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainInformationCorbaServant* m_trainInformationCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainCctvCorbaServant* m_trainCctvCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainPecCorbaServant* m_trainPecCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainDriverCallCorbaServant* m_trainDriverCallCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainTisCorbaServant* m_trainTisCorbaServant;


        /**
         * This is the implementation of a corba servant,
         * it delegates calls off to the objects that do the actual work, and converts data between C++ and CORBA types.
         */
        TrainPaCorbaServant* m_trainPaCorbaServant;


        /**
         * This actually subcribes to and passes on updates from the duty agent
         */
        DutyAgentObserver* m_dutyAgentObserver;


        /**
         * This actually subcribes to and passes on updates from the ats agent
         */
        AtsAgentObserver* m_atsAgentObserver;


        /**
         * This actually subcribes to and passes on updates from the radio agent
         */
        RadioAgentObserver* m_radioAgentObserver;

    };

} // TA_IRS_App

#endif // !defined(TrainAgent_H)
