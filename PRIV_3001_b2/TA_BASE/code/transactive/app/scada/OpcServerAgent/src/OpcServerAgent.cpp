/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcServerAgent.cpp $
  * @author:	Dhanshri Mokashi
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2016/07/11 18:24:37 $
  * Last modified by:	$Author: Dhanshri Mokashi $
  *
  * Description:
  *	OpcServerAgent implements the GenericAgent interface. OccOPCServiceAgent will collect the updates from all of the agents in the differect server/stations. 
  *	And pass the data to OCC opcbridge to reflect on multiple OCC MFTs.
  *
  */

#include "core/data_access_interface/src/OpcServerAgentAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/OpcSimpleNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/OpcSimplePointAgentCorbaDef_Impl.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/SimpleNode.h"
#include "bus/scada/DataOpcServerAgentCorbaDef_Impl/src/SimplePoint.h"

#include "OpcServerManager.h"
#include "OpcServerAgent.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	OpcServerAgent::OpcServerAgent ( int argc, char* argv[] ):m_operatorMode(TA_Base_Core::NotApplicable),
    m_genericAgent( NULL )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating GenericAgent object...");
		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent ( argc, argv, this );

		if ( NULL == m_genericAgent )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Failure to create GenericAgent object.");
			return ;
		}
		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AgentName = %s", m_agentName.c_str() );
		m_pOpcServerManager =  new OpcServerManager;	
		m_dpdnAgentList.empty();
		createSpecialEntities();
	}

	OpcServerAgent::~OpcServerAgent()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Destroying m_pOpcServerManager...");   
		if( NULL != m_pOpcServerManager )
		{	
			delete m_pOpcServerManager;
			m_pOpcServerManager = NULL;
		}
		// if bulk interface is created
		if ( NULL != m_bulkDNPolledInterface )
		{
			delete m_bulkDNPolledInterface;
			m_bulkDNPolledInterface = NULL;
		}

		if ( NULL != m_bulkDPPolledInterface )
		{
			delete m_bulkDPPolledInterface;
			m_bulkDPPolledInterface = NULL;
		}

		std::vector<TA_Base_Core::AgentData*>::iterator itList = m_dpdnAgentList.begin();

		while (itList != m_dpdnAgentList.end())
		{
			if((*itList) != NULL)
			{
				delete (*itList);
				(*itList) = NULL;
			}
		}
		m_dpdnAgentList.clear();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Destroying GenericAgent object...");
		// clean up the GenericAgent
		if ( NULL != m_genericAgent )
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = NULL;
			}
			catch ( ... )
			{
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Failed to deactivate and delete m_genericAgent servant.");
			}
		}
	}

    void OpcServerAgent::agentTerminate()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug," In OpcServerAgent::agentTerminate()");
		m_pOpcServerManager->terminateAndWait();
	}

    /**
      * createAllEntities
      *
      * This method is called by GenericAgent. This agent does not own any datapoints/datanodes; 
	  * Due to the absence of these internal DP/DN, no need to create any entity.
	*/
	bool OpcServerAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                           const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                           TA_Base_Bus::IEntityList& createdEntities)
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In createAllEntities() - EMPTY" );

		if(0 < entityDataList.size())
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "This agent is not supposed to have any internal entities like Datanode or Datapoint." );
			return true;
		}
		// return true to indicate not to call createEntity
        return true;
	}
	
	bool OpcServerAgent::createSpecialEntities()
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In createSpecialEntities()..." );

		TA_Base_Core::OpcServerAgentAccessFactory::getOccAndStationAgentList(m_dpdnAgentList);
		unsigned int i = 0;
		unsigned int nSize = m_dpdnAgentList.size();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Total agents = %d; Creating lightweight version of all DataNodes and DataPoints", nSize);

		std::vector<TA_Base_Core::AgentData*>::const_iterator it = m_dpdnAgentList.begin();
		for (i = 0; i < nSize; ++i)
		{
			int cDN = 0, cDP = 0;
			if( ((*it)->agentKey) > 0)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Accessing data of agent name = %s", (*it)->agentName.c_str());
				// Get DN list
				TA_Base_Core::IEntityDataList edListDN = TA_Base_Core::EntityAccessFactory::getInstance().getDescendantsOfAgentOfType((*it)->agentName.c_str(), TA_Base_Core::DataNodeEntityData::getStaticType());
				for ( TA_Base_Core::IEntityDataList::const_iterator itDN = edListDN.begin();itDN != edListDN.end(); ++itDN )
				{
					if( (*itDN)->getKey() > 0)
					{	
						m_pOpcServerManager->addSimpleNode((*itDN)->getKey(), (*it)->agentKey);
					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Agent with Agent key %u has invalid DN key %u", (*it)->agentKey, (*itDN)->getKey());
					}
					cDN++;
				}
				
				// Get DP list
				TA_Base_Core::IEntityDataList edListDP = TA_Base_Core::EntityAccessFactory::getInstance().getDescendantsOfAgentOfType((*it)->agentName.c_str(), TA_Base_Core::DataPointEntityData::getStaticType());
				for ( TA_Base_Core::IEntityDataList::const_iterator itDP = edListDP.begin();itDP != edListDP.end(); ++itDP )
				{
					if( (*itDP)->getKey() > 0)
					{	
						m_pOpcServerManager->addSimplePoint((*itDP)->getKey(), (*it)->agentKey);
					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Agent with Agent key %u has invalid DP key %u", (*it)->agentKey, (*itDP)->getKey());
					}

					cDP++;
				}
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "For this agent, Total DP=%d: DN=%d", cDP, cDN);
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Agent name = %s, has Agent key = %u", (*it)->agentName.c_str(), (*it)->agentKey);
			}
			//Move on to get next agent to fill up DN/DP lists...
			it++;
		}
		m_pOpcServerManager->createAgentProxy(m_dpdnAgentList);

		// return true to indicate not to call createEntity
        return true;
    }


	TA_Base_Bus::IEntity* OpcServerAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In createEntity() - Empty routine.." );
        return 0;
	}

    void OpcServerAgent::entityCreationComplete()
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - entityCreationComplete" );
    }

    void OpcServerAgent::agentSetMonitor()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In OpcServerAgent::agentSetMonitor()");
		// if the agent was in Control mode before
		if (m_operatorMode != TA_Base_Core::Monitor)
		{
			m_operatorMode = TA_Base_Core::Monitor;
            m_pOpcServerManager->stopSynchronise();
		}
		if ( NULL != m_bulkDPPolledInterface )
		{
			// start receiving remote requests
			m_bulkDPPolledInterface->setToMonitor();
		}

		if (NULL != m_bulkDNPolledInterface)
		{
			m_bulkDNPolledInterface->setToMonitor();
		}
	}

	void OpcServerAgent::agentSetControl()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In OpcServerAgent::agentSetControl()");
		// if the agent was in Monitor mode before
		if (m_operatorMode != TA_Base_Core::Control)
		{
			m_operatorMode = TA_Base_Core::Control;            
            m_pOpcServerManager->startSynchronise();
		}
		if ( NULL != m_bulkDPPolledInterface )
		{
			// start receiving remote requests
			m_bulkDPPolledInterface->setToControl();
		}

		if (NULL != m_bulkDNPolledInterface)
		{
			m_bulkDNPolledInterface->setToControl();
		}
	}
 
	void OpcServerAgent::agentSetStandby()
	{
		if (m_operatorMode != TA_Base_Core::Standby)
		{
			m_operatorMode = TA_Base_Core::Standby;
            m_pOpcServerManager->stopSynchronise();
		}
		if ( NULL != m_bulkDPPolledInterface )
		{
			// start receiving remote requests
			m_bulkDPPolledInterface->setToStandby();
		}

		if (NULL != m_bulkDNPolledInterface)
		{
			m_bulkDNPolledInterface->setToStandby();
		}
	}

	/** 
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
	void OpcServerAgent::notifyGroupOffline( const std::string& group )
	{
		// nothing to do at this stage
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - notifyGroupOffline" );
	}

	/** 
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
	void OpcServerAgent::notifyGroupOnline( const std::string& group )
	{
		// nothing to do at this stage
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - notifyGroupOnline" );
	}

    /**
      * registerForStateUpdates
      *
      * This method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not they
      * are operating in monitor or control mode.
      */
	void OpcServerAgent::registerForStateUpdates()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - registerForStateUpdates" );
	}

    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      */
    void OpcServerAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - receiveSpecialisedMessage" );
	}

    /**
      * processOnlineUpdate
      *
      * Generic Agent will handle the receiving of online updates. However, in some
      * cases, it will be necessary to pass the updates on to the registered agent.
      * For example, if a child entity needs to be updated.
      *
      * @param The update event
      */
    void OpcServerAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - processOnlineUpdate" );
	}

	void OpcServerAgent::startOpcServerAgent()
	{
		// check the runtime parameter OperationMode
		std::string operationMode = TA_Base_Core::RunParams::getInstance().get( RPARAM_OPERATIONMODE );
		
		// create an instance of the class OpcDataNodeAgentCorbaDef_Impl and OpcDataPointAgentCorbaDef_Impl
		m_bulkDNPolledInterface = new TA_Base_Bus::OpcSimpleNodeAgentCorbaDef_Impl( m_agentName, this );
		m_bulkDPPolledInterface = new TA_Base_Bus::OpcSimplePointAgentCorbaDef_Impl( m_agentName, this );

		if ( RPARAM_CONTROL == operationMode )
		{
			agentSetControl();
		}
		else if ( RPARAM_MONITOR == operationMode )
		{
			agentSetMonitor();
		}
		else if (RPARAM_STANDBY == operationMode)
		{
			agentSetStandby();
		}

		// start the Generic Agent loop
		if ( NULL != m_genericAgent )
		{
			m_genericAgent->run();
		}
	}

	TA_Base_Bus::SimplePoint*  OpcServerAgent::getSimplePoint ( unsigned long entityKey )
	{
		return m_pOpcServerManager->getSimplePoint(entityKey);
	}

	void  OpcServerAgent::getAllSimplePoints (std::map < unsigned long, TA_Base_Bus::SimplePoint *>& dataPointList )
	{
		m_pOpcServerManager->getAllSimplePoints(dataPointList);
		return;
	}

	TA_Base_Bus::SimpleNode*  OpcServerAgent::getSimpleNode( unsigned long entityKey )
	{
		return m_pOpcServerManager->getSimpleNode(entityKey);
	}

	void  OpcServerAgent::getAllSimpleNodes (std::map < unsigned long, TA_Base_Bus::SimpleNode *>& dataNodeList )
	{
		m_pOpcServerManager->getAllSimpleNodes(dataNodeList);
		return;
	}

}
