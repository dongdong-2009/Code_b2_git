/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSAgent.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4503) // decorated name length exceeded
#endif


#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/AgentModeMonitor.h"
#include "app/signs/pids_agent/src/PIDSAgent.h"

#include "bus/scada/datapoint_library/src/DataPointFactory.h"
//#include "bus/signs_4669/Pids_agent_access/src/PIDSAgentAccessFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
//#include "bus/scada/datanode_library/src/DataNode.h"
//#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
//#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
//#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
//#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
//#include "core/data_access_interface/entity_access/src/PIDSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"



using TA_Base_Bus::IEntity;


using TA_Base_Core::IEntityData;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
//using TA_Base_Bus::PIDSAgentAccessFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;


using TA_Base_Bus::DataPoint;

using TA_Base_Bus::DataPointFactory;


namespace TA_IRS_App
{

	PIDSAgent::PIDSAgent(int argc, char* argv[]) 
        : m_genericAgent(NULL)
	{
		FUNCTION_ENTRY("Constructor");

		m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);

		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
    		
		unsigned long key = entityData->getKey();
		unsigned long typeKey = entityData->getTypeKey();

		char buff1[50] = {0};            
		char buff2[50] = {0};            
		sprintf(buff1,"%lu",key);
		sprintf(buff2,"%lu",typeKey);
		RunParams::getInstance().set("entityKey", buff1);
		RunParams::getInstance().set("entityTypeKey", buff2);

		m_location = entityData->getLocationName();

        try
        {
            m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_location);
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Error retrieving location type from database");

            // do a name based check
            if (m_location == "OCC")
            {
                m_locationType = TA_Base_Core::ILocation::OCC;
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"Agent start at Occ");
            }
            else
            {
                m_locationType = TA_Base_Core::ILocation::STATION;
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
					"Agent start at Station, invalid for PidsAgent");
            }
        }

        m_agentName = entityData->getName();
		PIDSCommunicationsHandler::getInstance()->initialise(entityData->getLocation());

		PIDSManager::getInstance()->setEntityName(m_agentName);

		//PIDSManager::getInstance()->subscribeAtsPidUpdateMsg(entityData->getLocation());
//		TA_Base_Core::PIDSAgentEntityDataPtr agentData;
		
//		agentData = boost::dynamic_pointer_cast<TA_Base_Core::PIDSAgentEntityData>(entityData);

//		TA_ASSERT(agentData.get() != NULL, "Agent entity data is incorrect type");

		// Pass all the current data to the Status Monitor
		// From then on, the data is accessed via the 
		// which is updated by the StateUpdateSubscriber
//		StatusMonitor::getInstance()->initialise(agentData, m_locationType);

		FUNCTION_EXIT;
	}


	//
	// Destructor
	//
	PIDSAgent::~PIDSAgent()
	{
		FUNCTION_ENTRY("Destructor");
		try
        {
             // Stop the subscriber from receiving messages.
//            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( &m_stateUpdateSubscriber );

            // Deactivate and delete the Generic Agent.
            if( m_genericAgent != NULL )
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = 0;
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }
		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "WILDAgent cleanUp");

		FUNCTION_EXIT;
	}

	//
	// agentTerminate
	//
	void PIDSAgent::agentTerminate()
	{
 		FUNCTION_ENTRY("agentTerminate");

		// Stop the status request message thread
/*		if( StatusMonitor::getInstance()->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PIDSAgent asking the Status Monitor thread to terminate...");

			StatusMonitor::getInstance()->terminateAndWait();
		}
*/
		FUNCTION_EXIT;
	}


    bool PIDSAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
            createdEntities.push_back( 
                TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
        }
        
        // return true to indicate not to call createEntity
        return true;
    }
    
    
    void PIDSAgent::entityCreationComplete()
    {
        // loading complete, free some memory
//		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }
    
    
    //
	// createEntity
	//
	IEntity* PIDSAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
 		FUNCTION_ENTRY("createEntity");
	
		FUNCTION_EXIT;
		return NULL;
	}


	//
	// agentSetMonitor
	//
	void PIDSAgent::agentSetMonitor()
	{
 		FUNCTION_ENTRY("agentSetMonitor");

//		AgentModeMonitor::getInstance().setToMonitorMode();
		// Set this to Monitor before we active the servant - this owns some datapoints
		// that also must be set to Monitor
        // Don't have to explicitly set it to monitor  Mode as GenericAgent will do so.
		PIDSManager::getInstance()->setToMonitorMode();

		// Dont want the servants to service clients while in Monitor mode
	//	PIDSManager::getInstance()->deactivateServant();
	//	TPIDSManager::getInstance()->deactivateServant();

	//	StatusMonitor::getInstance()->terminate();

		FUNCTION_EXIT;
	}


	//
	// agentSetControl
	//
	void PIDSAgent::agentSetControl()
	{
 		FUNCTION_ENTRY("agentSetControl");

//		AgentModeMonitor::getInstance().setToControlMode();
		//StatusMonitor::getInstance()->start();

		// Set this to control before we active the servant - this owns some datapoints
		// that also must be set to Control
		PIDSManager::getInstance()->setToControlMode();

	//	PIDSManager::getInstance()->activateServant();
	//	TPIDSManager::getInstance()->activateServant();

		FUNCTION_EXIT;
	}

	//
	// notifyGroupOffline
	//
	void PIDSAgent::notifyGroupOffline( const std::string& group )
	{
 		FUNCTION_ENTRY("notifyGroupOffline");
		
		FUNCTION_EXIT;
	}

	//
	// notifyGroupOnline
	//
	void PIDSAgent::notifyGroupOnline( const std::string& group )
	{
 		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT;
	}

	//
	// registerForControlMessages
	//
	void PIDSAgent::registerForControlMessages()
	{
 		FUNCTION_ENTRY("registerForControlMessages");
		FUNCTION_EXIT;
	}


	//
	// registerForMonitorMessages
	//
	void PIDSAgent::registerForMonitorMessages()
	{
 		FUNCTION_ENTRY("registerForMonitorMessages");
 		FUNCTION_EXIT;
	}


	//
	// registerForStateUpdates
	//
	void PIDSAgent::registerForStateUpdates()
	{
 		FUNCTION_ENTRY("registerForStateUpdates");

		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare("Control") )
		{
			registerForControlMessages();
		}
		else
		{
			registerForMonitorMessages();
		}

 		FUNCTION_EXIT;
	}


	//
	// receiveSpecialisedMessage
	//
	void PIDSAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
	{
 		FUNCTION_ENTRY("receiveSpecialisedMessage");

		FUNCTION_EXIT;
	}


	//
	// processOnlineUpdate
	//
	void PIDSAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
	{
 		FUNCTION_ENTRY("processOnlineUpdate");
		FUNCTION_EXIT;
	}

	//
	// run
	//
	void PIDSAgent::run()
	{
 		FUNCTION_ENTRY("run");

		TA_ASSERT( m_genericAgent != NULL, "GenericAgent has not been initialised" );

		// Perform OCC-specific initialization
        if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Agent started at OCC, checking state of predefined message libraries...");

			// Either of the following situations could only occur if the predefined message library 
			// has been retrieved, parsed and stored to the database but the 
			// synchronisation of stations did not complete
			// start the synchronisation thread if this is the case
		}
		else // Must be a station
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Agent started at station, checking state of predefined message libraries...");

			try
			{
                // resolve the OCC PIDS agent
//                PIDSAgentAccessFactory::getInstance().getOccPIDSAgent();
			}
            catch ( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Caught an exception while attempting to retrieve OCC PIDSAgent entity data");
			}
		}

		TA_Base_Core::EOperationMode operationMode = m_genericAgent->getOperationMode();

		if( operationMode == TA_Base_Core::Control )
		{
			PIDSManager::getInstance()->setToControlMode();	
		}
		else
		{
			PIDSManager::getInstance()->setToMonitorMode();
		}

//		StatusMonitor::getInstance()->start();

//		if( operationMode == TA_Base_Core::Control )
		{
			// add by hongzhi
//			AgentModeMonitor::getInstance().setToControlMode();
		}
//		else
		{
			// change internal flag of operation mode
//			AgentModeMonitor::getInstance().setToMonitorMode();
		}

        // Start GenericAgent

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting GenericAgent" );

        m_genericAgent->run();

		FUNCTION_EXIT;
	}

}  // TA_App

