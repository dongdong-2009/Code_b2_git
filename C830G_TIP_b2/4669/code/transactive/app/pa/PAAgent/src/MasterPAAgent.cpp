/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/MasterPAAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/MasterPAAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/MasterPAAgent.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaStationCachedMap.h"

// song toan++
// bug 459
#include "app/pa/PAAgent/src/PasTableManager.h"
// ++song toan
// bug 459

using TA_Base_Bus::IEntity;
using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

//
// Constructor
//
MasterPAAgent::MasterPAAgent(int argc, char* argv[])
: AbstractPAAgent()
{
	FUNCTION_ENTRY("Constructor");

    setup(argc, argv);

	FUNCTION_EXIT;
}


//
// Destructor
//
MasterPAAgent::~MasterPAAgent()
{
	FUNCTION_ENTRY("Destructor");

    cleanup();

	FUNCTION_EXIT;
}

// ExceptionChecked
void MasterPAAgent::createSpecificComponents()
    //throw(TA_Base_Core::GenericAgentException, TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException)
{
    FUNCTION_ENTRY("createSpecificComponents");

    FUNCTION_EXIT;
}

// ExceptionChecked
void MasterPAAgent::destroySpecificComponents()
{
    FUNCTION_ENTRY("destroySpecificComponents");

    FUNCTION_EXIT;
}

// ExceptionChecked
void MasterPAAgent::startSpecificComponents()
{
    FUNCTION_ENTRY("startSpecificComponents");

	FUNCTION_EXIT;
}

// ExceptionChecked
void MasterPAAgent::stopSpecificComponents()
{
    FUNCTION_ENTRY("stopSpecificComponents");

	FUNCTION_EXIT;
}


// ExceptionChecked
void MasterPAAgent::processSpecificConfiguration()
    //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
{
    FUNCTION_ENTRY("processSpecificConfiguration");

    MasterPAAgentEntityDataPtr entityData = boost::dynamic_pointer_cast<MasterPAAgentEntityData>
        ( m_genericAgent->getAgentEntityConfiguration() );

    if (NULL==entityData.get())
    {
        TA_THROW(InvalidPasConfigurationException("Entity is not configured as a MasterPAAgent entity type"));
    }


    FUNCTION_EXIT;
}

// ExceptionChecked
void MasterPAAgent::initialiseAndValidateCachedMaps()
    //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
{
    FUNCTION_ENTRY("initialiseAndValidateCachedMaps");

    // Get all zones and messages across the Circle Line
    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneRecordMap();
    TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().startListening();

    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToStationDvaMessageRecordMap(CachedConfig::getInstance()->getAgentLocationKey());
    TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap().startListening();

    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToTrainDvaMessageRecordMap();
    TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().startListening();

    // TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaVersionRecordMap();
	TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToDvaPublicVersionRecordMap();
    TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap().startListening();
    
    TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaStationRecordMap();
    TA_IRS_Bus::CachedMaps::getInstance()->getPaStationCachedMap().startListening();

    TA_IRS_Bus::CachedMaps::getInstance()->refreshZoneIdToDataPointMap(m_dataPoints);

    TA_IRS_Bus::CachedMaps::getInstance()->validateAllInitialisedMaps();

    FUNCTION_EXIT;
}


void MasterPAAgent::agentSetMonitorSpecifics()
{
    FUNCTION_ENTRY("agentSetMonitorSpecifics");

    stopSpecificComponents();

	FUNCTION_EXIT;

}
void MasterPAAgent::agentSetControlSpecifics()
{
    FUNCTION_ENTRY("agentSetControlSpecifics");

    startSpecificComponents();

// song toan++
// bug 459
	// TD15174
    // m_fireCountdownAckMonitor->setControl();
	// TD15174
// ++song toan
// bug 459

	FUNCTION_EXIT;
}


} // namespace TA_IRS_App

