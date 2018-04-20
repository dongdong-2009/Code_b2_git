/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationPAAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/StationPAAgent.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/AtsAgentSubscriber.h"
#include "bus/pa/cachedmaps/src/PaAtsTriggeredBroadcastCachedMap.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "app/pa/PAAgent/src/PasTableManager.h"

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
      m_fireCountdownAckMonitor(NULL),
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
        
        delete m_fireCountdownAckMonitor;
        m_fireCountdownAckMonitor = NULL;

		FUNCTION_EXIT;
	}


	// ExceptionChecked
	void StationPAAgent::createSpecificComponents()
	    //throw(TA_Base_Core::GenericAgentException, TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException)
	{
        FUNCTION_ENTRY("createSpecificComponents");

        m_atsAgentSubscriber = new AtsAgentSubscriber(*m_broadcastManager);

        if ( false == CachedConfig::getInstance()->getIsDepot() )
        {
            m_fireCountdownAckMonitor = new FireCountdownAckMonitor( m_pasTableManager->getTable570() );
        }

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

	    FUNCTION_EXIT;
	}


	// ExceptionChecked
	void StationPAAgent::stopSpecificComponents()
	{
	    FUNCTION_ENTRY("stopSpecificComponents");

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
	        PasHelpers::getInstance()->submitPaAgentDependencyFailureAuditMessage("ATS agent", "ATS triggered broadcasts not possible");
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

	    // TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaVersionRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
		TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaPrivateVersionRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
	    TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap().startListening();

	    TA_IRS_Bus::CachedMaps::getInstance()->refreshZoneIdToDataPointMap(m_dataPoints);


	    TA_IRS_Bus::CachedMaps::getInstance()->validateAllInitialisedMaps();

	    FUNCTION_EXIT;
	}


	void StationPAAgent::agentSetMonitorSpecifics()
	{
	    FUNCTION_ENTRY("agentSetMonitorSpecifics");

	    stopSpecificComponents();

		if (!CachedConfig::getInstance()->getIsDepot())
		{
		    m_fireCountdownAckMonitor->setMonitor();
		}

		FUNCTION_EXIT;

	}
	void StationPAAgent::agentSetControlSpecifics()
	{
	    FUNCTION_ENTRY("agentSetControlSpecifics");

	    startSpecificComponents();

		if (!CachedConfig::getInstance()->getIsDepot())
		{
			m_fireCountdownAckMonitor->setControl();
		}

		FUNCTION_EXIT;
	}

} // namespace TA_IRS_App


