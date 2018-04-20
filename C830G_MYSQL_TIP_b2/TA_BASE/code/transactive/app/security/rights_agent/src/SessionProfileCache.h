#if !defined(SESSIONPROFILECACHE_H_INCLUDED)
#define SESSIONPROFILECACHE_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/security/rights_agent/src/SessionProfileCache.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #4 $
 *
 * Last modification: $DateTime: 2011/06/14 10:26:22 $
 * Last modified by:  $Author: shurui.liu $
 *
 * Maintains a cache of sessions and profiles associated with those sessions
 */

namespace TA_Base_App
{
    class SessionProfileCache : public TA_Base_Bus::ISessionUpdateObserver
    {
    public:
	    SessionProfileCache( TA_Base_Bus::SessionUpdateSubject &sessionUpdate );
		virtual ~SessionProfileCache() {}

		void add( const std::string& session, const std::list<unsigned long>& profileList );
		// yanrong++
		void remove( const std::string& session );
		// ++yanrong
		bool lookup( const std::string& session, std::list<unsigned long>& profileList ) const;

		// Inherited from ISessionUpdateObserver
		virtual void sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation );
		virtual void sessionEnd( const std::string& session );
		virtual void sessionOverrideBegin( const std::string& session, unsigned long profile );
		virtual void sessionOverrideEnd( const std::string& session, unsigned long profile );

	private:

		typedef std::list<unsigned long> ProfileList;
		typedef std::map<std::string,ProfileList > SessionCache;

		SessionCache m_cache;
		mutable ACE_RW_Mutex m_mutex;
    };
}

#endif // !defined(SESSIONPROFILECACHE_H_INCLUDED)
