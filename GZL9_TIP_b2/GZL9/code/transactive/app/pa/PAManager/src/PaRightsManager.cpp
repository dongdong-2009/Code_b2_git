/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Sean Liew
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * Contains passthrough functions for querying the right to perform
  * various operations
  *
  */

#pragma warning(disable:4284)   // For std::set warnings

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/pa/PAManager/src/IRightsChangeListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PaRightsManager* PaRightsManager::s_me = 0;

PaRightsManager::PaRightsManager()
:
m_rightsLibrary(0)
{
    /////////////////////////START define right mappings
    #define MAP_RIGHT(x) { m_localToGlobalRightsMap[x] = TA_Base_Bus::aca_##x; }

	MAP_RIGHT(PA_BROADCAST_STATION_DVA);    
	MAP_RIGHT(PA_BROADCAST_OCC_DVA);
	MAP_RIGHT(PA_BROADCAST_STATION_LIVE);
	MAP_RIGHT(PA_BROADCAST_OCC_LIVE);
	MAP_RIGHT(PA_BROADCAST_STN_BACKGROUND_MUSIC);
	MAP_RIGHT(PA_BROADCAST_OCC_BACKGROUND_MUSIC);
	MAP_RIGHT(PA_BROADCAST_STN_MONITOR);
	MAP_RIGHT(PA_BROADCAST_OCC_MONITOR);
	MAP_RIGHT(PA_MODIFY_ZONE_GROUPS);
	MAP_RIGHT(PA_CONFIGURE_PA_MESSAGE_DESCRIPTION);

    #undef MAP_RIGHT
    /////////////////////////END define right mappings

    // Confirm all the rights exist (debug)
    TA_ASSERT(debugConfirmRightsExist(), "Rights map not set correctly");
}


PaRightsManager::~PaRightsManager()
{
    try
    {
        delete m_rightsLibrary;
        m_rightsLibrary = 0;
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
    }
}


PaRightsManager& PaRightsManager::getInstance()
{
	if ( 0 == s_me )
	{
        s_me = new PaRightsManager();
    	
        TA_ASSERT(s_me, "Unable to create");
	}

	return *s_me;
}


void PaRightsManager::removeInstance( )
{
	if ( s_me != NULL )
	{
		delete s_me;
		s_me = NULL;
	}
}


bool PaRightsManager::debugConfirmRightsExist()
{
    try
    {
        std::set<unsigned long> temp;
        for (int i = 0; i < PA_RIGHT_MAX; i ++)
        {
            if (!temp.insert(convertLocalRightIdentifier(i)).second)
            {
                TA_ASSERT(false, "Multiple local rights mapped to same global right");
            }
        }
    }
    catch (const TA_Base_Core::ValueNotSetException&)
    {
        TA_ASSERT(false, "Local right not identified by map");
    }

    return true;
}


unsigned long PaRightsManager::convertLocalRightIdentifier(unsigned long localRight) const
{
    std::map<unsigned long, unsigned long>::const_iterator itr = 
                        m_localToGlobalRightsMap.find(localRight);

    if (itr == m_localToGlobalRightsMap.end())
    {
        TA_THROW(TA_Base_Core::ValueNotSetException());
    }

    return itr->second;
}


bool PaRightsManager::hasRight(EPublicRight right) const
{   
    // Access internal implementation (non typesafe)
    return hasRight((unsigned long)right);
}

bool PaRightsManager::hasRight(unsigned long right) const
{
    try
    {
        if (0 == m_rightsLibrary)
        {
            // Postpone instancing of rights library until first request  	
            TA_Base_Bus::RightsLibraryFactory rlFactory;

    	    m_rightsLibrary = rlFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
        }
    }
    catch (...)
    {
    }

    if (0 == m_rightsLibrary)
    {
        // Unable to create rights library, assume no rights by default
        return false;
    }

    bool response = false;

    try
    {
        unsigned long resourceKey = getResourceKey();
        std::string reasonString = "";
    	TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        unsigned long globalRightIdentifier = convertLocalRightIdentifier(right);
            
        response = m_rightsLibrary->isActionPermittedOnResource( 
                                   CachedConfig::getInstance()->getSessionId(),
														    resourceKey,
														    globalRightIdentifier,
														    reasonString,
														    decisionModule );
    }
    catch (const TA_Base_Core::RightsException&)
    {       
    }
    catch (const TA_Base_Core::ValueNotSetException&)
    {
    }
    catch (...)
    {
    }

    return response;
}

unsigned long PaRightsManager::getResourceKey() const
{
    if (m_resourceKey.hasBeenSet())
    {
        // Return the cached version
        return m_resourceKey.getValue();
    }

    unsigned long entityKey = CachedConfig::getInstance()->getEntityKey();
    TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().
                                        getResourceFromEntity( entityKey );
    if (0 == resource)
    {
        TA_THROW(TA_Base_Core::ValueNotSetException());
    }
	m_resourceKey.setValue(resource->getKey());

	delete resource;

    return m_resourceKey.getValue();
}

std::auto_ptr<TA_Base_Core::ThreadGuard> PaRightsManager::obtainReEntrantLock()
{
    std::auto_ptr<TA_Base_Core::ThreadGuard> result(new TA_Base_Core::ThreadGuard(m_lock));
    return result;
}


bool PaRightsManager::hasRightToBroadcastLive()
{
    bool bHasRight = false;

    if ( CachedConfig::getInstance()->isAtOcc() )
    {
        bHasRight = hasRight( PA_BROADCAST_OCC_LIVE );
    }
    else
    {
        bHasRight = hasRight( PA_BROADCAST_STATION_LIVE );
    }

    // In addition to the right, the operator must be physically
    // located (defined by locationKey) at the station we're operating for
    bHasRight &= isPhysicallyLocatedAtOperatingLocation();

    return bHasRight;
}

bool PaRightsManager::hasRightToBackgroundMusic()
{
	bool bHasRight = false;

	if ( CachedConfig::getInstance()->isAtOcc() )
	{
        bHasRight = hasRight( PA_BROADCAST_OCC_BACKGROUND_MUSIC );
	}
	else
	{
        bHasRight = hasRight( PA_BROADCAST_STN_BACKGROUND_MUSIC );
	}

	// In addition to the right, the operator must be physically
	// located (defined by locationKey) at the station we're operating for
	bHasRight &= isPhysicallyLocatedAtOperatingLocation();

	//return bHasRight;
	return true;
}

bool PaRightsManager::hasRightToPlaybackDvaMessage()
{
    // Operator must be physically located at the same station 
    // to allow playing of DVA Messages
    if (isPhysicallyLocatedAtOperatingLocation())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PaRightsManager::isPhysicallyLocatedAtOperatingLocation()
{
    return CachedConfig::getInstance()->getPhysicalLocationKey() ==   
           CachedConfig::getInstance()->getEntityLocationKey();
}

void PaRightsManager::registerRightsChangeListener(IRightsChangeListener& listener)
{
    TA_Base_Core::ThreadGuard sync(m_lock);

    if (!m_rightsChangeListeners.insert(&listener).second)
    {
        TA_ASSERT(FALSE, "Listener already registered");
    }
}


void PaRightsManager::deRegisterRightsChangeListener(IRightsChangeListener& listener) 
{
    TA_Base_Core::ThreadGuard sync(m_lock);

    std::set<IRightsChangeListener*>::iterator itr = m_rightsChangeListeners.find(&listener);

    if (itr == m_rightsChangeListeners.end())
    {
        TA_ASSERT(FALSE, "Could not find listener to deRegister");
    }
    else
    {
        m_rightsChangeListeners.erase(itr);
    }
}


void PaRightsManager::flagChangeInRights()
{
    TA_Base_Core::ThreadGuard sync(m_lock);

    std::vector<unsigned long> globalRightIds;

    // For now, send out notification that all rights have changed
    for (int i = 0; i < PA_RIGHT_MAX; i ++)
    {
        globalRightIds.push_back(convertLocalRightIdentifier(i));
    }

    for (std::set<IRightsChangeListener*>::iterator itr = m_rightsChangeListeners.begin();
            itr != m_rightsChangeListeners.end();
            itr ++)
    {
        (*itr)->onRightsChanged(globalRightIds);
    }
}

bool PaRightsManager::hasRightToBroadcastDva()
{
    bool bHasRight = false;

    if ( CachedConfig::getInstance()->isAtOcc() )
    {
        bHasRight = hasRight( PA_BROADCAST_OCC_DVA );
    }
    else
    {
        bHasRight = hasRight( PA_BROADCAST_STATION_DVA );
    }

    return bHasRight;
}

//begin-added by lixiaoxia
bool PaRightsManager::hasRightToStartMonitor()
{
	bool bHasRight = false;

	if ( CachedConfig::getInstance()->isAtOcc() )
	{
		bHasRight = hasRight( PA_BROADCAST_OCC_MONITOR );
	}
	else
	{
		bHasRight = hasRight( PA_BROADCAST_STN_MONITOR );
	}
	
	// In addition to the right, the operator must be physically
	// located (defined by locationKey) at the station we're operating for
	bHasRight &= isPhysicallyLocatedAtOperatingLocation();

	//return bHasRight;
	return true;
}

bool PaRightsManager::hasRightToStopMonitor()
{
	bool bHasRight = false;

	if ( CachedConfig::getInstance()->isAtOcc() )
	{
		bHasRight = hasRight( PA_BROADCAST_OCC_MONITOR );
	}
	else
	{
		bHasRight = hasRight( PA_BROADCAST_STN_MONITOR );
	}

	// In addition to the right, the operator must be physically
	// located (defined by locationKey) at the station we're operating for
	bHasRight &= isPhysicallyLocatedAtOperatingLocation();

	//return bHasRight;
	return true;
}
//end-added by lixiaoxia

bool PaRightsManager::hasRightToStopDva()
{
    bool bHasRight = false;

    if ( CachedConfig::getInstance()->isAtOcc() )
    {
        bHasRight = hasRight( PA_BROADCAST_OCC_DVA );
    }
    else
    {
        bHasRight = hasRight( PA_BROADCAST_STATION_DVA );
    }

    return bHasRight;
}

bool PaRightsManager::hasRightToModifyDvaMsgDescription()
{
    return hasRight( PA_CONFIGURE_PA_MESSAGE_DESCRIPTION );
}

bool PaRightsManager::hasRightToModifyZoneGroup()
{
    return hasRight( PA_MODIFY_ZONE_GROUPS );
}
