/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentFramework.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioServerAgentFramework.h"
#include "app/radio/RadioTetraAgent/src/RadioSDS.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/CommonTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"

#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using namespace TA_IRS_App;
using namespace TA_IRS_Bus;

namespace
{
    static const std::string RADIO_SERVER_COMMAND_ENTITY_NAME = "RadioServerCommand";
    static const std::string RADIO_SERVER_SDS_ENTITY_NAME = "RadioServerSDS";
	static const std::string RADIO_FALLBACK_DATAPOINT_POSTFIX = ".SYS.ATS.ATSRCS.aiiRadio-NormalFallBack";
}

RadioServerAgentFramework::RadioServerAgentFramework(int argc, char *argv[])
: //	m_radioServerAgentSummary(NULL),
	m_radioServerAgent(NULL),
    m_radioCommandServant(NULL),
	m_pGenericAgent(NULL),
	m_radioSDSSummary(NULL),
	m_dataPointFactory(NULL),
	m_scadaProxyFactory(NULL),
	m_summaryProcessor(NULL),
	m_dpBulkPolledInterface(NULL),
	m_dpAccessInterface(NULL),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_radioFallbackDp(NULL),
	m_radioSDSLinkStatus(NULL)
{
    FUNCTION_ENTRY("Constructor");

	if ( NULL == m_dataPointFactory )
	{
		m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
	}

	m_dataPointList.clear();
	m_dataNodes.clear();

	m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

    // must create the agent first to be used by createEntity()
    m_radioServerAgent = new RadioServerAgent();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "RadioServerAgent constructed");
	//This must be the first step to get the configuration data;
    m_pGenericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this );
	TA_Base_Core::IEntityDataPtr entityData = m_pGenericAgent->getAgentEntityConfiguration();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "GenericAgent constructed");
	m_entityName = entityData->getName();

	// setup corba call to train agent
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Initialize TrainAgentProxy");
	TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise(entityData);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "TrainAgentProxy initialized");

	m_scadaProxyFactory->registerLocalEntities(m_entityName, m_pGenericAgent->getEntityMap());

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

	m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
	m_summaryProcessor->setEntityMap(m_pGenericAgent->getEntityMap());

	//For Radio SDS Server link Status Datapoint
	m_radioSDSLinkStatus = new RadioSDSLinkStatusThread();

	// setup radio gateway server address
	unsigned long key = entityData->getKey();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Agent Key %ld",key);
	TA_Base_Core::EntityHelper help( key,"Radio");
	std::string strServerAddress=help.getParameter("PrimaryServers");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Address pair %s",strServerAddress.c_str());
	std::string strTimeout = help.getParameter("ServerTimeout");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ServerTimeout %s",strTimeout.c_str());
	int cormaPos = strServerAddress.find_first_of(',');
	
	TA_ASSERT(cormaPos != std::string::npos, "Incorrect IP address pair format.");
	std::string primaryAddress,secondaryAddress;
	primaryAddress = strServerAddress.substr(0,cormaPos);
	secondaryAddress = strServerAddress.substr(cormaPos+1,strServerAddress.size() - cormaPos -1);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "primaryAddress %s,secondaryAddress %s",primaryAddress.c_str() ,secondaryAddress.c_str());
	unsigned long timeout = atol(strTimeout.c_str() );
	if (NULL != m_radioCommandServant) {
		m_radioCommandServant->setCommandSender(primaryAddress, secondaryAddress,timeout,m_radioFallbackDp);
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Command Servant not initialized");
	}

	// setup radio system MCC address, for occ
	std::string ior, username, password;
	try {
		TA_Base_Core::EntityHelper mccHelp(key,"Radio");
		ior = mccHelp.getParameter("RadioResourceSyncIOR");
		username = mccHelp.getParameter("RadioResourceSyncUsername");
		password = mccHelp.getParameter("RadioResourceSyncPassword");
	} catch (...) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to get detail for radio connection.");
		FUNCTION_EXIT;
		return;
	}
	if (NULL != m_radioCommandServant) {
		m_radioCommandServant->setMccHandler(username,password,ior);
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Command Servant not initialized");
	}
	
	//For PDSServer
	try
	{
		std::string UdpServer = help.getParameter("UdpServer");
		std::string UdpServicePort = help.getParameter("UdpServicePort");
		CommonTypes::SRadioPDSTimer sPDSTimer;
		sPDSTimer.lTimer5 = atol( help.getParameter("Tis5Timer").c_str() );
		sPDSTimer.lTimer7 = atol( help.getParameter("Tis7Timer").c_str() );

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"UdpServer : %s, UdpServicePort: %s, Tis5Timer : %d, Tis7Timer : %d",UdpServer.c_str(),
		 UdpServicePort.c_str(),sPDSTimer.lTimer5,sPDSTimer.lTimer7);
		
		if (NULL != m_radioCommandServant) {
			m_radioCommandServant->setCommandPDSServer(UdpServer,UdpServicePort,sPDSTimer);
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Command Servant not initialized");
		}
	}
	catch (...) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get detailed information from the database for UDPServer connection.");
		FUNCTION_EXIT;
	
	}
	FUNCTION_EXIT;
}


RadioServerAgentFramework::~RadioServerAgentFramework()
{
	FUNCTION_ENTRY("Destructor");
	m_dataPointList.clear();
	m_dataNodes.clear();

	if ( NULL != m_dataPointFactory)
	{
		delete m_dataPointFactory;
		m_dataPointFactory = NULL;
	}

	if ( NULL != m_dpBulkPolledInterface)
	{
		delete m_dpBulkPolledInterface;
		m_dpBulkPolledInterface = NULL;
	}

	if ( NULL != m_dpAccessInterface)
	{
		delete m_dpAccessInterface;
		m_dpAccessInterface = NULL;
	}

	if ( NULL != m_dnBulkPolledInterface)
	{
		delete m_dnBulkPolledInterface;
		m_dnBulkPolledInterface = NULL;
	}

	if ( NULL != m_dnAccessInterface)
	{
		delete m_dnAccessInterface;
		m_dnAccessInterface = NULL;
	}

	if( m_pGenericAgent != NULL )
	{
		m_pGenericAgent->deactivateAndDeleteServant();
		m_pGenericAgent = 0;
	}

	if ( NULL != m_summaryProcessor )
	{
		TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
		m_summaryProcessor = NULL;
	}

	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_scadaProxyFactory = NULL;

	if( m_radioServerAgent != NULL )
	{
		delete m_radioServerAgent;
		m_radioServerAgent = NULL;
	}

	//For Radio SDS Server Link Status Datapoint
	if ( NULL != m_radioSDSLinkStatus )
	{
		delete m_radioSDSLinkStatus;
		m_radioSDSLinkStatus = NULL;
	}

    //if( m_radioServerAgentSummary != NULL )
    //{
    //    delete m_radioServerAgentSummary;
    //    m_radioServerAgentSummary = NULL;
    //}

	delete m_radioSDSSummary;
	m_radioSDSSummary = NULL;


	FUNCTION_EXIT;
}


bool RadioServerAgentFramework::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
										const TA_Base_Core::SharedIEntityDataList& entityDataList,
										TA_Base_Bus::IEntityList& createdEntities)
{
	bool createdDnSpecificUtils = false;

	for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
		iter != entityDataList.end(); ++iter )
	{
		if ( (false == createdDnSpecificUtils) &&
			((*iter)->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) )
		{
			// create an instance of DataNodeAccessFactory if not done already
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Start of create DataNode Hierarchy Map" );

			TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);

			// initialise ScadaPersistence
			m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );

			createdDnSpecificUtils = true;
		}

		TA_Base_Bus::IEntity* entity = createEntity(*iter);
		if (NULL != entity) {
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type((*iter)->getKey(), entity));
		}		
	}

	// return true to indicate not to call createEntity
	return true;
}
    



TA_Base_Bus::IEntity* RadioServerAgentFramework::createEntity(IEntityDataPtr entityData)
{
	FUNCTION_ENTRY("createEntity");

    std::string typ = entityData->getType();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create entity called for : %s",typ.c_str());
    if (typ == RADIO_SERVER_COMMAND_ENTITY_NAME)
    {
//		m_radioServerAgentSummary = new RadioServerAgentSummary(entityData);

        m_radioCommandServant = new RadioServerAgentCommandServant(entityData, this);
        return m_radioCommandServant;

    }
    else if (typ == RADIO_SERVER_SDS_ENTITY_NAME)
    {
        //RadioSDS* sds = new RadioSDS(EntityData);
        //m_radioSDS.push_back(sds);
		m_radioSDSSummary = new RadioSDSSummary(entityData);

        return NULL;
    }
	else if (TA_Base_Core::DataPointEntityData::getStaticType() == typ)
	{
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = 
			boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( entityData );

		TA_Base_Bus::DataPoint* dataPoint = m_dataPointFactory->createDataPoint ( dataPointEntityData );

		if (dataPoint == NULL)
		{
			std::ostringstream msg;
			msg << "Child entity datapoint " << dataPointEntityData->getName() 
				<< " has not configured correctly and cannot be created";  

			TA_THROW(TA_Base_Core::GenericAgentException(
				TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
				msg.str().c_str()));
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "%s",
				"Child entity datapoint %s has been created",
				dataPointEntityData->getName().c_str());
			m_dataPointList[dataPointEntityData->getKey()] = dataPoint;
		}

		if (!dataPointEntityData->getAllowWrite())
		{
			// Initialise the datapoint to not connected status
			TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();

			TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
			dataPoint->updateFieldState( newState );
		}

		if (dataPointEntityData->getName().find(RADIO_FALLBACK_DATAPOINT_POSTFIX) != std::string::npos) {
			m_radioFallbackDp = dataPoint;
		}

		return dataPoint;
	}
	else if(TA_Base_Core::DataNodeEntityData::getStaticType() == typ)
	{
		TA_Base_Bus::DataNode* dataNode = NULL;
		try
		{
			dataNode = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(entityData), m_persistence);

			m_dataNodes.push_back( dataNode );
			return dataNode;
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
				entityData->getName().c_str(), entityData->getKey() );
		}
	}

	FUNCTION_EXIT;
    return NULL;
}


void RadioServerAgentFramework::agentSetMonitor()
{
	FUNCTION_ENTRY("agentSetMonitor");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "agentSetMonitor");
	m_radioServerAgent->onMonitorMode();

	if ( NULL != m_dpBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dpBulkPolledInterface->setToMonitor();
	}
	// if dp Access Interface interface is set up
	if ( NULL != m_dpBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dpAccessInterface->setToMonitor();
	}
	// if bulk interface is set up
	if ( NULL != m_dnBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dnBulkPolledInterface->setToMonitor();
	}
	// if dn Access Interface interface is set up
	if ( NULL != m_dnBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dnAccessInterface->setToMonitor();
	}

	for ( DatapointList::iterator it = m_dataPointList.begin(); it != m_dataPointList.end(); ++it )
	{
		it->second->setToMonitorMode();
	}

	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		(*itr)->setOperationMode( false );
	}

	m_scadaProxyFactory->setProxiesToMonitorMode();
	m_dataPointFactory->setToMonitor();

	FUNCTION_EXIT;
}

void RadioServerAgentFramework::agentSetControl()
{
    FUNCTION_ENTRY("agentSetControl");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "agentSetControl");
    m_radioServerAgent->onControlMode();

	for ( DatapointList::iterator it = m_dataPointList.begin(); it != m_dataPointList.end(); ++it )
	{
		it->second->setToControlMode();
	}

	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		(*itr)->setOperationMode(true);
	}

	m_scadaProxyFactory->setProxiesToControlMode();
	m_dataPointFactory->setToControl();

	if ( NULL != m_dpBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dpBulkPolledInterface->setToControl();
	}
	// if dp Access Interface interface is set up
	if ( NULL != m_dpBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dpAccessInterface->setToControl();
	}
	// if bulk interface is set up
	if ( NULL != m_dnBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dnBulkPolledInterface->setToControl();
	}
	// if dn Access Interface interface is set up
	if ( NULL != m_dnBulkPolledInterface )
	{
		// stop receiving remote requests
		m_dnAccessInterface->setToControl();
	}

	
   
    FUNCTION_EXIT;
}

void RadioServerAgentFramework::agentTerminate()
{
	FUNCTION_ENTRY("agentTerminate");

	m_radioServerAgent->onTerminate();

	FUNCTION_EXIT;
}



void RadioServerAgentFramework::notifyGroupOffline(const std::string& group)
{
	FUNCTION_ENTRY("notifyGroupOffline");

	//Actually do nothing;
	if ( "OCC" == group )
	{	
		if(m_radioCommandServant == NULL) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Command Servant entity not defined");
			return;
		}


		 
		m_radioCommandServant->notifyMftAgent(ISCS_DEGRADED_MODE);
	}

	FUNCTION_EXIT;
}

void RadioServerAgentFramework::notifyGroupOnline(const std::string& group)
{
	FUNCTION_ENTRY("notifyGroupOnline");

	//Actually do nothing;

	if ( "OCC" == group )
	{	

		if(m_radioCommandServant == NULL) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Command Servant entity not defined");
			return;
		}
		m_radioCommandServant->notifyMftAgent(ISCS_NORMAL_MODE);
	 
	}
	FUNCTION_EXIT;
}

void RadioServerAgentFramework::registerForStateUpdates()
{
	FUNCTION_ENTRY("registerForStateUpdates");

	//Actually do nothing;

	FUNCTION_EXIT;
}

void RadioServerAgentFramework::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
	FUNCTION_ENTRY("receiveSpecialisedMessage");

	//Actually do nothing;

	FUNCTION_EXIT;
}

void RadioServerAgentFramework::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
{
	FUNCTION_ENTRY("processOnlineUpdate");

	//Actually do nothing;

	FUNCTION_EXIT;
}


void RadioServerAgentFramework::run()
{
    FUNCTION_ENTRY("run");
    
    TA_ASSERT(m_radioCommandServant != NULL, "Radio Command Servant entity not defined");

    m_radioServerAgent->setCommandServant(m_radioCommandServant);
//    m_radioCommandServant->setSummary(*m_radioServerAgentSummary);
    m_radioCommandServant->setRadioSDS(m_radioSDSSummary);

	m_radioSDSLinkStatus->setDataPointConfig(m_dataPointList,m_radioSDSSummary);

	m_radioCommandServant->setSDSLinkStatusDp(m_radioSDSLinkStatus);
    
    m_radioCommandServant->start();
    
	//Do not set the m_operationMode member here! It will be set in following call
    //to SetMonitor or SetControl
    //Note it is initialized to TA_Base_Core::NotApplicable in CTOR to drive init
    TA_Base_Core::EOperationMode operationMode = m_pGenericAgent->getOperationMode();
    
    switch(operationMode)
    {
    case TA_Base_Core::Control :
        {
            agentSetControl();
            break;
        }
    case TA_Base_Core::Monitor :
        {
            agentSetMonitor();
            break;
        }
    default:
        {
            agentSetMonitor();
        }
    } //end switch

	// if bulk interface is setup
	// create an instance of the class DataPointAgentCorbaDef_Impl
	m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_entityName, this );
	TA_ASSERT(m_dpBulkPolledInterface, "can't create the dp bulk interface.");
	if ( operationMode == TA_Base_Core::Control )
	{
		// start receiving remote requests
		m_dpBulkPolledInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dpBulkPolledInterface->setToMonitor();
	}

	m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_entityName, this );
	TA_ASSERT(m_dpAccessInterface, "can't create the datapoint access interface.");
	if ( operationMode == TA_Base_Core::Control )
	{
		// start receiving remote requests
		m_dpAccessInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dpAccessInterface->setToMonitor();
	}

	m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_entityName, this );
	TA_ASSERT(m_dnBulkPolledInterface, "can't create the dn bulk interface.");
	if ( operationMode == TA_Base_Core::Control )
	{
		// start receiving remote requests
		m_dnBulkPolledInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dnBulkPolledInterface->setToMonitor();
	}

	m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl ( m_entityName, this );
	TA_ASSERT(m_dnAccessInterface, "can't create the datanode access interface.");
	if ( operationMode == TA_Base_Core::Control )
	{
		// start receiving remote requests
		m_dnAccessInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dnAccessInterface->setToMonitor();
	}
    
    //At this point in the code your are saying that you have applied the mode
    //and are good to go
    
    m_pGenericAgent->run();
    

    FUNCTION_EXIT;
}

void RadioServerAgentFramework::checkOperationMode()
{
	TA_ASSERT(m_pGenericAgent != NULL, "m_genericAgent is NULL");
	
	if (m_pGenericAgent->getOperationMode() == TA_Base_Core::Monitor)
	{
		throw(TA_Base_Core::OperationModeException(RPARAM_MONITOR));
	}
}

TA_Base_Bus::DataPoint* RadioServerAgentFramework::getDataPoint(unsigned long entityKey)
{
	TA_Base_Bus::DataPoint* theDataPoint = NULL;

	try
	{
		// Find datapoint
		const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_pGenericAgent->getEntityMap();
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

void RadioServerAgentFramework::getAllDataPoints(std::map <unsigned long, TA_Base_Bus::DataPoint *>& dataPointList)
{
	// stop multi threads accessing the list of DataPoint objects at the same time
	TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

	dataPointList.clear();

	dataPointList = m_dataPointList;
}

TA_Base_Bus::DataNode* RadioServerAgentFramework::getDataNode(unsigned long entityKey)
{
	// stop multi threads accessing the list of DataNode objects at the same time
	TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );

	TA_Base_Bus::DataNode * dn = NULL;

	const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_pGenericAgent->getEntityMap();
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

void RadioServerAgentFramework::getAllDataNodes(std::map<unsigned long, TA_Base_Bus::DataNode*>& dataNodeList)
{
	TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );

	dataNodeList.clear();
	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		dataNodeList[ (*itr)->getEntityKey() ] = *itr;
	}        
}

TA_Base_Bus::DataPointFactory* RadioServerAgentFramework::getDataPointFactory()
{
	TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");

	return m_dataPointFactory;
}