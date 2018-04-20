/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingSessionManager.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/25 12:06:55 $
  * Last modified by:  $Author: builder $
  *
  *
  * This class is used to manage session keys for the Scheduling Agent.
  *
  */


#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

SchedulingSessionManager* SchedulingSessionManager::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingSessionManager::m_singletonLock;


SchedulingSessionManager::SchedulingSessionManager()
    :
    m_authenticationLibrary(NULL),
    m_operatorID(0),
    m_workstationID(0),
    m_password(""),
    m_isInitialised(false)
{
    FUNCTION_ENTRY("SchedulingSessionManager");

    m_authenticationLibrary = new TA_Base_Bus::AuthenticationLibrary();
    TA_ASSERT( m_authenticationLibrary != NULL, "m_authenticationLibrary should never be NULL");

    FUNCTION_EXIT;
}


SchedulingSessionManager::~SchedulingSessionManager()
{
    FUNCTION_ENTRY("~SchedulingSessionManager");

    delete m_authenticationLibrary;
    m_authenticationLibrary = NULL;

    FUNCTION_EXIT;
}


void SchedulingSessionManager::initialise(
    unsigned long operatorID,
    unsigned long workstationID,
    const std::string& password)
{
    FUNCTION_ENTRY("initialise");

    m_operatorID    = operatorID;
    m_workstationID = workstationID;
    m_password      = password;

    m_isInitialised = true;

    FUNCTION_EXIT;
}


SchedulingSessionManager& SchedulingSessionManager::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    //
    // If this method has been called before and initialise was not called,
    // assert.
    //
    TA_ASSERT(m_instance == NULL || m_instance->m_isInitialised,
              "SchedulingSessionManager::initialise() must be called the "
              "first time getInstance() is invoked");
    
    if(m_instance == NULL)
    {
        m_instance = new SchedulingSessionManager();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }

    return *m_instance;
}



void SchedulingSessionManager::setOperatorID(unsigned long operatorID)
{
    m_operatorID = operatorID;
}

void SchedulingSessionManager::setWorkstationID(unsigned long workstationID)
{
    m_workstationID = workstationID;
}

void SchedulingSessionManager::setPassword(const std::string& password)
{
    m_password = password;
}


std::string SchedulingSessionManager::getSessionKey(
    unsigned long profileID, unsigned long locationID)
{
    std::string sessionKey="";
    
    //
    //	TD 10540
    //	Get the current session key if it exists and if not then get a new session
    //
    
    std::vector< TA_Base_Bus::SessionInfo>::iterator sessionIter = m_currentSessions.begin();
    for ( ; sessionIter != m_currentSessions.end(); sessionIter++)
    {
        bool profileMatches( false);

        //
        //  Find a session that matches the fixed fields and then check its ProfileID vector
        //

        if ((sessionIter->LocationId == locationID) &&
            (sessionIter->WorkstationId == m_workstationID) &&
            (sessionIter->UserId == m_operatorID) )
        {
            std::vector<unsigned long>::const_iterator profileIter = sessionIter->ProfileId.begin();

            for ( ; profileIter != sessionIter->ProfileId.end(); profileIter++)
            {
                if ( *profileIter == profileID)
                {
                    profileMatches = true;
                    break;
                }
            }
            
            if (profileMatches)
            {
                //
                //	Use this session key because it matches the required location, operator,
                //  workstation and profile IDs ... but first make sure the session is active
                //
                try
                {
                    std::vector<TA_Base_Bus::SessionInfo> activeSessions = 
                        m_authenticationLibrary->getActiveSessions( sessionIter->SessionId);

                    if (activeSessions.size() <= 0)
                    {
                        //
                        //  The design notes say that getActiveSessions() should return an empty
                        //  vector if the session id is not valid, but at present it is throwing
                        //  TA_Base_Core::AuthenticationSecurityException::INVALID_SESSION instead.
                        //  The following code is duplicated in the exception handler below.
                        //
                        m_currentSessions.erase( sessionIter);
                        continue;
                    }
                }

                catch (TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException)
                {
                    //  This session is not active so remove it from the list and keep looking
                    m_currentSessions.erase( sessionIter);
                    continue;
                }

                sessionKey = sessionIter->SessionId;
                return sessionKey;
            }
        }
    }
    
    //
    // Get a new session key and return it.
    //
    sessionKey = m_authenticationLibrary->requestSession(
        m_operatorID,
        profileID,
        locationID,
        m_workstationID,
        m_password);
    
    m_currentSessions.push_back( getSessionInfo( sessionKey));

    return sessionKey;
}


TA_Base_Bus::SessionInfo
    SchedulingSessionManager::getSessionInfo(
        const std::string& sessionID)
{
    //
    // Get the session info from the authentication library.
    // The second parameter is the requesting session ID.
    // This is currently ignored by the authentication agent.
    //
    return m_authenticationLibrary->getSessionInfo(sessionID, "");
}

std::string SchedulingSessionManager::getSuperProfileSession()
{
	FUNCTION_ENTRY("getSuperProfileSession");
	const unsigned long OCC_LOCATION_KEY = 1;
	if (m_superSessionInfo.SessionId.empty())
	{
		m_superSessionInfo.LocationId = OCC_LOCATION_KEY; // this not used now , OCC locationKey = 1
		m_superSessionInfo.OperatorId.push_back(m_operatorID);

		std::auto_ptr<TA_Base_Core::IProfile> taSupperProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getTransActiveSuperProfile());

		unsigned long profileId = taSupperProfile->getKey();
		m_superSessionInfo.SessionId = TA_Base_Core::SessionAccessFactory::getSuperSessionId();
		m_superSessionInfo.ProfileId.push_back(profileId);
		m_superSessionInfo.UserId = m_operatorID;		
		m_superSessionInfo.WorkstationId = m_workstationID;

		m_currentSessions.push_back(m_superSessionInfo);
	}
	
	FUNCTION_EXIT;
	return m_superSessionInfo.SessionId;
}

}; // TA_Base_App

