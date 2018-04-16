/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingSessionManager.h $
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


#ifndef SCHEDULING_SESSION_MANAGER_H
#define SCHEDULING_SESSION_MANAGER_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include <map>
#include <utility>
#include <string>

namespace TA_Base_App
{



class SchedulingSessionManager : public TA_Base_Core::NonReEntrantThreadLockable
{

public:


	/**
	 *  Gets the single instance of this class.
     *  Note that the first time this method is called, initialise must be
     *  called.
	 */
	static SchedulingSessionManager& getInstance();


    /**
     *  Initialises the object.
     *
     *  This method is used instead of a constructor.
     *  It must be called the first time getInstance is invoked, or
     *  subsequent invocations will cause an assertion.
     *
     *  @param operatorID     The ID of the scheduling agent user.
     *  @param workstationID  The ID of the scheduling agent workstation.
     *  @param password       The scheduling agent's password.
     *  
     */
    void initialise(
        unsigned long operatorID,
        unsigned long workstationID,
        const std::string& password);

    /**
     *  Sets the operator ID for the scheduling agent.
     */
    void setOperatorID(unsigned long operatorID);

    /**
     *  Sets the workstation ID for the scheduling agent.
     */
    void setWorkstationID(unsigned long workstationID);

    /**
     *  Sets the password for the scheduling agent.
     */
    void setPassword(const std::string& password);


    /**
     *  Gets a session key for a given profile and location
     *
     *  @exception AuthenticationSecurityException
     *  Thrown if the log in attempt fails for security reasons.
     *
     *  @exception AuthenticationAgentException
     *  Thrown if there is a general error with the log in attempt.
     *
     *  @exception ObjectResolutionException
     *  Thrown if there is an issue resolving the authentication agent
     */
    std::string getSessionKey(
        unsigned long profileID, unsigned long locationID);


	/**
     *  Gets a super profile session key which is config is ta_session table
     *
     *  @exception DataException
     *  Thrown if the super profile session not configed in the table
     *  
     */
	std::string getSuperProfileSession();

    /**
     *  Gets the session information associated with the given sessionID
     *  from the Authentication Agent.
     *
     *  @exception AuthenticationAgentException
     *  Thrown if there is a general error with the attempt to retrieve
     *  the session info.
     */
    TA_Base_Bus::SessionInfo getSessionInfo(const std::string& sessionID);


    /**
     *  Destructor.
     */
    ~SchedulingSessionManager();



private:

	SchedulingSessionManager();
    SchedulingSessionManager(const SchedulingSessionManager&);
    const SchedulingSessionManager& operator=(const SchedulingSessionManager&);


    // The Authentication Library from which sessions are retrieved
    TA_Base_Bus::AuthenticationLibrary* m_authenticationLibrary;


    // The Scheduling Agent's operator ID
    unsigned long m_operatorID;

    // The Scheduling Agent's workstation ID
    unsigned long m_workstationID;

    // The Scheduling Agent's password
    std::string m_password;

    // Indicates if the initialise method has been called
    bool m_isInitialised;


    // The single instance of this class
    static SchedulingSessionManager* m_instance;

    // The lock used to stop multiple access to the instance of this class
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    // List of current and presumably active sessions for this 
    // Scheduling Agent instance to reuse (TD 10540)
    std::vector< TA_Base_Bus::SessionInfo> m_currentSessions;

	// Cache the super session info
	TA_Base_Bus::SessionInfo m_superSessionInfo;
};



}; // TA_Base_App


#endif // SCHEDULING_SESSION_MANAGER_H
