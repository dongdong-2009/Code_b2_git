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
 * This class implements the IRightsAgentCorbaDef interface.
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{
	const std::string XacmlContextFactory::INVALID_DUTY("INVALID_DUTY");
	const std::string XacmlContextFactory::INVALID_RIGHTS("INVALID_RIGHTS");

	XacmlContextFactory::XacmlContextFactory() :
		m_onlineUpdater(m_profileMap, m_profileLock, m_actionMap, m_actionLock,
			m_resourceMap, m_resourceLock, m_subsystemMap, m_subsystemLock,
			m_actionGroupMap, m_actionGroupLock),
		m_dutyMsgSubscriber(m_locSubDutyMap, m_locSubDutyLock)	
	{
		
		TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "RPARAM_LOCATIONKEY unset" );
		// Get the entity name based on the locationKey
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );
		std::string agentType = TA_Base_Core::DutyAgentEntityData::getStaticType();
		// this will only return a single entry
		TA_Base_Core::CorbaNameList dutyAgentNames = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtAllLocation(agentType,true);

		TA_ASSERT(dutyAgentNames.size()>0,"Can't load  DutyAgents data!");
	
		int size = dutyAgentNames.size();
		for(int i=0;i< size; ++i )
		{
			boost::shared_ptr<DutyAgentQueryType> duty ( new DutyAgentQueryType(dutyAgentNames[i]));
			m_dutyagentMap.insert(DutyAgentMap::value_type(dutyAgentNames[i].getLocationKey(), duty ));
		}	
		
		initCache();
	}

	XacmlContextFactory::~XacmlContextFactory()
	{		

		//Clear Profile, Action, Resource and Duty caches
		{
			RightsType::ProfileMap::iterator it;

			for( it = m_profileMap.begin(); it != m_profileMap.end(); it++ )
			{
				delete it->second;
			}
		}
		{
			RightsType::ActionMap::iterator it;

			for( it = m_actionMap.begin(); it != m_actionMap.end(); it++ )
			{
				delete it->second;
			}
		}
		{
			RightsType::ResourceMap::iterator it;

			for( it = m_resourceMap.begin(); it != m_resourceMap.end(); it++ )
			{
				delete it->second;
			}
		}
		{
			RightsType::SubsystemMap::iterator it;

			for( it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++ )
			{
				delete it->second;
			}

			m_subsystemMap.clear();
		}
		{
			RightsType::ActionGroupMap::iterator it;

			for( it = m_actionGroupMap.begin(); it != m_actionGroupMap.end(); it++ )
			{
				delete it->second;
			}

		}
	}

	void XacmlContextFactory::initCache()
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "start to init Rights data Cache");
		
		unsigned long tempKey=0;
		// Now reload all the profiles
	        std::vector<TA_Base_Core::IProfile*> profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles( false);

	        // And insert the profiles into our map. 
	        for (std::vector<TA_Base_Core::IProfile*>::iterator iter = profiles.begin();
	             iter != profiles.end();
	             ++iter
	             )
	        {
	             TA_Base_Core::Profile* theProfile = static_cast<TA_Base_Core::Profile*>(*iter);
	             
			tempKey=theProfile->getKey();
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "about to insert profile: %lu into cache",tempKey);
			m_profileMap.insert(RightsType::ProfileMap::value_type( tempKey, theProfile  ) );
			
	        }
		//===========debug: check profile insert================

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "profile cache size : %lu ",(unsigned long)m_profileMap.size());

		//============================================
		

		// Now reload all the actions
	        std::vector<TA_Base_Core::IAction*> actions = TA_Base_Core::ActionAccessFactory::getInstance().getAllActions();

	        // And insert the actions into our map. 
	        for (std::vector<TA_Base_Core::IAction*>::iterator actionIter = actions.begin();
	             actionIter != actions.end();
	             ++actionIter
	             )
	        {
	             TA_Base_Core::IAction* theAction = *actionIter;
	             
			tempKey=theAction->getKey();
			m_actionMap.insert(RightsType::ActionMap::value_type( tempKey, theAction) );
	        }
		//===========debug: check action insert================

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "action cache size : %lu ",(unsigned long)m_actionMap.size());

		//============================================

		// Now reload all the actionGroup
	        std::vector<TA_Base_Core::IActionGroup*> actionGroups = TA_Base_Core::ActionGroupAccessFactory::getInstance().getAllActionGroups(false);

	        // And insert the actionGroups into our map. 
	        for (std::vector<TA_Base_Core::IActionGroup*>::iterator actionGroupIter = actionGroups.begin();
	             actionGroupIter != actionGroups.end();
	             ++actionGroupIter
	             )
	        {
	             TA_Base_Core::ActionGroup* theActionGroup = static_cast<TA_Base_Core::ActionGroup*>(*actionGroupIter);
	             
			tempKey=theActionGroup->getKey();
			
			m_actionGroupMap.insert(RightsType::ActionGroupMap::value_type( tempKey, theActionGroup) );
	        }
		//===========debug: check actionGroup insert================

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "actionGroup cache size : %lu ",(unsigned long)m_actionGroupMap.size());

		//============================================
		 //now reload all subsystem
	        std::vector<TA_Base_Core::ISubsystem*> subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(false);

	        // And insert the actionGroups into our map. 
	        for (std::vector<TA_Base_Core::ISubsystem*>::iterator subsystemIter = subsystems.begin();
	             subsystemIter != subsystems.end();
	             ++subsystemIter
	             )
	        {
	             TA_Base_Core::Subsystem* theSubsystem = static_cast<TA_Base_Core::Subsystem*>(*subsystemIter);
	             
			tempKey=theSubsystem->getKey();
			
			m_subsystemMap.insert(RightsType::SubsystemMap::value_type( tempKey, theSubsystem) );
	        }
		//===========debug: check subsystem insert================

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "subsystem size : %lu ",(unsigned long)m_subsystemMap.size());

		//============================================
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "end  init Rights data Cache");
	}

	std::auto_ptr<Context> XacmlContextFactory::build(	const TA_Base_Bus::SessionInfo& sessionId,
														unsigned long subject, 
														unsigned long resource, 
														unsigned long action )
	{
		std::ostringstream sub, res, act;

		sub << subject;
		res << resource;
		act << action;
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin new Context");
		std::auto_ptr<Context> ctx( new Context( sub.str(), res.str(), act.str() ) );


		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end new Context");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getActionAttributes( ctx, action );");
		getActionAttributes( ctx, action ); //thread safe, actionlock
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getActionAttributes( ctx, action );");
		
		if( isPhysicalResource( resource ) ) //threadsafe,resourceLock
		{
			ctx->addResourceAttribute( RightsConst::RESOURCE_TYPE, RightsConst::PHYSICAL );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getSubjectAttributesFromDutyAgent");
			getSubjectAttributesFromDutyAgent(ctx, sessionId, subject, resource); //thread safe, resourceLock, loc-sub-duty lock, actionGroupLock
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getSubjectAttributesFromDutyAgent(");
		}
		else
		{
			ctx->addResourceAttribute( RightsConst::RESOURCE_TYPE, RightsConst::LOGICAL );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getSubjectAttributesFromDatabase(ctx, subject, resource)");
			getSubjectAttributesFromDatabase(ctx, subject, resource); //thread safe, profileLock
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getSubjectAttributesFromDatabase(ctx, subject, resource)");
		}
		return( ctx );
	}

	std::auto_ptr<Context> XacmlContextFactory::build(	const TA_Base_Bus::SessionInfo& sessionId,
														unsigned long subject, unsigned long location, 
														unsigned long subsystem, unsigned long action )
	{
		std::ostringstream sub, res, act;
		std::ostringstream osResource; 

		osResource << "L:" << location <<"--S:" << subsystem;

		sub << subject;
		res << osResource.str();
		act << action;
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin new Context");
		std::auto_ptr<Context> ctx( new Context( sub.str(), res.str(), act.str() ) );
		
		
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end new Context");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getActionAttributes( ctx, action );");
		getActionAttributes( ctx, action ); //thread safe, actionlock
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getActionAttributes( ctx, action );");
		
		if( isPhysicalSubsystem( subsystem ) ) //threadsafe,resourceLock
		{
			ctx->addResourceAttribute( RightsConst::RESOURCE_TYPE, RightsConst::PHYSICAL );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getSubjectAttributesFromDutyAgent");
			getSubjectAttributesFromDutyAgent(ctx, sessionId, subject, location, subsystem); //thread safe, resourceLock, loc-sub-duty lock, actionGroupLock
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getSubjectAttributesFromDutyAgent(");
		}
		else
		{
			ctx->addResourceAttribute( RightsConst::RESOURCE_TYPE, RightsConst::LOGICAL );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@begin getSubjectAttributesFromDatabase(ctx, subject, resource)");
			getSubjectAttributesFromDatabase(ctx, subject, location, subsystem); //thread safe, profileLock
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@end getSubjectAttributesFromDatabase(ctx, subject, resource)");
		}
		return( ctx );
	}

	void XacmlContextFactory::getActionAttributes( std::auto_ptr<Context>& ctx, unsigned long action )
	{
		FUNCTION_ENTRY("getActionAttributes");

		RightsType::ActionMap::iterator it;

		ValueSet* actionVs = NULL;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_actionLock);

		it = m_actionMap.find( action );

		if( it != m_actionMap.end() )
		{
			try
			{
				actionVs = new ValueSet( it->second->getActionGroups() );
			}
			catch( const TA_Base_Core::DatabaseException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", e.what() );

				{
					delete it->second;
					m_actionMap.erase( it );
				}
				TA_THROW(e);
			}
			catch( const TA_Base_Core::DataException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", e.what() );

				{
					delete it->second;
					m_actionMap.erase( it );
				}
				TA_THROW(e);
			}
		}
		else
		{
			std::auto_ptr<TA_Base_Core::IAction> newAction( new TA_Base_Core::Action(action) );
			
			if ( NULL != newAction.get() )
			{
				actionVs = new ValueSet( newAction->getActionGroups() );

				m_actionMap.insert( RightsType::ActionMap::value_type( action, newAction.release() ) );
			}
		}

		if( NULL != actionVs )
		{
			try
			{
				ctx->addActionAttribute( RightsConst::RIGHTS, *actionVs );

			}
			catch( const TA_Base_Core::PolicyDecisionPointException& e )
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::PolicyDecisionPointException", e.what() );
				ctx->addActionAttribute( RightsConst::RIGHTS, INVALID_RIGHTS );
			}
			catch(...)
			{
				delete actionVs;
				throw;
			}
			delete actionVs;
		}

		FUNCTION_EXIT;
	}

	void XacmlContextFactory::getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx, 
																unsigned long subject, 
																unsigned long resource )
	{
		FUNCTION_ENTRY("getSubjectAttributesFromDatabase");

		std::string actionGroup;

		RightsType::ProfileMap::iterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_profileLock);

		it = m_profileMap.find( subject );
		if(  it != m_profileMap.end() )
		{
			try
			{
				actionGroup = it->second->getActionGroupAsStringForResource(resource);				
			}
			catch( const TA_Base_Core::DatabaseException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
			catch( const TA_Base_Core::DataException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
		}
		else
		{
			std::auto_ptr<TA_Base_Core::IProfile> profile( new TA_Base_Core::Profile(subject) );

			actionGroup = profile->getActionGroupAsStringForResource(resource);

			m_profileMap.insert( RightsType::ProfileMap::value_type( subject, profile.release() ) );
		}

		try
		{
			ctx->addSubjectAttribute( RightsConst::RIGHTS, actionGroup );
		}
		catch( const TA_Base_Core::PolicyDecisionPointException& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::PolicyDecisionPointException", e.what() );
			ctx->addSubjectAttribute( RightsConst::RIGHTS, INVALID_RIGHTS );
		}

		FUNCTION_EXIT;
	}

	void XacmlContextFactory::getSubjectAttributesFromDatabase( std::auto_ptr<Context>& ctx, unsigned long subject,
																unsigned long location, unsigned long subsystem )
	{
		FUNCTION_ENTRY("getSubjectAttributesFromDatabase");
		
		std::string actionGroup;
		
		RightsType::ProfileMap::iterator it;
		
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_profileLock);
		
		it = m_profileMap.find( subject );
		if(  it != m_profileMap.end() )
		{
			try
			{
				actionGroup = it->second->getActionGroupAsStringForSubsystem(subsystem,0 /*SS_NORMAL*/);				
			}
			catch( const TA_Base_Core::DatabaseException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
			catch( const TA_Base_Core::DataException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
		}
		else
		{
			std::auto_ptr<TA_Base_Core::IProfile> profile( new TA_Base_Core::Profile(subject) );
			
			actionGroup = profile->getActionGroupAsStringForSubsystem(subsystem,0 /*SS_NORMAL*/);
			
			m_profileMap.insert( RightsType::ProfileMap::value_type( subject, profile.release() ) );
		}
		
		try
		{
			ctx->addSubjectAttribute( RightsConst::RIGHTS, actionGroup );
		}
		catch( const TA_Base_Core::PolicyDecisionPointException& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::PolicyDecisionPointException", e.what() );
			ctx->addSubjectAttribute( RightsConst::RIGHTS, INVALID_RIGHTS );
		}
		
		FUNCTION_EXIT;
	}
	void XacmlContextFactory::getSubjectAttributesFromDutyAgent(
		std::auto_ptr<Context>& ctx, const TA_Base_Bus::SessionInfo& session,
		unsigned long subject, unsigned long resource )
	{
	
		FUNCTION_ENTRY("getSubjectAttributesFromDutyAgent");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@enter getSubjectAttributesFromDutyAgent(ctx, subject, resource)");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@session: %s  subject: %lu  resource: %lu",(session.SessionId).c_str(),subject,resource);
		unsigned long location;
		unsigned long subsystem;

		try
		{
			getLocSub( resource, location, subsystem ); //thread safe, resourceLock
		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Error retrieving location & subsystem for resource" );
			ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
			FUNCTION_EXIT;
			return;
		}
		
		LocSubSesProf lssp(location,subsystem,session.SessionId, subject );
		/*
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA REQ: (LOC:%lu,SUB:%lu,SES:%s,PRF:%lu",
			location,subsystem,session.SessionId.c_str(),subject );
		
		// now we have the location and subsystem, try to find the profile/dutytype pair
		{
			RightsType::LocSubDutyMap::iterator it;

			ACE_Read_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);

			it = m_locSubDutyMap.find( lssp );
			
			if( it != m_locSubDutyMap.end() )
			{
				std::string actionGroup = getActionGroupNameFromCache(it->second); //thread safe, actionGorupLock

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cached: DUTY=%s", actionGroup.c_str() );

				try
				{
					ctx->addSubjectAttribute( RightsConst::DUTY, actionGroup );
				}
				catch( const TA_Base_Core::PolicyDecisionPointException& e )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::PolicyDecisionPointException", e.what() );
					ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
				}

				FUNCTION_EXIT;
				return;
			}
		}
		*/
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@getSubjectAttributesFromDutyAgent: duty not cached, now call duty agent queryHasDuty");
		// at this point we need to consult the duty agent directly
		bool hasDuty = false;

		DutyAgentTypeCorbaDef::Key actionGroup = DutyAgentTypeCorbaDef::INVALID_KEY;
		try
		{
			
			DutyAgentQueryType* dutyAgent = m_dutyagentMap[location].get();
			
			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@CORBA_CALL_RETURN queryHasDuty start");
			CORBA_CALL_RETURN( hasDuty,
			                   (*dutyAgent),
			                   queryHasDuty,
			                   ( corbaSession, location, subsystem, subject, actionGroup ) );
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@CORBA_CALL_RETURN queryHasDuty end");
			if( !hasDuty || DutyAgentTypeCorbaDef::INVALID_KEY == actionGroup )
			{
				{
					ACE_Read_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
					m_locSubDutyMap.insert( RightsType::LocSubDutyMap::value_type(lssp, DutyAgentTypeCorbaDef::INVALID_KEY ) );
				}

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA: NO DUTY" );

				ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
				FUNCTION_EXIT;
				return;
			}

		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Error communicating with Duty Agent" );
			ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
			FUNCTION_EXIT;
			return;
		}

		std::string actionGroupName = getActionGroupNameFromCache( actionGroup ); //thread safe, actionGroupLock

		ACE_Write_Guard<ACE_RW_Mutex> actionGroupGuard(m_locSubDutyLock);

		m_locSubDutyMap.insert( RightsType::LocSubDutyMap::value_type(lssp, actionGroup) );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA: DUTY=%s", actionGroupName.c_str() );

		ctx->addSubjectAttribute( RightsConst::DUTY, actionGroupName );

		FUNCTION_EXIT;
	}

	void XacmlContextFactory::getSubjectAttributesFromDutyAgent(
		std::auto_ptr<Context>& ctx, const TA_Base_Bus::SessionInfo& session,
		unsigned long subject, unsigned long location, unsigned long subsystem )
	{
	
		FUNCTION_ENTRY("getSubjectAttributesFromDutyAgent");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@enter getSubjectAttributesFromDutyAgent(ctx, subject, resource)");
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@session: %s  subject: %lu  location: %lu, subsystem: %lu",
			(session.SessionId).c_str(),subject,location, subsystem);

		LocSubSesProf lssp(location,subsystem,session.SessionId, subject );
		/*
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA REQ: (LOC:%lu,SUB:%lu,SES:%s,PRF:%lu",
			location,subsystem,session.SessionId.c_str(),subject );

		// now we have the location and subsystem, try to find the profile/dutytype pair
		{
			RightsType::LocSubDutyMap::iterator it;

			ACE_Read_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);

			it = m_locSubDutyMap.find( lssp );
			
			if( it != m_locSubDutyMap.end() )
			{
				std::string actionGroup = getActionGroupNameFromCache(it->second); //thread safe, actionGorupLock

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cached: DUTY=%s", actionGroup.c_str() );

				try
				{
					ctx->addSubjectAttribute( RightsConst::DUTY, actionGroup );
				}
				catch( const TA_Base_Core::PolicyDecisionPointException& e )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::PolicyDecisionPointException", e.what() );
					ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
				}

				FUNCTION_EXIT;
				return;
			}
		}
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@getSubjectAttributesFromDutyAgent: duty not cached, now call duty agent queryHasDuty");
		*/
		// at this point we need to consult the duty agent directly
		bool hasDuty = false;

		DutyAgentTypeCorbaDef::Key actionGroup = DutyAgentTypeCorbaDef::INVALID_KEY;
		try
		{
			
			DutyAgentQueryType* dutyAgent = m_dutyagentMap[location].get();
			
			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@CORBA_CALL_RETURN queryHasDuty start");
			CORBA_CALL_RETURN( hasDuty,
			                   (*dutyAgent),
			                   queryHasDuty,
			                   ( corbaSession, location, subsystem, subject, actionGroup ) );
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"@@@@@@@@CORBA_CALL_RETURN queryHasDuty end");
			if( !hasDuty || DutyAgentTypeCorbaDef::INVALID_KEY == actionGroup )
			{
				{
					ACE_Read_Guard<ACE_RW_Mutex> guard(m_locSubDutyLock);
					m_locSubDutyMap.insert( RightsType::LocSubDutyMap::value_type(lssp, DutyAgentTypeCorbaDef::INVALID_KEY ) );
				}

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA: NO DUTY" );

				ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
				FUNCTION_EXIT;
				return;
			}

		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Error communicating with Duty Agent" );
			ctx->addSubjectAttribute( RightsConst::DUTY, INVALID_DUTY );
			FUNCTION_EXIT;
			return;
		}

		std::string actionGroupName = getActionGroupNameFromCache( actionGroup ); //thread safe, actionGroupLock

		ACE_Write_Guard<ACE_RW_Mutex> actionGroupGuard(m_locSubDutyLock);

		m_locSubDutyMap.insert( RightsType::LocSubDutyMap::value_type(lssp, actionGroup) );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "DA: DUTY=%s", actionGroupName.c_str() );

		ctx->addSubjectAttribute( RightsConst::DUTY, actionGroupName );

		FUNCTION_EXIT;
	}

	void XacmlContextFactory::getLocSub( unsigned long resource,
		unsigned long& location, unsigned long& subsystem )
	{
		bool isPhysical = false;

		RightsType::ResourceMap::iterator   resIt;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_resourceLock);

		resIt = m_resourceMap.find( resource );
		
		
		if( resIt == m_resourceMap.end() )
		{
			std::auto_ptr<TA_Base_Core::IEntityData> newResource(
				TA_Base_Core::EntityAccessFactory::getInstance().getEntity( resource ) );

			location     = newResource->getLocation();

			getSubsystem( newResource.get(), subsystem, isPhysical );
			
			m_resourceMap.insert( RightsType::ResourceMap::value_type(resource, newResource.release()) );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@now resourceMap size is : %lu",m_resourceMap.size());
		}
		else
		{
			location     = resIt->second->getLocation();

			getSubsystem( resIt->second, subsystem, isPhysical );
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"@@@@@@@@@@@@@@@@getLocSub--- resouce  found in cache; cache resouce info: %lu location: %lu subsystem: %lu",
								resIt->second,
								location,
								subsystem);
		}
	}

	std::string XacmlContextFactory::getActionGroupNameFromCache( unsigned long actionGroup )
	{
		if( DutyAgentTypeCorbaDef::INVALID_KEY == actionGroup )
		{
			return(INVALID_DUTY);
		}

		std::string rval(INVALID_DUTY);

		RightsType::ActionGroupMap::iterator dutyIt;

		ACE_Write_Guard<ACE_RW_Mutex> actionGroupGuard(m_actionGroupLock);
	
		dutyIt = m_actionGroupMap.find( actionGroup );

		if( dutyIt != m_actionGroupMap.end() )
		{
			rval = dutyIt->second->getName();
		}
		else
		{
			try
			{
				std::auto_ptr<TA_Base_Core::IActionGroup> newActionGroup(
				TA_Base_Core::ActionGroupAccessFactory::getInstance().getActionGroup( actionGroup ) );

				rval = newActionGroup->getName();

				m_actionGroupMap.insert(
					RightsType::ActionGroupMap::value_type(actionGroup, newActionGroup.release()) );
			}
			catch(...)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Failed to retrieve duty name from the database" );
			}
			
		}

		return( rval );
	}

	
	bool XacmlContextFactory::isPhysicalSubsystem( unsigned long subsystem )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_subsystemLock);
		bool result = false;
		RightsType::SubsystemMap::iterator itr =  m_subsystemMap.find(subsystem);
		if(itr != m_subsystemMap.end())
		{
			result =itr->second->isPhysical();
		}
		return result;
	}
	bool XacmlContextFactory::isPhysicalResource( unsigned long resource )
	{
		bool isPhysical = false;
		unsigned long subsystem = DutyAgentTypeCorbaDef::INVALID_KEY;

		// get the resource
		{
			RightsType::ResourceMap::iterator it;

			ACE_Write_Guard<ACE_RW_Mutex> guard(m_resourceLock);

			it = m_resourceMap.find(resource);

			if( it == m_resourceMap.end() )
			{
				std::auto_ptr<TA_Base_Core::IEntityData> newResource(
					TA_Base_Core::EntityAccessFactory::getInstance().getEntity( resource ));

				getSubsystem( newResource.get(), subsystem, isPhysical );

				m_resourceMap.insert( 
					RightsType::ResourceMap::value_type(resource, newResource.release()) );
			}
			else
			{
				getSubsystem( it->second, subsystem, isPhysical );
			}
		}

		return( isPhysical );
	}

	void XacmlContextFactory::getSubsystem( TA_Base_Core::IEntityData* resource, unsigned long& subsystem,
		bool& isPhysical ) const
	{
		TA_ASSERT( NULL != resource, "resource == NULL" );

		unsigned long tmpSub = DutyAgentTypeCorbaDef::INVALID_KEY;

		tmpSub = resource->getPhysicalSubsystem();

		if( 0 == tmpSub )
		{
			subsystem     = resource->getSubsystem();
			isPhysical    = false;
			return;
		}

		subsystem     = resource->getPhysicalSubsystem();
		isPhysical    = true;
	}

	// TODO: This pattern for accessing cached database data should be abstracted. Currently
	// it is copy & pasted throughout the code... :(
	std::string XacmlContextFactory::getProfileNameFromCache( unsigned long subject )
	{
		FUNCTION_ENTRY("getProfileNameFromCache");

		std::string profileName;

		RightsType::ProfileMap::iterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_profileLock);

		it = m_profileMap.find( subject );

		if(  it != m_profileMap.end() )
		{
			try
			{
				profileName = it->second->getName();
			}
			catch( const TA_Base_Core::DatabaseException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DatabaseException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
			catch( const TA_Base_Core::DataException& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::DataException", e.what() );
				
				{
					delete it->second;
					m_profileMap.erase( it );
				}
				TA_THROW(e);
			}
		}
		else
		{
			std::auto_ptr<TA_Base_Core::IProfile> profile( new TA_Base_Core::Profile(subject) );
			profileName = profile->getName();

			m_profileMap.insert( RightsType::ProfileMap::value_type( subject, profile.release() ) );
		}

		return( profileName );
	}

}
