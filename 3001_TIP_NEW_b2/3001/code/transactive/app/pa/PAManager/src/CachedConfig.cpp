/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/CachedConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/06/02 17:42:46 $
  * Last modified by:  $Author: huangjian $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"

#include <string>
#include <vector>
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

CachedConfig* CachedConfig::m_me = NULL;

NonReEntrantThreadLockable CachedConfig::m_lockForConfigUpdate;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CachedConfig::CachedConfig() : 
m_entityKey( 0u ),
m_ulMftLocKey( 0u ),
m_ulSchematicLocKey( 0u ),
m_pagingConsoleSourceId(0),
m_pagingConsoleZoneId(0),
m_eSchematicLocType( LOCATION_TYPE_INVALID ),
m_eMftLocType( LOCATION_TYPE_INVALID ),
m_eAgentLocType( LOCATION_TYPE_INVALID )
{
    TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, RPARAM_SESSIONID );
}

CachedConfig::~CachedConfig()
{
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

void CachedConfig::setSessionId(const std::string& sessionId)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_sessionId = sessionId;

    // Use the session ID to derive the local console recording 
    // source Id & zone information
    try
    {
        IConsole* localConsole = ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId, false);

        m_pagingConsoleSourceId = static_cast<unsigned short>(localConsole->getPagingConsoleSourceId());
        m_pagingConsoleZoneId = static_cast<unsigned short>(localConsole->getPagingConsoleZoneId());

        delete localConsole;
        localConsole = NULL;
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unkonwn exception" );
    }
}

void CachedConfig::onRunParamChange(const std::string& name, const std::string& value)
{
    if ( 0 == name.compare( RPARAM_SESSIONID ))
    {
        // The session ID has changed, so must propogate associated rights changes throughout GUI
        setSessionId( value );

        PaRightsManager::getInstance().flagChangeInRights();
    }
}

void CachedConfig::setAgentLocKey(unsigned long param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_ulAgentLocKey = param;
    TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_ulAgentLocKey);

	// Determine special location flags:
	if ( TA_Base_Core::ILocation::OCC == eLocationType )
	{
		m_eAgentLocType = LOCATION_TYPE_OCC;
	}
	else
	{ 
		if ( TA_Base_Core::ILocation::DPT == eLocationType )
		{
			m_eAgentLocType = LOCATION_TYPE_DEPOT;
		}
		else
		{
			// Station
			m_eAgentLocType = LOCATION_TYPE_STATION;
		}
	}
}


void CachedConfig::setPAAgentCorbaName(const TA_Base_Core::CorbaName& param)
{
    // Not sure what to check here
    
    // Scope to limit locking as the updateInterestedObservers() takes an unknown amount of time
    // Don't want to hold up any getX() operations in particular
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        std::string strStationCorbaObjName = param.getObjectName();
        strStationCorbaObjName += TA_Base_Bus::IPAAgentCorbaDef::STATION_PA_CORBA_NAME;

        TA_Base_Core::CorbaName stCorbaName( param.getAgentName(), strStationCorbaObjName );
        m_agentObject.setCorbaName( stCorbaName );
    }
}


void CachedConfig::setMftLocKey(unsigned long key)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_ulMftLocKey = key;
	TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_ulMftLocKey);

    // Determine special location flags:
	if (TA_Base_Core::ILocation::OCC == eLocationType)
    {
        m_eMftLocType = LOCATION_TYPE_OCC;
    }
    else
    { 
		if (TA_Base_Core::ILocation::DPT == eLocationType)
        {
            m_eMftLocType = LOCATION_TYPE_DEPOT;
        }
        else
        {
            // Station
            m_eMftLocType = LOCATION_TYPE_STATION;
        }
    }
}

void CachedConfig::setSchematicLocKey( unsigned long key )
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_ulSchematicLocKey = key;
	TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_ulSchematicLocKey);

    // Determine special location flags:
	if (TA_Base_Core::ILocation::OCC == eLocationType)
    {
        m_eSchematicLocType = LOCATION_TYPE_OCC;
    }
    else
    { 
		if (TA_Base_Core::ILocation::DPT == eLocationType)
        {
            m_eSchematicLocType = LOCATION_TYPE_DEPOT;
        }
        else
        {
            // Station
            m_eSchematicLocType = LOCATION_TYPE_STATION;
        }
    }
}

unsigned long CachedConfig::getMftLocKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_ulMftLocKey;
}

unsigned long CachedConfig::getSchematicLocKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_ulSchematicLocKey;
}

unsigned long CachedConfig::getEntityKey()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_entityKey;
}

unsigned long CachedConfig::getAgentLocKey()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_ulAgentLocKey;
}

std::string CachedConfig::getSessionId()
{
    // No assert checks made in case we access these details early due to alarms raised during startup
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_sessionId;
}

CachedConfig::ELocationType CachedConfig::getSchematicLocType()
{
    ThreadGuard guard( m_lockForConfigUpdate );
	if (LOCATION_TYPE_INVALID == m_eSchematicLocType)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_eSchematicLocType not set" );
	}
    return m_eSchematicLocType;
}

CachedConfig::ELocationType CachedConfig::getAgentLocType()
{
	ThreadGuard guard( m_lockForConfigUpdate );
	if (LOCATION_TYPE_INVALID == m_eAgentLocType)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_eAgentLocType not set" );
	}
	return m_eAgentLocType;
}

unsigned short CachedConfig::getLocalConsoleRecordingSourceId()
{
    ThreadGuard guard( m_lockForConfigUpdate );

    return m_pagingConsoleSourceId;
}


unsigned short CachedConfig::getLocalConsolePlaybackZoneId()
{
    ThreadGuard guard( m_lockForConfigUpdate );

    return m_pagingConsoleZoneId;
}


bool CachedConfig::isAtOcc()
{
    return (CachedConfig::LOCATION_TYPE_OCC == m_eMftLocType);    
}


bool CachedConfig::isAtDepot()
{
    return (CachedConfig::LOCATION_TYPE_DEPOT == m_eMftLocType);    
}


bool CachedConfig::isSchematicAtOcc()
{
    return (CachedConfig::LOCATION_TYPE_OCC == m_eSchematicLocType);    
}

unsigned long CachedConfig::getOccLocationKey()
{
    static unsigned long ulOccLocKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();

    return ulOccLocKey; 
}