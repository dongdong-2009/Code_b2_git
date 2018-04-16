/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StationPAAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/StationPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/TisCachedMaps.h"
#include "bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/AtsAgentSubscriber.h"
#include "app/pa/PAAgent/src/StationPAAgent.h"

using TA_Base_Bus::IEntity;
using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

	//
	// Constructor
	//
	// ExceptionChecked
	StationPAAgent::StationPAAgent(int argc, char* argv[])
	: AbstractPAAgent(),
	  m_atsAgentSubscriber(NULL)
	{
		FUNCTION_ENTRY("Constructor");

	    setup(argc,argv);

		FUNCTION_EXIT;
	}


	//
	// Destructor
	//
	// ExceptionChecked
	StationPAAgent::~StationPAAgent()
	{
		FUNCTION_ENTRY("Destructor");

	    cleanup();

        delete m_atsAgentSubscriber;
        m_atsAgentSubscriber = NULL;

		FUNCTION_EXIT;
	}


	// ExceptionChecked
	void StationPAAgent::createSpecificComponents()
	    //throw(TA_Base_Core::GenericAgentException, TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException)
	{
        FUNCTION_ENTRY("createSpecificComponents");

        m_atsAgentSubscriber = new AtsAgentSubscriber(*m_pStnBroadcastProxy);

        FUNCTION_EXIT;
    }

	// ExceptionChecked
	void StationPAAgent::destroySpecificComponents()
	{
	    FUNCTION_ENTRY("destroySpecificComponents");

	    FUNCTION_EXIT;
	}


	// ExceptionChecked
	void StationPAAgent::startSpecificComponents()
	{
	    FUNCTION_ENTRY("startSpecificComponents");

        if ( NULL != m_atsAgentSubscriber )
        {
            m_atsAgentSubscriber->subscribe();
        }

	    FUNCTION_EXIT;
	}


	// ExceptionChecked
	void StationPAAgent::stopSpecificComponents()
	{
        FUNCTION_ENTRY("stopSpecificComponents");

        if ( NULL != m_atsAgentSubscriber )
        {
            m_atsAgentSubscriber->unsubscribe();
        }

	    FUNCTION_EXIT;
	}

	// ExceptionChecked
	void StationPAAgent::processSpecificConfiguration()
	    //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
	{
	    FUNCTION_ENTRY("processSpecificConfiguration");

	    StationPAAgentEntityDataPtr entityData = boost::dynamic_pointer_cast<StationPAAgentEntityData>
	        ( m_genericAgent->getAgentEntityConfiguration() );

	    if (NULL==entityData.get())
	    {
	        TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Entity is not configured as a StationPAAgent entity type"));
	    }


	    // Note the following can raise the TA_Base_Core::DatabaseException or TA_Base_Core::DataException.

	    // Locate the ATS agent key for subscribing to messages

		TA_Base_Core::IEntityDataList allEntities =
	        EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( AtsAgentEntityData::getStaticType(), entityData->getLocation() );

	    if ( allEntities.size() != 1 )
	    {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ATS agent is not configured, ATS triggered broadcasts not possible" );
	    }
	    else
	    {
	        CachedConfig::getInstance()->setLocalATSAgentKey(allEntities[0]->getKey());
	    }

	    for (TA_Base_Core::IEntityDataList::iterator it = allEntities.begin(); it != allEntities.end(); ++it)
	    {
	        delete *it;
	    }
	    allEntities.clear();

	    FUNCTION_EXIT;
	}

	// ExceptionChecked
	void StationPAAgent::initialiseAndValidateCachedMaps()
	    //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
	{
	    FUNCTION_ENTRY("initialiseAndValidateCachedMaps");

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToStationDvaMessageRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaAtsTriggeredBroadcastRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaAtsTriggeredBroadcastCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneGroupRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToTrainDvaMessageRecordMap();
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaVersionRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap().startListening();

		TA_IRS_Bus::TisCachedMaps::getInstance().refreshPaTisMapping(CachedConfig::getInstance()->getAgentLocationKey());

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshDataPointMap(m_dataPoints, false);

	    TA_IRS_Bus::CachedMaps::getInstance()->validateAllInitialisedMaps();

	    FUNCTION_EXIT;
	}

	void StationPAAgent::agentSetMonitorSpecifics()
	{
	    FUNCTION_ENTRY("agentSetMonitorSpecifics");

	    stopSpecificComponents();

		FUNCTION_EXIT;

	}

	void StationPAAgent::agentSetControlSpecifics()
	{
	    FUNCTION_ENTRY("agentSetControlSpecifics");

	    startSpecificComponents();

		FUNCTION_EXIT;
	}

} // namespace TA_IRS_App


