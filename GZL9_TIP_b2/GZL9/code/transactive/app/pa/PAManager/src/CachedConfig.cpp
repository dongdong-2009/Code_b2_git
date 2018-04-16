/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// CachedConfig.cpp: implementation of the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

CachedConfig* CachedConfig::m_me = NULL;

NonReEntrantThreadLockable CachedConfig::m_lockForConfigUpdate;
NonReEntrantThreadLockable CachedConfig::m_lockForObserverMap;

static const std::string   UNINITIALISED_STRING = "<dunno what yet>";
static const unsigned long UNINITIALISED_NUMBER = -1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CachedConfig::CachedConfig()
 : m_entityKey(0),
   m_entityName(""),
   m_entityTypeKey(0),
   m_entitySubsystemKey(0),
   m_pagingConsoleSourceId(0),
   m_pagingConsoleZoneId(0),
   m_ulPABackEntityKey(0),
   m_locationType(LOCATION_TYPE_INVALID)
{
    m_keyToObserversMap.clear();
}

CachedConfig::~CachedConfig()
{
    RunParams::getInstance().deregisterRunParamUser(this);

    m_keyToObserversMap.clear();
}

void CachedConfig::registerRunParamUser()
{
    // Have us listen out for changes to session ID
	RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
}


CachedConfig* CachedConfig::getInstance()
{
	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

		ThreadGuard guard( m_lockForConfigUpdate );

		if ( 0 == m_me )
		{
			m_me = new CachedConfig();
    	}
	}

	return m_me;
}

void CachedConfig::removeInstance( )
{
	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lockForConfigUpdate );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}
}

void CachedConfig::setEntityKey(unsigned long param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_entityKey = param;
}

void CachedConfig::setEntityName(const std::string& param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_entityName = param;
}

void CachedConfig::setEntityTypeKey(unsigned long param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_entityTypeKey = param;
}

void CachedConfig::setSessionId(const std::string& sessionId)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_sessionId = sessionId;

    try
    {
        IConsole* localConsole = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId, false);

        m_pagingConsoleSourceId = localConsole->getPagingConsoleSourceId();
        m_pagingConsoleZoneId   = localConsole->getPagingConsoleZoneId();

        delete localConsole;
        localConsole = NULL;
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
    }
}

void CachedConfig::onRunParamChange(const std::string& name, const std::string& value)
{
    if (RPARAM_SESSIONID == name)
    {
        // The session ID has changed, so must propogate associated rights changes throughout GUI
        setSessionId(value);

        PaRightsManager::getInstance().flagChangeInRights();
    }
}

void CachedConfig::setEntityLocationKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException)
{
    if (param == 0 || param == -1)
    {
        TA_THROW( InvalidPasConfigurationException("Invalid LocationKey set for PA Manager") );
    }

    ThreadGuard guard( m_lockForConfigUpdate );
    m_entityLocationKey.setValue(param);
}

void CachedConfig::setEntitySubsystemKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException)
{
    if ( param == -1)
    {
        TA_THROW( InvalidPasConfigurationException("Invalid SubsystemKey set for PA Manager") );
    }

    ThreadGuard guard( m_lockForConfigUpdate );
    m_entitySubsystemKey = param;
}

void CachedConfig::setPhysicalLocationKey(unsigned long key)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_physicalLocationKey = key;
}

unsigned long CachedConfig::getPhysicalLocationKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_physicalLocationKey;
}

void CachedConfig::setLocationType(ELocationType param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_locationType = param;
}

unsigned long CachedConfig::getEntityKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    //TA_ASSERT(m_entityKeyInitialised, "m_entityKey not set");
    return m_entityKey;
}

std::string CachedConfig::getEntityName()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_entityName;
}

unsigned long CachedConfig::getEntityTypeKey()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_entityTypeKey;
}

unsigned long CachedConfig::getEntityLocationKey()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(m_entityLocationKey.hasBeenSet(), "Fetching location key before it has been set");
    return m_entityLocationKey.getValue();
}

unsigned long CachedConfig::getEntitySubsystemKey()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_entitySubsystemKey;
}

std::string CachedConfig::getSessionId()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_sessionId;
}

unsigned long CachedConfig::getDepotLocationKey() 
{ 
    if (!m_depotLocationKey.hasBeenSet())
    {
        m_depotLocationKey.setValue( TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey() );
    }
    
    return m_depotLocationKey.getValue(); 
}

bool CachedConfig::isAtOcc()
{
    return (CachedConfig::LOCATION_TYPE_OCC == getLocationType());    
}

CachedConfig::ELocationType CachedConfig::getLocationType()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(m_locationType != LOCATION_TYPE_INVALID, "m_locationType not set");
    return m_locationType;
}

unsigned long CachedConfig::getLocalConsolePlaybackZoneId()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    
    return m_pagingConsoleZoneId;
}

unsigned long CachedConfig::getLocalConsoleRecordingSourceId()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    
    return m_pagingConsoleSourceId;
}

void CachedConfig::setPABackEntityKey( unsigned long ulKey )
{
    ThreadGuard guard( m_lockForConfigUpdate );

    m_ulPABackEntityKey = ulKey;
}

unsigned long CachedConfig::getPABackEntityKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );

    return m_ulPABackEntityKey;
}