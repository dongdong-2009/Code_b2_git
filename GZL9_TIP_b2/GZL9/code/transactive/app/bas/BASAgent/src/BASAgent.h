/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BASAgent.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/29 19:57:02 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *
  */

#if !defined(APP_BAS_BASAgent_BASAgent_H)
#define APP_BAS_BASAgent_BASAgent_H

#include <map>
#include "app/bas/BASAgent/src/IStationModeExecutionProxy.h"
#include "app/bas/BASAgent/src/ITimetableExecutionProxy.h"
#include "app/bas/BASAgent/src/BasAgentController.h"
#include "bus/scada/scada_agent/src/ScadaAgentUser.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/threads/src/Thread.h"

//BAS Self Logic
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"

namespace TA_Base_Bus
{
	class DataPointFactory;
	class LocalUpdateEventProcessor;
	class GenericAgent;
	class AgentStateUpdateQueueSingletonManager;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
	class RTUFactory;
};

namespace TA_Base_Core
{
	class AuditMessageSender;
    class MessageType;
	class ThreadPoolSingletonManager;
};

namespace TA_IRS_App
{
	
    class ForcedDataPointSynchroniser;
	class StationModeManager;
	class PeriodicThread;
    class BasAgentController;

	class BASAgent : public virtual TA_Base_Bus::ScadaAgentUser
	{
	public:

		BASAgent ();
		virtual ~BASAgent();

		//
		// operations required by IGenericAgentUser
		//

		/** 
		  * checkOperationMode
		  *
		  * This function check the agent status, it will throw a OperationModeException when 
		  * corba call to this agent. 
		  * @param The group name
		  */
		virtual void checkOperationMode();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
		  * notifyGroupOffline from the Process Monitor.
		  *
		  * @param The group name
		  */
		virtual void notifyGroupOffline( const std::string& group );

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
		  * notifyGroupOnline from the Process Monitor.
		  *
		  * @param The group name
		  */
		virtual void notifyGroupOnline( const std::string& group );

		/**
		  * agentTerminate
		  *
		  * This pure virtual method is called from the onTerminate
		  * method in GenericAgent. This allows the agent to do any
		  * of its own cleaning up before exiting.
		  *
		  * If the agent's entities implement CORBA interfaces,
		  * this method should take care of deactivating them. It is not
		  * possible for GenericAgent to do this because the IEntity 
		  * interface is not a ServantBase.
		  *
		  * If the class implementing this interface holds the GenericAgent
		  * as a member variable, DO NOT delete it inside this method.
		  * GenericAgent waits for this call to return before shutting down
		  * the ORB. Deleting the GenericAgent from within this method will
		  * cause obvious problems.
		  */
		virtual void agentTerminate();

        /** 
          * createAllEntities
          *
          * [optional]
          *
          * This allows for bulk entity creation.
          * It is called first (before any calls to createEntity) to allow agents to
          * optimise entity creation. If true is returned, the entities are already created
          * if false is returned, then createEntity is called one by one for each entity.
          * Also entityCreationComplete will be called at the end of the creation
          *
          * Note that even if using this method, createEntity should still be implemented
          * as it is used for entity updates
          *
          * @param agentEntityData the agent's entity data
          * @param entityDataList a list of shared pointers to all entities
          * @param createdEntities the entities created to return to generic agent
          *
          * @return true if entity creation was done, false if createEntity should be used
          *
          */
        virtual bool createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                        const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities );

        /** 
          * entityCreationComplete
          *
          * [optional]
          *
          * this is called back after 
          * createEntity has been called for all entities.
          * The client can do any clean up of temporary data.
          */
        virtual void entityCreationComplete();

		/**
		  * createEntity
		  *
		  * This method takes Entity Configuration data stored in the database and
		  * generates an object that implements the IEntity interface. The returned
		  * objects are stored and managed within Generic Agent. The Agent can get a 
		  * pointer to the objects via the getEntityMap method in GenericAgent.
		  *
		  * @param Entity data retrieved from the database.
		  *
		  * @return The constructed entity
		  */
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);

		/**
		  * agentSetMonitor
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to monitor.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetMonitor();

		/**
		  * agentSetControl
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to control.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetControl();

		virtual void agentSetStandby(){};

		virtual void beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity) {};

		/**
		  * registerForStateUpdates
		  *
		  * This pure virtual method forces agents to subscribe for either
		  * state updates or state update requests, depending on whether or not they
		  * are operating in monitor or control mode.
		  */
		virtual void registerForStateUpdates();

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  */
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/**
		  * processOnlineUpdate
		  *
		  * Generic Agent will handle the receiving of online updates. However, in some
		  * cases, it will be necessary to pass the updates on to the registered agent.
		  * For example, if a child entity needs to be updated.
		  *
		  * @param The update event
		  */
		virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		/**
		  * startPMSAgent
		  *
		  * Start this PMSAgent process
		  *
		  */
		void startBASAgent(int argc, char* argv[]);


		/**
		  * isAgentRunningControlMode
		  *
		  * get the agent whether running in control mode process
		  *
		  */
		bool isAgentRunningControlMode();

	// operations
	private:
		
		/**
		  * loadAgentParamValues
		  *
		  * Load the PMSAgent entity's parameter from the database
		  *
		  */
		void loadAgentParamValues();

		/**
		  * startProcessors
		  *
		  * Start up all processors associated with the PMSAgent entity
		  *
		  */
		void startProcessors();

		/**
		  * stopProcessors
		  *
		  * Stop all processors associated with the PMSAgent entity
		  *
		  */
		void stopProcessors();

		/**
		 * submitAuditMessage
		 *
		 * Generates an audit message of the specified message type
		 *
		 * @param	description		The description of the audit message
		 * @param	messageType		The audit message type
		 *
		 */
		void submitAuditMessage ( const TA_Base_Core::DescriptionParameters & description,
								  const TA_Base_Core::MessageType & messageType );

		/**
		  * registerForControlMessages
		  *
		  * Registers for the messages expected to receive while in 
		  * control mode.
		  */
		void registerForControlMessages();

		/**
		  * registerForMonitorMessages
		  *
		  * Registers for the messages expected to receive while in 
		  * monitor mode.
		  */
		void registerForMonitorMessages();

		void updateBASAgentConfiguration( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

		void updateDataPointConfiguration( const TA_Base_Core::ConfigUpdateDetails & updateEvent);


		/**
		 * createRTUWorkers
		 *
		 * create rtu internal worker utilities
		 *
		 */
        void createRTUWorkers();
            // attributes

		//BAS Self Logic
		void startupBASFunctions();
		void processModePointsIfNecessary(TA_Base_Bus::DataPoint*);

	private:

		unsigned long m_basAgentPrimaryKey;
		int m_proxyDpMonitoringPeriodSecs;
        TA_Base_Core::EOperationMode	m_operationMode;
        bool m_isRTUSwitchingDisabled;
        int m_analogueDpUpdateInterval;
		std::string m_stationName;
		std::string m_agentName;
		std::string m_agentAssetName;

		//RTUManagerMap    m_rtuManagers;
		TA_Base_Bus::RTUFactory * m_rtuFactory;
		

		// BAS Self Logic
        TA_IRS_App::BasAgentController*         m_basAgentController;
		
        TA_IRS_App::IStationModeExecutionProxy* m_stationModeExecutionImpl;
        TA_IRS_App::ITimetableExecutionProxy*   m_timetableExecutionImpl;
	};
}

#endif //APP_BAS_BASAgent_BASAgent_H
