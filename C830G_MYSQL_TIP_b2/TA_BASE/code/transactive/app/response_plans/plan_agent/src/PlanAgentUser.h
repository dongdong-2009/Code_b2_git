#ifndef PLANAGENTUSER_H_INCLUDED
#define PLANAGENTUSER_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class PlanAgentUser : public TA_Base_Bus::IGenericAgentUser
	{
	// Public methods
	public:
		virtual ~PlanAgentUser();

		virtual void checkOperationMode();
		// GenericAgent interface
		virtual void agentTerminate();
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);
		virtual void agentSetMonitor();
		virtual void agentSetControl();
        virtual void notifyGroupOffline( const std::string& group );
        virtual void notifyGroupOnline( const std::string& group );

		virtual void registerForStateUpdates();
        virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
        void updatePlanAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails& updateEvent );

		// SpecialisedMessageSubscriber interface
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

        // Handle new packed message of all plan states update
        void handlePASUpdateData( TA_Base_Core::PASUpdateData* updateData, TA_Base_Core::UpdateId& lastPasUpdateId );

        void initialActivePlanStateFromControlAgent();

		void run();

		void registerForMonitorMessages();
		void registerForControlMessages();

		unsigned long getAgentEntityKey() { return m_entityKey; }
		unsigned long getAgentEntityTypeKey() { return m_entityTypeKey; }
		unsigned long getAgentSubsystemKey() { return m_subsystemKey; }
		unsigned long getAgentLocationKey() { return m_locationKey; }
		
		const std::string& getAgentAssetName() { return m_assetName; }
		int getActivePlanExpiryTime() { return m_activePlanExpiryTime; }
		int getActivePlanThreadPoolSize() { return m_activePlanThreadPoolSize; }
		unsigned long getPausedPlanSleepTime() { return m_pausedPlanSleepTime; }

		void initAgentUser( int argc, char* argv[] );

		static PlanAgentUser * getInstance()
		{
			//no need to lock
			if (NULL == m_instance)
			{
				m_instance = new PlanAgentUser();
			}
			TA_ASSERT(m_instance != NULL, "instance should not be NULL");
			return m_instance;
		}

	private:
		PlanAgentUser();

		PlanAgentUser& operator=( const PlanAgentUser& );
		PlanAgentUser( const PlanAgentUser& );
		
		void initialisePlanAgentConfiguration();
        void unsubscribeToMessages();//limin, failover
	
	private:
		TA_Base_Bus::GenericAgent* m_genericAgent;
		TA_Base_App::PlanAgentCorbaDefImpl* m_agent;

		static PlanAgentUser* m_instance;

		bool m_agentControlMode;
        bool m_contorlDataSynced;
        bool m_bFailToSyncState;

        TA_Base_Core::TimedWaitSemaphore m_synMonitorInitialization;

//		std::string m_entityName;
		unsigned long		m_entityKey;
		unsigned long		m_entityTypeKey;
		unsigned long		m_subsystemKey;
		unsigned long		m_locationKey;
		std::string			m_assetName;

		int                 m_activePlanExpiryTime;
		int                 m_activePlanThreadPoolSize;
		unsigned long		m_pausedPlanSleepTime; 	
	}; // PlanAgentUser
}; // TA_Base_App

#endif // PLANAGENTUSER_H_INCLUDED
