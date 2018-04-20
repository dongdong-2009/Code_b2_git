#ifndef RIGHTSCACHE_H_INCLUDED
#define RIGHTSCACHE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Caches rights.
  */

// [[Add start by HuangQi for TD17256 on 2009-07-20
namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}
// Add end by HuangQi on 2009-07-20]]

namespace TA_Base_Bus
{
    // [[Modify start by HuangQi for TD17256 on 2009-07-20
	// class RightsCache : public ISessionUpdateObserver
    class RightsCache : public ISessionUpdateObserver,
                        public TA_Base_Core::IOnlineUpdatable
    // Modify end by HuangQi on 2009-07-20]]
	{


	public:
		RightsCache(SessionUpdateSubject& sessionUpdate);
        // Added by HuangQi for TD17256 on 2009-07-20
        ~RightsCache();

		void add( const std::string& session, unsigned long resource, unsigned long action,
			bool result, const std::string& reason, EDecisionModule module );

		bool lookup( const std::string& session, unsigned long resource, unsigned long action,
			bool& result, std::string& reason, EDecisionModule& module ) const;

		bool lookup( const std::string& session, unsigned long resource, unsigned long action,
			unsigned long & location , unsigned long& subsystem, bool& result, std::string& reason, EDecisionModule& module ) const;

		void add( const std::string& session, unsigned long location, unsigned long subsystem, unsigned long action,
			bool result, const std::string& reason, EDecisionModule module );

		bool lookup( const std::string& session, unsigned long location, unsigned long subsystem, unsigned long action,
			bool& result, std::string& reason, EDecisionModule& module ) const;

		bool processDutyUpdate( const std::string& session, unsigned long location,
			unsigned long subsystem );

        // Added by HuangQi for TD17256 on 2009-07-20
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		void sessionBegin( const std::string& session, unsigned long user,
			unsigned long profile, unsigned long location,
								   unsigned long workstation );

		void sessionEnd  ( const std::string& session );		
	

		void sessionOverrideBegin( const std::string& session, unsigned long profile );
		void sessionOverrideEnd  ( const std::string& session, unsigned long profile );

		void reset();
		void reset(unsigned long location);
		void notifyRightsChange(std::string& session,ERightsChangeType changeType, DutyChanges& changes);
		const std::string& getSession() const;

		void reload();

	protected:
		friend class CorbaRightsLibrary;
		void setRightsLibrary(CorbaRightsLibrary* rightsLibrary);
		void  inline setSession(const std::string& session) const;
		void invalidateSession();

		void changeSessionRights( const std::string& session, ERightsChangeType changeType);
		void getSubsystems(unsigned long location, std::list<unsigned long>& subsystems);
		bool isSuperSession( const std::string& sessionId );

	private:
		RightsCache( const RightsCache& );
		RightsCache& operator=( const RightsCache& );

		typedef std::set<RightsQuery> Cache;

		Cache m_cache;
		mutable LocSubDbCache m_dbCache;

		mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;
		CorbaRightsLibrary* m_rightsLibrary;

		TA_Base_Core::ThreadPoolSingletonManager * m_threadPool;

		std::string m_hostname;
		unsigned long m_consoleKey;
		mutable std::string m_session;
		mutable bool m_isSetSession;

		typedef TA_Base_Core::NamedObject<IRightsAgentCorbaDef,
			IRightsAgentCorbaDef_ptr,
			IRightsAgentCorbaDef_var> RightsAgentType;
		
		boost::shared_ptr<RightsAgentType> m_rightsAgent;
	};
	class RightsCacheWorkItem: public TA_Base_Core::IWorkItem
	{
	public:
		RightsCacheWorkItem(RightsCache& parent,std::string session,ERightsChangeType changeType, boost::shared_ptr<DutyChanges> dutyChanges );
		virtual ~RightsCacheWorkItem();

		//operations specific to IWorkItem
		virtual void executeWorkItem();
	protected:
		RightsCache& parent_;
		std::string session_;
		ERightsChangeType changeType_;
		boost::shared_ptr<DutyChanges> dutyChanges_;
	};
}

#endif // RIGHTSCACHE_H_INCLUDED
