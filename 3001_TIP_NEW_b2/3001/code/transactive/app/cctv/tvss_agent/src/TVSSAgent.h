/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP/3001/transactive/app/cctv/tvs_agent/src/TVSSAgent.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/04 10:43:52 $
  * Last modified by:  $Author:  $
  *
  */
#ifndef TVSSAGENT_H
#define TVSSAGENT_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"
//#include "core/data_access_interface/entity_access/src/TVSSAgentEntityData.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "app/cctv/tvss_agent/src/TVSSServerLinkStatusUpdateThread.h"
#include "app/cctv/tvss_agent/src/TVSSServant.h"


#define TRAINBORNE_MONITOR_TYPE		"TrainBorneMonitor"

namespace TA_Base_Core
{
	class IEntityData;
}

namespace TA_Base_Bus
{
	class GenericAgent;
	class DataPoint;
	class DataPointFactory;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;

}

namespace TA_IRS_App
{
	class TVSSServant;
	class TrainBorneMonitor;
	class TrainBorneCameraAlarm;
	class TVSSAlarmProcessor;
    class TrainBorneHeartBeatProcessor;

	class TVSSAgent : public virtual TA_Base_Bus::IGenericAgentUser,
		              public virtual TA_Base_Bus::IDataPointAgent,
					  public virtual TA_Base_Bus::IDataNodeAgent
	{
		typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;

	public:

	   /**
        * Constructor
        */
        TVSSAgent(int argc, char* argv[]);

       /**
        * Destructor
        */
         virtual ~TVSSAgent();

		//load VideoSwitch Monitors according to the EntityType
		void loadMonitors(unsigned long locationKey);
		//load TrainBorne Camera Alarms according to the EntityType
		void loadTrainBorneCamerasAlarm(unsigned long locationKey);

		//handle the operation mode
		virtual void checkOperationMode(){};

	   /**
        * agentTerminate
        *
        * This method is called by the GenericAgent when it is time to
        * terminate.
        */
		 void agentTerminate();
		
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
          * If create entity throws an exception when Generic Agent is creating its
          * vector of entities (in the GenericAgent::doEntityConfiguration method), 
          * the Generic Agent construction will fail.
          *
          * @param Entity data retrieved from the database.
          *
          * @return The constructed entity
          */
	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);
		
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

		//
		// operations required by IDataPointAgent interface
		//

		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );
		
		virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );
		
		virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();

		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );

		virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );
		/**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether or not they
          * are operating in monitor or control mode.
          */
        virtual void registerForStateUpdates();

		virtual TVSSServant* getTVSSServant() { return m_tvssServant; };
		
	   /**
		* run
		*
	    * This method calls GenericAgent::run() which blocks until the System
	    * Controller shuts us down
		*/
		void run();

		void setInitialOperationMode();

	protected:
		void cleanUp();

	private:
		// Disable default constructor, copy constructor and assignment operator
		TVSSAgent();
        TVSSAgent( const TVSSAgent& tvsAgent);
        TVSSAgent& operator=(const TVSSAgent &);
		std::map< unsigned long, TA_Base_Bus::DataPoint * > m_tvssDataPointList;
		TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;


		TA_Base_Bus::GenericAgent*                  m_genericAgent;
		TVSSServant*                                m_tvssServant;
		TVSSAlarmProcessor*						    m_alarmProcessor;

		TrainBorneHeartBeatProcessor*				m_trainborneHeartbeatProcessor;
		//TA_Base_Core::TVSSAgentEntityDataPtr        m_entityData;
		TA_Base_Core::IEntityDataPtr			    m_entityData;
	protected:

		

		TA_Base_Bus::DataPointFactory*              m_dataPointFactory;

		TVSSServerLinkStatusUpdateThread*           m_linkStatusThread;

		TA_Base_Bus::ScadaProxyFactory*             m_scadaProxyFactory;

	
		//TA_Base_Core::IEntityDataPtr		        m_entityData;
		TA_Base_Bus::SummaryProcessor*              m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr            m_persistence;

		TA_Base_Bus::DataPointAgentCorbaDef_Impl*   m_dpBulkPolledInterface;
		TA_Base_Bus::DataPointAccessCorbaDef_Impl*  m_dpAccessInterface;
		TA_Base_Bus::DataNodeAgentCorbaDef_Impl*    m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl*   m_dnAccessInterface;
		
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
		TrainBorneMonitor*	createMonitor(TA_Base_Core::IEntityDataPtr entityData);
		TrainBorneCameraAlarm*	createTrainBorneCameraAlarm(TA_Base_Core::IEntityDataPtr entityData);
		DataNodeList m_dataNodes;

		// aget name f
		std::string m_agentAssetName;
		std::list<TrainBorneMonitor*> m_monitorPKeyList;
		std::map<unsigned long,TrainBorneMonitor*> m_monitorConsoleMap;
		std::map<std::string,TrainBorneCameraAlarm*> m_alarmAddressMap;
		std::map<unsigned long,TrainBorneCameraAlarm*> m_alarmKeyMap;
	}; 
}

#endif