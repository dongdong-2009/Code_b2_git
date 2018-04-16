#ifndef TISAGENT_H_INCLUDED
#define TISAGENT_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TISAgent.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/03/16 17:17:15 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#pragma warning(disable:4786)
#pragma warning(disable:4146)
#pragma warning(disable:4018)

#include "core/data_access_interface/src/ILocation.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/DPRequestProcessor.h"


namespace TA_Base_Bus 
{
    class GenericAgent; // Forward Declaration
    class IEntity; // Forward Declaration
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;

}

namespace TA_Base_Core
{
	

	class IEntityData; // Forward Declaration

	class ConfigUpdateDetails; // Forward Declaration

	struct StateUpdateMessageCorbaDef; // Forward Declaration
}



//class TTISManager;

namespace TA_IRS_App
{
	class TISAgent : public virtual TA_Base_Bus::IGenericAgentUser,
					 public virtual TA_Base_Bus::IDataPointAgent,
					 public virtual TA_Base_Bus::IDataNodeAgent
	{
	typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
	public:

		/**
		  * Constructor
		  *
		  * @param The number of command line arguments passed in
		  * @param The command line
		  *
		  * @exception GenericAgentException thrown if GenericAgent fails to
		  *            initialise.
		  */
		TISAgent(int argc, char* argv[]);

		/**
		  * Destructor
		  */
		virtual ~TISAgent();

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
		  * This method is called by the GenericAgent to allow the child entities to be
		  * created.  These entities are then managed by the generic agent.
		  *
		  * @param Entity data retrieved from the database.
		  *
		  * @return The constructed entity
		  */
	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		/**
		  * agentSetMonitor
		  *
		  * This method is called by GenericAgent when the Alarm Agent operating mode is
		  * set to Monitor.
		  */
		void agentSetMonitor();

		/**
		  * agentSetControl
		  *
		  * This method is called by GenericAgent when the Alarm Agent operating mode is
		  * set to Control.
		  */
		void agentSetControl();

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
		  * registerForStateUpdates
		  *
		  * This pure virtual method forces agents to subscribe for either
		  * state updates or state update requests, depending on whether or not they
		  * are operating in monitor or control mode.
		  */
		void registerForStateUpdates();

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  */
		void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/**
		  * processOnlineUpdate
		  *
		  * This is called by Generic Agent whenever it gets an online update that it
		  * cant handle itself.
		  *
		  * @param The update event
		  */
		void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		/**
		  * run
		  *
		  * This method calls GenericAgent::run() which blocks until the System
		  * Controller shuts us down
		  */
		void run();


   		//
		// operations required by IDataPointAgent interface
		//

		/**
		  * getDataPoint
		  *
		  * Allows the bulk poll library to obtain a reference to the data point, specified by
		  * the pkey, which is created and maintained by the agent.  A null pointer is returned
		  * if the agent does not have a data point whose pkey matches with the specified pkey.
		  *
		  * @param	entityKey	The pkey of the wanted data point
		  *
		  */
		virtual TA_Base_Bus::DataPoint* getDataPoint ( unsigned long entityKey );

		/**
		  * getAllDataPoints
		  *
		  * Allows the bulk poll library to obtain a reference to the data point, specified by
		  * the pkey, which is created and maintained by the agent.  A null pointer is returned
		  * if the agent does not have a data point whose pkey matches with the specified pkey.
		  *
		  * @param	entityKey	The pkey of the wanted data point
		  *
		  */
		virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );
        
        virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
        
		//
		// operations required by IDataNodeAgent interface
		//
		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );

		/**
		* getAllDataNodes 
		* 
		* 
		* @return virtual void 
		* @param : std::map < unsigned long
		* @param : TA_Base_Bus::DataNode *>& dataNodeList
		* 
		* @exception <exceptions> Optional
		*/
        virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );

		virtual void beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity);


	private:
		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode();
		/**
		  * registerForControlMessages
		  *
		  * This method registers for the messages we expect to receive while in 
		  * control mode.
		  */
		void registerForControlMessages();

		/**
		  * registerForMonitorMessages
		  *
		  * This method registers for the messages we expect to receive while in 
		  * monitor mode.
		  */
		void registerForMonitorMessages();


		/**
		  * receiveRequestForStateUpdateMsg
		  *
		  * This method is called we receive a request for
		  * state information (this should only happen if we are in Control mode)
		  */
		void receiveRequestForStateUpdateMsg();

		TA_Base_Bus::IEntity* createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData );

		std::string getEquipmentName( const std::string& fullName );

		std::string getShortDataPointName( const std::string& fullName );

		// Disable copy constructor and assignment operator
		TISAgent( const TISAgent& theAgent);
		TISAgent& operator=(const TISAgent &);

		// The Generic Agent
		TA_Base_Bus::GenericAgent* m_genericAgent;

		// The location of this agent ( standard CCL 3-character string, e.g. "DBG" )
		std::string m_location;

        // is this agent at the depot, occ, or a station
        TA_Base_Core::ILocation::ELocationType m_locationType;

        TA_Base_Bus::DataPointFactory* m_dataPointFactory;

        // Bulk polling library
        TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_dpBulkPolledInterface;
		TA_Base_Bus::DataPointAccessCorbaDef_Impl* m_dpAccessInterface;

        // Agent name
        std::string m_agentName;

		//datapoint map lock
		TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;

		//datapoint map
		std::map< unsigned long, TA_Base_Bus::DataPoint * > m_tisDataPointList;
		
		TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_dnAccessInterface;
		TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
		TA_Base_Bus::SummaryProcessor* m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr m_persistence;
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
		DataNodeList m_dataNodes;
		DPRequestProcessor m_dpReqProcessor; //CL16505
	};

} // TA_IRS_App

#endif // TISAGENT_H_INCLUDED