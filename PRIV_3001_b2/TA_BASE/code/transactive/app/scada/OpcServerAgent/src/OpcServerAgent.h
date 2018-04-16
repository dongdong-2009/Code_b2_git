/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcServerAgent.h $
  * @author:	Dhanshri Mokashi
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2016/07/11 18:24:37 $
  * Last modified by:	$Author: Dhanshri Mokashi $
  *
  * Description:
  *
  *	OpcServerAgent implements the GenericAgent interface. OccOPCServiceAgent will collect the updates from all of the agents in the differect server/stations. 
  *	And pass the data to OCC opcbridge to reflect on multiple OCC MFTs.
  */

#ifndef OPCSERVERAGENT_H
#define OPCSERVERAGENT_H

#include <map>
#include <vector>

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/ISimpleNodeAgent.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/ISimplePointAgent.h"

#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class AgentData;
};

namespace TA_Base_Bus
{
	class GenericAgent;
	class OpcSimplePointAgentCorbaDef_Impl;
	class OpcSimpleNodeAgentCorbaDef_Impl;
	class SimplePoint;
	class SimpleNode;
};

namespace TA_Base_App
{
	class OpcServerManager;

	class OpcServerAgent : public TA_Base_Bus::IGenericAgentUser,
					 public virtual TA_Base_Bus::ISimplePointAgent,
					 public virtual TA_Base_Bus::ISimpleNodeAgent
	{

	public:

		OpcServerAgent ( int argc, char* argv[] );
		virtual ~OpcServerAgent();

		virtual void checkOperationMode()
		{
			//do nothing...
		};
		//
		// operations required by IGenericAgentUser
		//

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
		virtual void agentSetStandby();
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

		virtual void notifyGroupOffline( const std::string& group );

		virtual void notifyGroupOnline( const std::string& group );

		//
		// opearations specific to OpcServerAgent
		//

		/**
		  * startOpcServerAgent
		  *
		  * Start this OpcServerAgent process
		  *
		  */
		void startOpcServerAgent();

		//
		//Functions required by Interface - ISimplePointAgent
		//
		virtual TA_Base_Bus::SimplePoint * getSimplePoint ( unsigned long entityKey ) ;
		virtual void getAllSimplePoints (std::map < unsigned long, TA_Base_Bus::SimplePoint *>& dataPointList ) ;

		//
		//Functions required by Interface - ISimpleNodeAgent
		//
		virtual TA_Base_Bus::SimpleNode * getSimpleNode( unsigned long entityKey );
        virtual void getAllSimpleNodes (std::map < unsigned long, TA_Base_Bus::SimpleNode *>& dataNodeList );
		bool createSpecialEntities();

	// attributes
	private:

		std::string m_stationName;
		std::string m_agentName;

		TA_Base_Core::EOperationMode m_operatorMode;

		TA_Base_Bus::OpcSimpleNodeAgentCorbaDef_Impl* m_bulkDNPolledInterface;
		TA_Base_Bus::OpcSimplePointAgentCorbaDef_Impl* m_bulkDPPolledInterface;

		TA_Base_Bus::GenericAgent * m_genericAgent;
		std::vector<TA_Base_Core::AgentData*> m_dpdnAgentList;

		OpcServerManager* m_pOpcServerManager;
	};
};
#endif

