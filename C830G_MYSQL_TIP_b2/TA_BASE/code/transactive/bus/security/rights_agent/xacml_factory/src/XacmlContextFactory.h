#if !defined(XACMLCONTEXTFACTORY_H_INCLUDED)
#define XACMLCONTEXTFACTORY_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File$
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 */

namespace TA_Base_Bus
{
	class XacmlContextFactory
	{
	public:

		XacmlContextFactory();
		~XacmlContextFactory();
		std::auto_ptr<Context> build( const TA_Base_Bus::SessionInfo& sessionId,
			unsigned long subject, unsigned long resource, unsigned long action );

		std::auto_ptr<Context> build( const TA_Base_Bus::SessionInfo& sessionId,
			unsigned long subject, unsigned long location, unsigned long subsystem, unsigned long action );

	private:

		XacmlContextFactory( const XacmlContextFactory& x );
		XacmlContextFactory& operator=( const XacmlContextFactory& x );

		void XacmlContextFactory::initCache();

		void getActionAttributes( std::auto_ptr<Context>& ctx, unsigned long action );

		void getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx,
			unsigned long subject, unsigned long resource );

		void getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx,
			unsigned long subject, unsigned long location, unsigned long subsystem );

		void getSubjectAttributesFromDutyAgent( std::auto_ptr<Context>& ctx,
			const TA_Base_Bus::SessionInfo& session, unsigned long subject,
			unsigned long resource );

		void getSubjectAttributesFromDutyAgent( std::auto_ptr<Context>& ctx,
			const TA_Base_Bus::SessionInfo& session, unsigned long subject,
			unsigned long location, unsigned long subsystem );

		bool isPhysicalResource( unsigned long resource );
		bool isPhysicalSubsystem( unsigned long subsystem);
		
		void XacmlContextFactory::getLocSub( unsigned long resource,
			unsigned long& location, unsigned long& subsystem );

		void getSubsystem( TA_Base_Core::IEntityData* resource, unsigned long& subsystem, bool& isPhysical ) const;

		std::string XacmlContextFactory::getProfileNameFromCache( unsigned long subject );

		std::string XacmlContextFactory::getActionGroupNameFromCache( unsigned long dutyType );

		RightsType::ProfileMap     m_profileMap;
		RightsType::ActionMap      m_actionMap;
		RightsType::ResourceMap    m_resourceMap;
		RightsType::SubsystemMap   m_subsystemMap;
		RightsType::ActionGroupMap m_actionGroupMap;
		RightsType::LocSubDutyMap  m_locSubDutyMap;

		// TODO: Due to caching, only the write lock is being used. Maybe
		// replace these with simple Mutex locks (except for locSubDuty).
		ACE_RW_Mutex m_profileLock;
		ACE_RW_Mutex m_actionLock;
		ACE_RW_Mutex m_resourceLock;
		ACE_RW_Mutex m_subsystemLock;
		ACE_RW_Mutex m_actionGroupLock;
		ACE_RW_Mutex m_locSubDutyLock;
		
		ContextOnlineUpdatable      m_onlineUpdater;
		ContextSubDutyMsgSubscriber m_dutyMsgSubscriber;

		typedef TA_Base_Core::NamedObject<IDutyAgentQueryCorbaDef,
				IDutyAgentQueryCorbaDef_ptr,
				IDutyAgentQueryCorbaDef_var> DutyAgentQueryType;		

		typedef std::map<unsigned long,boost::shared_ptr<DutyAgentQueryType> > DutyAgentMap;
		DutyAgentMap m_dutyagentMap;

		static const std::string INVALID_RIGHTS;
		static const std::string INVALID_DUTY;

	};
}

#endif // !defined(XACMLCONTEXTFACTORY_H_INCLUDED)
