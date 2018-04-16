/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/TisAgent.h $
  * @author:	HoaVu
  * @version:	$Revision: #5 $
  *
  * Last modification:	$DateTime: 2011/03/17 16:52:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	TisAgent implementes the GenericAgent interface.  TisAgent monitors and controls the 
  * the entities associated with its process, such as TIS RTU and the DataPoint entities.
  *
  */

#ifndef TRAINALARM_TISAGENT_H_INCLUDED
#define TRAINALARM_TISAGENT_H_INCLUDED

#include "TisDpProcessor.h"
#include "TisStandardRtuManager.h"

#include "bus/scada/scada_agent/src/ScadaAgentUser.h"
#include "bus/scada/rtu_library/src/StandardRTU.h"
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

#include <map>
#include <vector>
#include <utility>

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
	class RTU;	
	class RTUManager;
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

	class TisAgent : public virtual TA_Base_Bus::ScadaAgentUser
	{
    typedef std::map<int, TisStandardRtuManager*> RTUManagerMap;

	public:
		TisAgent();
		virtual ~TisAgent();

		//
		// operations required by IGenericAgentUser
		//

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
		  * startTisAgent
		  *
		  * Start this TisAgent process
		  *
		  */
		void startTisAgent(int argc, char* argv[]);

	// operations
	private:
		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode(){};
		/**
		  * loadAgentParamValues
		  *
		  * Load the TisAgent entity's parameter from the database
		  *
		  */
		void loadAgentParamValues();

		/**
		  * startProcessors
		  *
		  * Start up all processors associated with the TisAgent entity
		  *
		  */
		void startProcessors();

		/**
		  * stopProcessors
		  *
		  * Stop all processors associated with the TisAgent entity
		  *
		  */
		void stopProcessors();

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

		void updateTisAgentConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

		void updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

		/**
		 * createEMSWorkers
		 *
		 * create rtu internal worker utilities
		 *
		 */
        void createRTUWorkers();
            // attributes

	private:
		unsigned long m_tisAgentPrimaryKey;
        // int m_proxyDpMonitoringPeriodSecs;
        TA_Base_Core::EOperationMode	m_operationMode;
		bool m_bulkInterfaceStarted;        		        						
		std::string m_stationName;
		std::string m_agentName;
		std::string m_agentAssetName;
		
        bool m_hasOtherBusiness;
        int m_analogueDpUpdateInterval;
        bool m_isRTUSwitchingDisabled;

		RTUManagerMap    m_rtuManagers;        
        
        // the following 2 lines stores train data configuration from the database
        std::vector<std::string> m_trainTypes;
        std::vector<std::pair<std::string, unsigned long> > m_startAddressOfTrains, m_trainNumbers, m_trainInfoLens;

        TisDpProcessor* m_tisDpProcessor;
        std::map<unsigned long, TA_Base_Core::RTUEntityData*> m_rtuEntityDataMap;        
	};
}

#endif

