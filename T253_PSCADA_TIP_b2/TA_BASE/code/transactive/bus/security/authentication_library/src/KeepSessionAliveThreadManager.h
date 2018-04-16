/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/authentication_library/src/KeepSessionAliveThreadManager.h $
  * @author:   Anita Lee
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2018/02/27 13:38:15 $
  * Last modified by:  $Author: Ouyang $
  *
  */
#if !defined(KEEPSESSIONALIVETHREADMANAGER_H)
#define KEEPSESSIONALIVETHREADMANAGER_H

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <map>
#include <string>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/naming/src/NamedObject.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThread.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include <ace/Singleton.h>

namespace TA_Base_Bus
{

    class KeepSessionAliveThreadManager
    {
    public:

        /**
          * getInstance
          *
          * Returns THE instance of the KeepSessionAliveThreadManager.
          *
          * @return KeepSessionAliveThreadManager& The instance.
          */
        static KeepSessionAliveThreadManager& getInstance();

		static void  releaseInstance();


        /**
          * setAuthenticationAgentNamedObject
          *
          * Sets the named object of the authentication agent used to send keep alive
          * requests to.
          *
          * @param authNamedObject The named object of the authentication agent.
          */
        void setAuthenticationAgentNamedObject( AuthenticationAgentNamedObjPtr authNamedObject );

        /**
          * canFindSessionWithId
          *
          * Checks to see if a session with the given id is being kept alive. If one such
          * session is found the session details are filled in.
          *
          * @param sessionId The session to search for
          * @param session The details of the found session.
          *
          * @return bool The session was found.
          */
        bool canFindSessionWithId( const std::string& sessionId, SessionInfo& session );


       /**
          * startKeepingSessionAlive
          *
          * Adds the session details to the thread used to keep the sessions alive.
          *
          * @param session The session to keep alive.
          * @param authAgent The authentication agent that the session was requested from.
          */
        void startKeepingSessionAlive( const SessionInfo& session );


        /**
          * stopKeepingSessionAlive
          *
          * Removes the session details from the thread used to keep sessions alive.
          *
          * @param sessionId The session to stop keeping alive.
          */
        void stopKeepingSessionAlive( const std::string& sessionId );

		//TD18707 xinsong ++
		void initAuthAgentMap();

		AuthenticationAgentNamedObjPtr getAuthCorbaNameObject( unsigned long locationID );
		AuthenticationAgentNamedObjPtr getAuthCorbaNameObject( const std::string & sessionId );
		bool isSameLocationId( const std::string & sessionId, unsigned long locationId );

		bool isLocalLocationId(unsigned long locationId );

		unsigned long getLocalLocationId( )
		{
			TA_Assert( m_localLocationId > 0 );
			return m_localLocationId;
		}

		std::map<unsigned long, AuthenticationAgentNamedObjPtr> & getAuthenAgentMap()
		{
			return m_authAgentMap;
		}


    private:

        // Hidden methods.
        KeepSessionAliveThreadManager();
        ~KeepSessionAliveThreadManager();
        KeepSessionAliveThreadManager( const KeepSessionAliveThreadManager& );
        const KeepSessionAliveThreadManager& operator=( const KeepSessionAliveThreadManager& );
        friend class ACE_Singleton<KeepSessionAliveThreadManager, ACE_Recursive_Thread_Mutex>;

        // The thread used to keep the sessions alive.
        // There is no need to have multiple threads for multiple sessions as a single thread can
        // easily accomplish the task.
        KeepSessionAliveThread m_keepAliveThread;

        //AuthenticationAgentNamedObj m_localAuthAgent;
		std::map<unsigned long, AuthenticationAgentNamedObjPtr> m_authAgentMap;
		unsigned long m_localLocationId;

    };
}

#endif // !defined(KEEPSESSIONALIVETHREADMANAGER_H)
