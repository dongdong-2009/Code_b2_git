/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/CachedConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  */
// CachedConfig.cpp: implementation of the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/trains/TrainBorne_PA/src/StdAfx.h"

#include <string>
#include <vector>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/PaErrorHandler.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
//#include "app/trains/TrainBorne_PA/src/PaRightsManager.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "boost/tokenizer.hpp"
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

   m_localPAAgentKey                    ( UNINITIALISED_NUMBER ),
   m_localPAAgentCorbaNameAgentName     ( UNINITIALISED_STRING ),
   m_localPAAgentCorbaNameObjectName    ( UNINITIALISED_STRING ),
   m_localTrainAgentEntityName          ( UNINITIALISED_STRING ),
   m_localPAAgentCorbaNameLocationKey   ( UNINITIALISED_NUMBER ),
   m_locationType                       ( LOCATION_TYPE_INVALID )
{
    m_keyToObserversMap.clear();

    // Have us listen out for changes to session ID
	RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
	// 18611 libo 
	RunParams::getInstance().registerRunParamUser(this, "Display");
	// 18611 libo 
}

CachedConfig::~CachedConfig()
{
    RunParams::getInstance().deregisterRunParamUser(this);

    m_keyToObserversMap.clear();
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

    // Use the session ID to derive the local console recording 
    // source Id & zone information
    try
    {
        IConsole* localConsole = ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId, false);

        m_pagingConsoleSourceId = localConsole->getPagingConsoleSourceId();
        m_pagingConsoleZoneId   = localConsole->getPagingConsoleZoneId();
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // If unavailable, just leave unset, but display error
        if (sessionId.compare("debug") != 0)
        {
            // Only display the error if not in debug mode
            PaErrorHandler::displayError(PaErrorHandler::ENTITY_NAME_NOT_FOUND);
        }
    }
}


void CachedConfig::onRunParamChange(const std::string& name, const std::string& value)
{        
    if (RPARAM_SESSIONID == name)
    {
		// Maochun Sun++
		// TD14507
		// Change it back. Remove the change of TD14203. Change the Control Station to fix the both defects.

		// Maochun Sun++
		// TD14203

        // The session ID has changed, so must propogate associated rights changes throughout GUI
        setSessionId(value);

        //PaRightsManager::getInstance().flagChangeInRights();

		//::PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);

		// ++Maochun Sun
		// TD14203
		// ++Maochun Sun
		// TD14507
    }
	// 18611 libo 
	//if(CheckActiveSchematic(name,value))
	//{
	//	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_PA_MANAGER_ACTIVE, 0, 0);
	//}
	// 18611 libo 
}

//void CachedConfig::setEntityLocationKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException)
//{
//    if (param == 0 || param == -1)
//    {
//        TA_THROW( InvalidPasConfigurationException("Invalid LocationKey set for PA Manager") );
//    }
//
//    ThreadGuard guard( m_lockForConfigUpdate );
//    m_entityLocationKey.setValue(param);
//}

void CachedConfig::setEntitySubsystemKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException)
{
    if ( param == -1)
    {
        TA_THROW( InvalidPasConfigurationException("Invalid SubsystemKey set for PA Manager") );
    }

    ThreadGuard guard( m_lockForConfigUpdate );
    m_entitySubsystemKey = param;
}

void CachedConfig::setLocalPAAgentKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException)
{
    if (param == 0 || param == UNINITIALISED_NUMBER)
    {
        std::ostringstream msg;
        msg << "Invalid PA Manager parameter.  "
            << "LocalPAAgentKey=" << param;

        TA_THROW( InvalidPasConfigurationException(msg.str().c_str()) );
    }

    // Scope to limit locking as the updateInterestedObservers() takes an unknown amount of time
    // Don't want to hold up any getX() operations in particular
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        m_localPAAgentKey = param;
    }

    updateInterestedObservers(KEY_LOCAL_PA_AGENT_KEY);
}

void CachedConfig::setLocalPAAgentCorbaName(const TA_Base_Core::CorbaName& param) throw(TA_Base_Core::InvalidPasConfigurationException)
{
    // Not sure what to check here
    
    // Scope to limit locking as the updateInterestedObservers() takes an unknown amount of time
    // Don't want to hold up any getX() operations in particular
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        m_localPAAgentCorbaNameAgentName   = param.getAgentName();
        m_localPAAgentCorbaNameObjectName  = param.getObjectName();
        m_localPAAgentCorbaNameObjectName += TA_Base_Bus::IPAAgentCorbaDef::TRAIN_PA_CORBA_NAME;
        m_localPAAgentCorbaNameLocationKey = param.getLocationKey();

        TA_Base_Core::CorbaName stCorbaName( m_localPAAgentCorbaNameAgentName, m_localPAAgentCorbaNameObjectName );
        m_agentObject.setCorbaName( stCorbaName );
    }

    updateInterestedObservers(KEY_LOCAL_PA_AGENT_CORBA_NAME);
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


void CachedConfig::setLocalPAAgentEntityName(const std::string& name)
     throw(TA_Base_Core::InvalidPasConfigurationException)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_localPAAgentEntityName    = name;

    updateInterestedObservers(KEY_LOCAL_PA_AGENT_NAME);
}


std::string CachedConfig::getLocalPAAgentEntityName()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    return m_localPAAgentEntityName;
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
    /*ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(m_entityLocationKey.hasBeenSet(), "Fetching location key before it has been set");*/
    //return m_entityLocationKey.getValue();
	return getPhysicalLocationKey();
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


CachedConfig::ELocationType CachedConfig::getLocationType()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(m_locationType != LOCATION_TYPE_INVALID, "m_locationType not set");
    return m_locationType;
}

/**
 * attachObserver
 *
 * Adds the ICachedConfigObserver instance to the collection of observers.  If the observer 
 * already exists for the key, no change will be made.
 *
 * @param observer pointer to the ICachedConfigObserver instance
 *
 * @param key the ECachedConfigItemKey the observer is interested in
 *
 */
void CachedConfig::attachObserver(ICachedConfigObserver* observer, ECachedConfigItemKey key)
{

    // Search for all observers associated with the key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    bool found(false);
    for ( ; range.first != range.second; ++range.first)
    {
        if ( (range.first)->second == observer)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        ThreadGuard guard( m_lockForObserverMap );
        m_keyToObserversMap.insert(key2observer(key, observer));
    }
}


/**
 * detachObserver
 *
 * Removes all instances of the ICachedConfigObserver pointer in the observer collection.  The
 * observer will be removed for all keys.
 *
 */
void CachedConfig::detachObserver(ICachedConfigObserver* observer)
{
    ThreadGuard guard( m_lockForObserverMap );

    KeyToObserversMapIt it = m_keyToObserversMap.begin();
    while (it != m_keyToObserversMap.end())
    {
        if (it->second == observer)
        {
            m_keyToObserversMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

/**
 * updateInterestedObservers
 *
 * Calls the processCachedConfigUpdate() method on all registered observers interested in the 
 * specified key.  Implementors of ICachedConfigObserver MUST make sure that no exceptions 
 * are unhandled.
 *
 * @param key used to call specific observers
 *
 */
void CachedConfig::updateInterestedObservers(ECachedConfigItemKey key)
{
    ThreadGuard guard( m_lockForObserverMap );

    // Search for all observers associated with a key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    for ( ; range.first != range.second; ++range.first)
    {
        (range.first)->second->processCachedConfigUpdate(key);
    }
}


unsigned long CachedConfig::getLocalConsoleRecordingSourceId()
{
    ThreadGuard guard( m_lockForConfigUpdate );

    return m_pagingConsoleSourceId;
}


unsigned long CachedConfig::getLocalConsolePlaybackZoneId()
{
    ThreadGuard guard( m_lockForConfigUpdate );

    return m_pagingConsoleZoneId;
}

bool CachedConfig::isAtOcc()
{
    return (CachedConfig::LOCATION_TYPE_OCC == getLocationType());    
}

std::string CachedConfig::getLocalTrainAgentEntityName()
{
    ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(m_localTrainAgentEntityName != UNINITIALISED_STRING, "m_localTrainAgentEntityName not set");

    return m_localTrainAgentEntityName;
}


void CachedConfig::setLocalTrainAgentEntityName(const std::string& param)
{
    ThreadGuard guard( m_lockForConfigUpdate );
    m_localTrainAgentEntityName = param;
}


std::string CachedConfig::getDeletedStationDvaMessageName() const
{
    return "<Deleted DVA Message>";
}


std::string CachedConfig::getDeletedTrainDvaMessageName() const
{
    return "<Deleted DVA Message>";
}

BOOL CachedConfig::CheckActiveSchematic( const std::string& name, const std::string& value )
{
	const std::string separatorList =",";
	BOOL bFlag = FALSE;
	std::vector<std::string> parts;
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( separatorList.c_str() );
    tokenizer tokens(value, sep);
    
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        parts.push_back(*tok_iter);
    }

    // if parts is empty, then this should return the entire string
    if (parts.size() == 0)
    {
        parts.push_back(value);
    }
	if( (name.compare("Display") == 0) && (parts[2].compare("SHOW") == 0))
	{
		bFlag = TRUE;
	}
	return  bFlag;
}


BOOL CachedConfig::GetActiveSchematicFlag()
{
	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "CachedConfig:: GetActiveSchematicFlag");

	std::string displayValue = TA_Base_Core::RunParams::getInstance().get("Display");
	if(displayValue == "")
		return FALSE;
	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "CachedConfig::Display ='%s'",displayValue.c_str());
	if(!CheckActiveSchematic("Display", displayValue))
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "CachedConfig::HIDE");
		return FALSE;
	}
	LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "CachedConfig::SHOW");
	return TRUE;
}
