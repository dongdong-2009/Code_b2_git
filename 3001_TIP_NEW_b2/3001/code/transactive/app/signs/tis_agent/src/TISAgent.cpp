/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TISAgent.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #11 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4503) // decorated name length exceeded
#endif


#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/TTISManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/TISAgent.h"

#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
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
using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;


using TA_Base_Bus::DataPoint;

using TA_Base_Bus::DataPointFactory;


namespace TA_IRS_App
{

	TISAgent::TISAgent(int argc, char* argv[]) 
        : m_genericAgent(NULL), 
		  m_dataPointFactory(NULL), 
		  m_dpBulkPolledInterface(NULL),
		  m_dpAccessInterface(NULL),
		  m_dnBulkPolledInterface(NULL),
		  m_dnAccessInterface(NULL),
		  m_scadaProxyFactory(NULL),
		  m_summaryProcessor(NULL)
	{
		FUNCTION_ENTRY("Constructor");

		m_tisDataPointList.clear();
		m_dataNodes.clear();

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
            }
            else
            {
                m_locationType = TA_Base_Core::ILocation::STATION;
            }
        }

        m_agentName = entityData->getName();

		TA_Base_Core::TISAgentEntityDataPtr agentData;
		
		agentData = boost::dynamic_pointer_cast<TA_Base_Core::TISAgentEntityData>(entityData);

		TA_ASSERT(agentData.get() != NULL, "Agent entity data is incorrect type");

		// Pass all the current data to the Status Monitor
		// From then on, the data is accessed via the 
		// which is updated by the StateUpdateSubscriber
		StatusMonitor::getInstance()->initialise(agentData, m_locationType);

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

//		StatusMonitor::getInstance()->setStationLibrariesSynchronised(false);

	
		
		//test end
		FUNCTION_EXIT;
	}


	//
	// Destructor
	//
	TISAgent::~TISAgent()
	{
		FUNCTION_ENTRY("Destructor");
		m_tisDataPointList.clear();
		m_dataNodes.clear();
	
		// if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		// else do nothing

		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}
		
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}

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

			if( m_dataPointFactory != NULL )
			{
				delete m_dataPointFactory;
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
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		
		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}
		
		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = NULL;

		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "WILDAgent cleanUp");

		FUNCTION_EXIT;
	}

	void TISAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();

		dataPointList = m_tisDataPointList;
	}

	//
	// agentTerminate
	//
	void TISAgent::agentTerminate()
	{
 		FUNCTION_ENTRY("agentTerminate");

        // if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		// else do nothing

		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}
		
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}


		// Stop the status request message thread
		if( StatusMonitor::getInstance()->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TISAgent asking the Status Monitor thread to terminate...");

			StatusMonitor::getInstance()->terminateAndWait();
		}

		FUNCTION_EXIT;
	}


    bool TISAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
        bool createdDnSpecificUtils = false;
        
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
            // only datanodes need these things, so if there are no datanodes, why create them?
            if ( (false == createdDnSpecificUtils) &&
                ((*iter)->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) )
            {
                // create an instance of DataNodeAccessFactory if not done already
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Start of create DataNode Hierarchy Map" );
                
                TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
                
                // initialise ScadaPersistence
                m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
                
                createdDnSpecificUtils = true;
            }
            
            createdEntities.push_back( 
                TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
        }
        
        // return true to indicate not to call createEntity
        return true;
    }
    
    
    void TISAgent::entityCreationComplete()
    {
        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }
    
    
    //
	// createEntity
	//
	IEntity* TISAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
 		FUNCTION_ENTRY("createEntity");

		try
		{
			if ( EntityData->getType() == TA_Base_Core::STISEntityData::getStaticType() )
			{
				 LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Type is STIS Entity Config");
				 				 
				 STISManager::getInstance()->initialise( EntityData );

				 return( STISManager::getInstance() );
				 
			}
			else if ( EntityData->getType() == "TTis" )
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Type is TTIS Entity Config");

				TTISManager::getInstance()->initialise( EntityData );
		
				return( TTISManager::getInstance() );
			}
			else if ( EntityData->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Type is Datapoint Entity Config");

				return createDataPointEntity ( EntityData );
			}
			else if( TA_Base_Core::DataNodeEntityData::getStaticType() == EntityData->getType() )
			{
				TA_Base_Bus::IEntity * theEntity = 0;
				TA_Base_Bus::DataNode* dn = NULL;
				try
				{
                    dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
                
					m_dataNodes.push_back( dn );
					theEntity = dn;
					return theEntity;
				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
						EntityData->getName().c_str(), EntityData->getKey() );
					
					// To ensure the generic agent is constructed even if one DataNode creation fails
				}
			}

		}
		catch ( ... )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Exception caught while creating entity data objects ");
		}
	
		FUNCTION_EXIT;
		return NULL;
	}


	//
	// agentSetMonitor
	//
	void TISAgent::agentSetMonitor()
	{
 		FUNCTION_ENTRY("agentSetMonitor");

		AgentModeMonitor::getInstance().setToMonitorMode();
		// set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

        // if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}
		if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setToMonitor();
		}
		
        if (NULL != m_dnAccessInterface)
        {
			m_dnAccessInterface->setToMonitor();
		}

		// CL16505++
		m_dpReqProcessor.clearWriteQueue();
        m_dpReqProcessor.terminateAndWait();
		//++CL16505

		// Set this to Monitor before we active the servant - this owns some datapoints
		// that also must be set to Monitor
        // Don't have to explicitly set it to monitor  Mode as GenericAgent will do so.
	//	STISManager::getInstance()->setToMonitorMode();

		// Dont want the servants to service clients while in Monitor mode
	//	STISManager::getInstance()->deactivateServant();
	//	TTISManager::getInstance()->deactivateServant();

	//	StatusMonitor::getInstance()->terminate();

		FUNCTION_EXIT;
	}


	//
	// agentSetControl
	//
	void TISAgent::agentSetControl()
	{
 		FUNCTION_ENTRY("agentSetControl");

		AgentModeMonitor::getInstance().setToControlMode();
		// set all datanodes to control mode
		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
		}
		// set the proxies to control mode
		m_scadaProxyFactory->setProxiesToControlMode();

        // if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
		{
			// start receiving remote requests
			m_dpAccessInterface->setToControl();
		}
		if ( NULL != m_dnBulkPolledInterface )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToControl();
        }

		//CL16505++
		m_dpReqProcessor.start();
		// ++CL16505

		//StatusMonitor::getInstance()->start();

		// Set this to control before we active the servant - this owns some datapoints
		// that also must be set to Control
	//	STISManager::getInstance()->setToControlMode();

	//	STISManager::getInstance()->activateServant();
	//	TTISManager::getInstance()->activateServant();

		FUNCTION_EXIT;
	}

	//
	// notifyGroupOffline
	//
	void TISAgent::notifyGroupOffline( const std::string& group )
	{
	}

	//
	// notifyGroupOnline
	//
	void TISAgent::notifyGroupOnline( const std::string& group )
	{
 		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT;
	}

	//
	// registerForControlMessages
	//
	void TISAgent::registerForControlMessages()
	{
	}


	//
	// registerForMonitorMessages
	//
	void TISAgent::registerForMonitorMessages()
	{
	}


	//
	// registerForStateUpdates
	//
	void TISAgent::registerForStateUpdates()
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
	void TISAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
	{
	}


	//
	// processOnlineUpdate
	//
	void TISAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
	{
 		FUNCTION_ENTRY("processOnlineUpdate");
		FUNCTION_EXIT;
	}

	IEntity* TISAgent::createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData )
	{
		DataPoint* theEntity = 0;
        TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( EntityData );

		// Do I need to perform some sort of address check here?
		// Shouldn't need to 
		theEntity = m_dataPointFactory->createDataPoint( dataPointEntityData );
		if ( 0 != theEntity )
		{
			m_tisDataPointList[ theEntity->getEntityKey() ] = theEntity;
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Created datapoint:%s", theEntity->getDataPointName().c_str());
			StatusMonitor::getInstance()->addDataPoint(theEntity);
		}

		return theEntity;
	}

	//
	// run
	//
	void TISAgent::run()
	{
 		FUNCTION_ENTRY("run");

		TA_ASSERT( m_genericAgent != NULL, "GenericAgent has not been initialised" );


		// Perform OCC-specific initialization
        if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Agent started at OCC, checking state of predefined message libraries...");

			unsigned long currentCDBStationLibrary = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
			unsigned long nextCDBStationLibrary = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
			bool stationLibrarySynchronised = StatusMonitor::getInstance()->getStationLibrariesSynchronised();

			unsigned long currentCDBTrainLibrary = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
			unsigned long nextCDBTrainLibrary = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
			bool trainLibrarySynchronised = StatusMonitor::getInstance()->getTrainLibrariesSynchronised();

			// Either of the following situations could only occur if the predefined message library 
			// has been retrieved, parsed and stored to the database but the 
			// synchronisation of stations did not complete
			// start the synchronisation thread if this is the case

			if( ( currentCDBStationLibrary != nextCDBStationLibrary ) && ( !stationLibrarySynchronised ) )
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Station libraries not synchronised, starting synchronisation thread..");
				//STISManager::getInstance()->synchroniseStationLibrary();
			}
			else
			{

				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Station libraries synchronised");
			}

			if( ( currentCDBTrainLibrary != nextCDBTrainLibrary ) && ( !trainLibrarySynchronised ) )
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Train libraries not synchronised, starting synchronisation thread..");
				// if current and new versions are different and !synchronisation complete
				//STISManager::getInstance()->synchroniseTrainLibrary();
			}
			else
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Train libraries synchronised");
			}
		}
		else // Must be a station
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Agent started at station, checking state of predefined message libraries...");

			try
			{
                // resolve the OCC TIS agent
                TISAgentAccessFactory::getInstance().getOccSTISAgent();
			}
            catch ( TA_Base_Core::DatabaseException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Caught an exception while attempting to retrieve OCC TISAgent entity data");
			}
            catch ( TA_Base_Core::DataException& de )
			{
                switch (de.getFailType())
                {
                case TA_Base_Core::DataException::NO_VALUE:
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				        "No Entities of type 'Stis' found for OCC - cannot establish predefined message library information");
                    break;
                case TA_Base_Core::DataException::NOT_UNIQUE:
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				        "Multiple Entities of type 'Stis' found for OCC - cannot establish predefined message library information");
                    break;
                default:
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					    "Caught an exception while attempting to retrieve OCC TISAgent entity data");
                    break;
                }
			}
            catch ( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Caught an exception while attempting to retrieve OCC TISAgent entity data");
			}
			

			try
			{

				unsigned long currentLocalStationLibrary = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
				unsigned long nextLocalStationLibrary = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();

				unsigned long currentLocalTrainLibrary = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
				unsigned long nextLocalTrainLibrary = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

				unsigned long currentCDBStationLibraryVersion;
                CORBA_CALL_RETURN( currentCDBStationLibraryVersion, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBSTISMessageLibraryVersion, () )//limin
				unsigned long nextCDBStationLibraryVersion;
                CORBA_CALL_RETURN( nextCDBStationLibraryVersion, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBSTISMessageLibraryVersion, () );//limin
				bool occStationLibrarySynchronised;
                CORBA_CALL_RETURN( occStationLibrarySynchronised, TISAgentAccessFactory::getInstance().getOccSTISAgent(), isStationLibrarySynchronisationComplete, () )//limin

				unsigned long currentCDBTrainLibraryVersion;
                CORBA_CALL_RETURN( currentCDBTrainLibraryVersion, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBTTISMessageLibraryVersion, () )//limin
				unsigned long nextCDBTrainLibraryVersion;
                CORBA_CALL_RETURN( nextCDBTrainLibraryVersion, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBTTISMessageLibraryVersion, () )//limin
				bool occTrainLibrarySynchronised;
                CORBA_CALL_RETURN( occTrainLibrarySynchronised, TISAgentAccessFactory::getInstance().getOccSTISAgent(), isTrainLibrarySynchronisationComplete, () )//limin


                // Have a look at the Station library version info
			    if(  nextLocalStationLibrary != nextCDBStationLibraryVersion )
				{
				    if( true == occStationLibrarySynchronised ) // Local DB already has the library
				    {	
					    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						    "The CDB has a different next Station library version and is synchronised - setting next Station library version to the same");

					    StatusMonitor::getInstance()->setNextISCSStationLibraryVersion( nextCDBStationLibraryVersion );
				    }
					else // Check DB & synchronise
					{
						bool haveLibrary = STISManager::getInstance()->reportStationLibraryVersionReceived( nextCDBStationLibraryVersion );
						if ( false == haveLibrary )
						{
							TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var stisFile;
							CORBA_CALL_RETURN( stisFile, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getSTISLibraryDBContent, (nextCDBStationLibraryVersion));
							STISManager::getInstance()->saveSTISLibraryToLocalDB(nextCDBStationLibraryVersion, stisFile);
						}
					}
				}
			    if( ( currentCDBStationLibraryVersion == nextCDBStationLibraryVersion ) &&
					     ( currentLocalStationLibrary != currentCDBStationLibraryVersion ) )
			    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					    "The OCC has a different Station library version and has been upgraded - checking local database for new library version...");

				    // This station has missed out on an upgrade - check that we have the new predefined
				    // library in this station database, and then force an upgrade
				    bool haveLibrary = STISManager::getInstance()->reportStationLibraryVersionReceived( currentCDBStationLibraryVersion );

				    if( haveLibrary )
				    {
					    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						    "This station has the new Station library version - performing forced upgrade to new version");

					    StatusMonitor::getInstance()->setNextISCSStationLibraryVersion( currentCDBStationLibraryVersion );
					    StatusMonitor::getInstance()->setCurrentISCSStationLibraryVersion( currentCDBStationLibraryVersion );
				    }
			    }

			    // Have a look at the Train library version info
			    if(  nextLocalTrainLibrary != nextCDBTrainLibraryVersion )
				{
				    if( true == occTrainLibrarySynchronised )
				    {	
					    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						    "The OCC has a different next Train library version and is synchronised - setting next Train library version to the same");

					    StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion( nextCDBTrainLibraryVersion );
				    }
					else // Check DB & synchronise
					{
						bool haveLibrary = STISManager::getInstance()->reportTrainLibraryVersionReceived( nextCDBTrainLibraryVersion );
						if ( false == haveLibrary )
						{
							TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var ttisFile;
							CORBA_CALL_RETURN( ttisFile, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getTTISLibraryDBContent, (nextCDBTrainLibraryVersion));							
							STISManager::getInstance()->saveTTISLibraryToLocalDB(nextCDBTrainLibraryVersion, ttisFile);
						}
					}
				}
			    if( ( currentCDBTrainLibraryVersion == nextCDBTrainLibraryVersion ) &&
					     ( currentLocalTrainLibrary != currentCDBTrainLibraryVersion ) )
			    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					    "The OCC has a different Train library version and has been upgraded - checking local database for new library version...");

				    // This station has missed out on an upgrade - check that we have the new predefined
				    // library in this station database, and then force an upgrade
				    bool haveLibrary = STISManager::getInstance()->reportTrainLibraryVersionReceived( currentCDBTrainLibraryVersion );

				    if( haveLibrary )
				    {
					    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						    "This station has the new Train library version - performing forced upgrade to new version");

					    StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion( currentCDBTrainLibraryVersion );
					    StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( currentCDBTrainLibraryVersion );
				    }
			    }
			}
			catch ( const TA_Base_Core::DatabaseErrorException& dbe )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseErrorException", "");
			}
			catch ( const TA_Base_Core::ObjectResolutionException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Could not resolve the Occ TISAgent while attempting to check the predefined message library versions");
			}
			catch ( const CORBA::Exception& ce )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Caught a CORBA exception while attempting to check the predefined message library versions at the OCC, reason:%s", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ).c_str());
			}
		}

		TA_Base_Core::EOperationMode operationMode = m_genericAgent->getOperationMode();

		if( operationMode == TA_Base_Core::Control )
		{
			STISManager::getInstance()->setToControlMode();	
			TTISManager::getInstance()->setToControlMode();
		}
		else
		{
			STISManager::getInstance()->setToMonitorMode();
			TTISManager::getInstance()->setToMonitorMode();
		}

		StatusMonitor::getInstance()->start();

		// if bulk interface is setup

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
		
		TA_ASSERT(m_dpBulkPolledInterface != NULL, "can't create bulk poll interface.");
		if( operationMode == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}

		m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_agentName, this );
		
		TA_ASSERT(m_dpAccessInterface != NULL, "can't create datapoint access interface.");
		if( operationMode == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dpAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}

		m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
		TA_ASSERT(m_dnBulkPolledInterface != NULL, "can't create m_dnBulkPolledInterface");
		if( operationMode == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToMonitor();
		}
		// else do nothing
		m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
		TA_ASSERT(m_dnAccessInterface != NULL, "can't create m_dnAccessInterface");
		if( operationMode == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dnAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dnAccessInterface->setToMonitor();
		}

		//CL16505++
		if( operationMode == TA_Base_Core::Control )
		{
			m_dpReqProcessor.start();
		}
		//++CL16505

		if( operationMode == TA_Base_Core::Control )
		{
			// add by hongzhi
			AgentModeMonitor::getInstance().setToControlMode();
		}
		else
		{
			// change internal flag of operation mode
			AgentModeMonitor::getInstance().setToMonitorMode();
		}

        //limin++, inspectorpanel exception
        if( operationMode == TA_Base_Core::Control )
        {
            for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
            {
                (*itr)->setOperationMode( true );
            }
        }
        else
        {
            for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
            {
                (*itr)->setOperationMode( false );
            }
        }
        //limin++, inspectorpanel exception

	

        // Start GenericAgent

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting GenericAgent" );		

        m_genericAgent->run();

		FUNCTION_EXIT;
	}

	std::string TISAgent::getEquipmentName( const std::string& fullName )
	{
		std::string equipmentName;

		std::string::size_type position1 = fullName.find(".");

		if(position1 != std::string::npos)
		{
			std::string::size_type position2 = fullName.find(".",position1+1);

			if(position2 != std::string::npos )
			{
				std::string::size_type position3 = fullName.find(".",position2+1);

				if(position3 != std::string::npos )
				{
					std::string::size_type position4 = fullName.find(".",position3+1);

					equipmentName = fullName.substr(position3+1,(position4-(position3+1)));
				}
			}

		}
		return equipmentName;
	}

	std::string TISAgent::getShortDataPointName( const std::string& fullName )
	{
		std::string shortDataPointName;

		std::string::size_type position1 = fullName.find(".");

		if(position1 != std::string::npos)
		{
			std::string::size_type position2 = fullName.find(".",position1+1);

			if(position2 != std::string::npos )
			{
				std::string::size_type position3 = fullName.find(".",position2+1);

				if(position3 != std::string::npos )
				{
					std::string::size_type position4 = fullName.find(".",position3+1);

					shortDataPointName = fullName.substr(position4+1,fullName.size());
				}
			}

		}

		return shortDataPointName;
	}


	TA_Base_Bus::DataPoint* TISAgent::getDataPoint(unsigned long entityKey)
	{
		TA_Base_Bus::DataPoint* theDataPoint = NULL;

		try
		{
			// Find datapoint
			const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
			std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it = entityMap->find(entityKey);

			if (it != entityMap->end())
			{		
				theDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>((*it).second);
			}
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getDataPoint() - Exception thrown while retrieving datapoint");
		}

		return theDataPoint;
	}
    
    TA_Base_Bus::DataPointFactory* TISAgent::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }
    
	TA_Base_Bus::DataNode * TISAgent::getDataNode( unsigned long entityKey )
	{
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
		
		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
			
            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }
		
		return dn;
	}

	void TISAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }

	void TISAgent::checkOperationMode()
	{
	}

	void TISAgent::beforeGetDescendantsOfAgent( TA_Base_Core::IEntityDataPtr agentEntity )
	{
		if ( m_dataPointFactory == NULL )
		{
			m_dataPointFactory = new DataPointFactory(this);
		}

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		// create handle to DataPointFactory

	}

}  // TA_App

