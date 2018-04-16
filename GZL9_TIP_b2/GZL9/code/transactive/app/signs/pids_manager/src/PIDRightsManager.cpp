/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/PIDRightsManager.cpp $
  * @author Sean Liew
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by: $Author: grace.koh $
  * 
  * Contains passthrough functions for querying the right to perform
  * various operations
  *
  */

#pragma warning(disable:4284)   // For std::set warnings

#include "app/signs/pids_manager/src/StdAfx.h"
#include "app/signs/pids_manager/src/PIDRightsManager.h"

#include <set>

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ValueNotSetException.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;
namespace
{
	const std::string PIDAGENT_TYPE("PidAgent");
	enum
	{
		INVALID_PKEY = -1
	};
}

PIDRightsManager* PIDRightsManager::m_instance = NULL;

PIDRightsManager& PIDRightsManager::getInstance()
{
    FUNCTION_ENTRY("getInstance");
	if ( NULL == m_instance )
	{
        m_instance = new PIDRightsManager();
    	
        TA_ASSERT(m_instance, "Unable to create");
	}

    FUNCTION_EXIT;
	return *m_instance;
}

void PIDRightsManager::removeInstance()
{
    FUNCTION_ENTRY("removeInstance");
	if ( m_instance != NULL )
	{
		delete m_instance;
		m_instance = NULL;
	}
    FUNCTION_EXIT;
}

PIDRightsManager::PIDRightsManager():
m_rightsLibrary(NULL),
m_resourceKey(INVALID_PKEY)
{
    FUNCTION_ENTRY("Default Constructor");
    /////////////////////////START define right mappings
    #define MAP_RIGHT(x) { m_localToGlobalRightsMap[x] = TA_Base_Bus::aca_##x; }   
    MAP_RIGHT(STATION_TIS_PRE_DEFINED); 
    MAP_RIGHT(STATION_TIS_AD_HOC);
    MAP_RIGHT(STATION_TIS_RESET);
	MAP_RIGHT(STATION_TIS_PID_CONTROL);
    MAP_RIGHT(STATION_TIS_PID_GROUPS);
    MAP_RIGHT(RED_LINE_ACCESS);
    MAP_RIGHT(ORANGE_LINE_ACCESS);
    MAP_RIGHT(STATION_MODIFY_AD_HOC);

    #undef MAP_RIGHT
    /////////////////////////END define right mappings

    // Confirm all the rights exist (debug)
    TA_ASSERT(debugConfirmRightsExist(), "Rights map not set correctly");
    FUNCTION_EXIT;
}


PIDRightsManager::~PIDRightsManager()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

bool PIDRightsManager::hasRightToDisplayPredefinedMessage()
{
    FUNCTION_ENTRY("hasRightToDisplayPredefinedMessage");

	bool hasRights = hasRight(STATION_TIS_PRE_DEFINED);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToDisplayPredefinedMessage true" : "hasRightToDisplayPredefinedMessage false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::hasRightToDisplayAdhocMessage()
{
    FUNCTION_ENTRY("hasRightToDisplayAdhocMessage");

	bool hasRights = hasRight(STATION_TIS_AD_HOC);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToDisplayAdhocMessage true" : "hasRightToDisplayAdhocMessage false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::hasRightToClearStationMessage()
{
    FUNCTION_ENTRY("hasRightToClearStationMessage");

	bool hasRights = hasRight(STATION_TIS_RESET);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToClearStationMessage true" : "hasRightToClearStationMessage false");
	
    FUNCTION_EXIT;
    return hasRights;
}


bool PIDRightsManager::hasRightToModifyPIDGroups()
{
    FUNCTION_ENTRY("hasRightToModifyPIDGroups");

	bool hasRights = hasRight(STATION_TIS_PID_GROUPS);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToModifyPIDGroups true" : "hasRightToModifyPIDGroups false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::hasRightToAccessRedLine()
{
    FUNCTION_ENTRY("hasRightToAccessRedLine");

	bool hasRights = hasRight(RED_LINE_ACCESS);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToAccessRedLine true" : "hasRightToAccessRedLine false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::hasRightToAccessOrangeLine()
{
    FUNCTION_ENTRY("hasRightToAccessOrangeLine");

	bool hasRights = hasRight(ORANGE_LINE_ACCESS);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToAccessOrangeLine true" : "hasRightToAccessOrangeLine false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::hasRightToModifyAdhocMessage()
{
    FUNCTION_ENTRY("hasRightToModifyAdhocMessage");

	bool hasRights = hasRight(STATION_MODIFY_AD_HOC);	

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToModifyAdhocMessage true" : "hasRightToModifyAdhocMessage false");
	
    FUNCTION_EXIT;
    return hasRights;
}

bool PIDRightsManager::debugConfirmRightsExist()
{
    FUNCTION_ENTRY("debugConfirmRightsExist");
    try
    {
        std::set<unsigned long> temp;
        for (int i = 0; i < PID_RIGHT_MAX; i ++)
        {
            if (!temp.insert(convertLocalRightIdentifier(i)).second)
            {
                TA_ASSERT(false, "Multiple local rights mapped to same global right");
            }
        }
    }
    catch (TA_Base_Core::ValueNotSetException&)
    {
        TA_ASSERT(false, "Local right not identified by map");
    }
    FUNCTION_EXIT;
    return true;
}


unsigned long PIDRightsManager::convertLocalRightIdentifier(unsigned long localRight) const
{
    FUNCTION_ENTRY("convertLocalRightIdentifier");
    std::map<unsigned long, unsigned long>::const_iterator itr = 
                        m_localToGlobalRightsMap.find(localRight);

    if (itr == m_localToGlobalRightsMap.end())
    {
        TA_THROW(TA_Base_Core::ValueNotSetException()); // todo/kd - correct exception, description?
    }

    FUNCTION_EXIT;
    return itr->second;
}

unsigned long PIDRightsManager::getResourceKey() const
{
    FUNCTION_ENTRY("getResourceKey");
    
	//unsigned long resourcekey;
	if(INVALID_PKEY != m_resourceKey)
	{
		return m_resourceKey;
	}

    try
    {
		//get the phsical location key
		unsigned long locationKey = 0;
		if (RunParams::getInstance().isSet(RPARAM_LOCATIONKEY))
		{            
			std::stringstream converter;
			converter << RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			converter >> locationKey;
			
			TA_ASSERT(locationKey > 0, "Location key is invalid.");
		}
		else
		{
			TA_ASSERT(false, "Location key is not available as a runtime parameter.");
		}
		
		IEntityDataList pidAgentEntities =  EntityAccessFactory::getInstance().
											getEntitiesOfTypeAtLocation(PIDAGENT_TYPE, locationKey);  // may throw
		TA_ASSERT(pidAgentEntities.size() == 1, "Invalid number of local pid agents");
		unsigned long entityKey = pidAgentEntities[0]->getKey();
		//It is the responsibility of the *client* to delete the returned IEntityData objects 
		delete pidAgentEntities[0];
		
		TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().
											getResourceFromEntity( entityKey );
			
		TA_ASSERT(NULL != resource,"can't get the resource by the entityKey %")

		if(NULL == resource)
		{
			TA_THROW(TA_Base_Core::ValueNotSetException());
		}

		m_resourceKey = resource->getKey();

		delete resource;
		/*
        std::string entityname= RunParams::getInstance().get(RPARAM_ENTITYNAME);
        IEntityData* data = EntityAccessFactory::getInstance().getEntity(entityname);
        unsigned long entityKey = data->getKey();
        delete data;
        data = NULL;
        TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().
                                            getResourceFromEntity( entityKey );
        if (NULL == resource)
        {
            TA_THROW(TA_Base_Core::ValueNotSetException());
        }
	    resourcekey = resource->getKey();
        
	    delete resource;
        resource = NULL;
		*/
    }
	catch (const DataException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
    }	
    catch (TA_Base_Core::ValueNotSetException&)
    {
        TA_THROW(TA_Base_Core::ValueNotSetException()); // todo/kd
    }
    catch (TA_Base_Core::TransactiveException&)
    {       
        TA_THROW(TA_Base_Core::TransactiveException()); // todo/kd 
    }

    FUNCTION_EXIT;
    return m_resourceKey;
}

bool PIDRightsManager::hasRight(unsigned long right) const
{   
    FUNCTION_ENTRY("hasRight");
    if (NULL == m_rightsLibrary)
    {
        // Postpone instancing of rights library until first request  	
        TA_Base_Bus::RightsLibraryFactory rlFactory;
    
    	m_rightsLibrary = rlFactory.buildRightsLibrary();
    }

    if (NULL == m_rightsLibrary)
    {
        // Unable to create rights library, assume no rights by default
        return false;
    }

    bool response = false;
    std::string reasonString;
        
    try
    {
        unsigned long resourceKey = getResourceKey();
        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        
    	TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        unsigned long globalRightIdentifier = convertLocalRightIdentifier(right);
            
        response = m_rightsLibrary->isActionPermittedOnResource( 
                                                            sessionId,
														    resourceKey,
														    globalRightIdentifier,
														    reasonString,
														    decisionModule );
    }
    catch (TA_Base_Core::RightsException& ex)
    {       
        LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());
		response = false;
    }
    catch (TA_Base_Core::ValueNotSetException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "ValueNotSetException", ex.what());
		response = false;
    }
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException","");
		response = false;
	}

    FUNCTION_EXIT;
    return response;
}

bool PIDRightsManager::hasRightToControlPID()
{
    FUNCTION_ENTRY("hasRightToControlPID");
	
	bool hasRights = hasRight(STATION_TIS_PID_CONTROL);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		hasRights ? "hasRightToControlPID true" : "hasRightToControlPID false");
	
    FUNCTION_EXIT;
    return hasRights;
}


