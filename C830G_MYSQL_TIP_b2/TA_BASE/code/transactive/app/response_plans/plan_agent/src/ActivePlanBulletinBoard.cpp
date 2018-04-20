/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanBulletinBoard.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2011/06/21 13:53:41 $
  *  Last modified by:  $Author: qi.huang $
  *
  */


#include "PlanAgent.h"
#include <algorithm>
#include "core/utilities/src/DateFormat.h" // TD12474 

const short NETWORK_DELAY = 30;  //seconds allowance for a network delay -feel free to adjust if proven unsuitable
const std::string DEFAULT_ASSOC_ALARM_ID = "defaultId"; //default value for associated alarm ID - only those plans which raise alarms will 
											//have this value set to the one supplied by alarm agent (unique for unique alarms)

// This needs to exist so we can put ActivePlandId in a std::map

namespace TA_Base_App
{
	//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
	const int ActivePlanBulletinBoardImpl::INDICATION_SESSION_AND_PMID = 36;
	ActivePlanBulletinBoardImpl::ActivePlanBulletinBoardImpl()
	{
        if ( TA_Base_Core::MIN_UPDATE_ID < 1 )
            m_updateId = 0;
        else
			m_updateId = TA_Base_Core::MIN_UPDATE_ID - 1;

        if ( TA_Base_Core::MIN_UPDATE_ID < 1 )
            m_pasUpdateId = 0;
        else
			m_pasUpdateId = TA_Base_Core::MIN_UPDATE_ID - 1;

		m_hangupMessageReceived = false;
		m_isControlMode = false;
		m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::PlanAgentBroadcastComms::Context );
        m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender ( TA_Base_Core::PlanAgentStateUpdate::Context );

		TA_Base_Bus::RightsLibraryFactory rlf;

		//Uncomment the following line and delete the line with ALWAYS_TRUE_RL
#if defined ( DEVELOPMENT_BUILD )
		m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
#else
		m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL);
#endif // DEVELOPMENT_BUILD

        TA_Base_Core::IResource* r = 	TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( PlanAgentUser::getInstance()->getAgentEntityKey() );
        m_resourceId = r->getKey();
        delete r;
	}

	ActivePlanBulletinBoardImpl::~ActivePlanBulletinBoardImpl()
	{
		deleteMessageSenders ();
		deleteRightsLib ();
	}

	void ActivePlanBulletinBoardImpl::deleteMessageSenders( void )
	{
		if (m_commsSender != NULL)
		{
		delete m_commsSender;
		m_commsSender = NULL;
		}
		if (m_stateSender != NULL)
		{
		delete m_stateSender;
		m_stateSender = NULL;
		}
	}

	void ActivePlanBulletinBoardImpl::deleteRightsLib ()
	{
		if (m_rightsLibrary != NULL)
		{
			delete m_rightsLibrary;
			m_rightsLibrary = NULL;
		}
	}

	bool ActivePlanBulletinBoardImpl::wasHangupMessageReceived(void)
	{
		return m_hangupMessageReceived;
	}

	void ActivePlanBulletinBoardImpl::setHangupMessageReceived(void)
	{
		m_hangupMessageReceived = true;
	}

	bool ActivePlanBulletinBoardImpl::isActionPermitted(const std::string& session, unsigned long actionKey, std::string& reason)
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

		try
		{
			return m_rightsLibrary->isActionPermittedOnResource(
									session.c_str(), m_resourceId, actionKey, 
									reason, decisionModule);
		}
        catch (const TA_Base_Core::RightsException& ex)
        {
			reason = "Error: Rights Exception. ";
            reason += ex.what();
			return false;
        }
		catch ( ... )
		{
			reason = "Error: The Rights Library could not determine the access rights.";
			return false;
		}
	}

	//zn++ CL-21150 add method to check concrete resource
	bool ActivePlanBulletinBoardImpl::isActionPermitted(const std::string& session, unsigned long resourceKey, unsigned long actionKey, std::string& reason)
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		
		try
		{
			return m_rightsLibrary->isActionPermittedOnResource(
				session.c_str(), resourceKey, actionKey, 
				reason, decisionModule);
		}
        catch (const TA_Base_Core::RightsException& ex)
        {
			reason = "Error: Rights Exception. ";
            reason += ex.what();
			return false;
        }
		catch ( ... )
		{
			reason = "Error: The Rights Library could not determine the access rights.";
			return false;
		}
	}
	//++zn

    bool ActivePlanBulletinBoardImpl::isControlMode(void)
    {
        return m_isControlMode;
    }

	void ActivePlanBulletinBoardImpl::setControlMode( bool controlMode )
	{
		std::string message;
		if ( controlMode )
		{
			if ( 0 == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate);
			}
			m_isControlMode = true;

			message = "Plan Agent has been put into CONTROL MODE.";
		}
		else
		{
			if ( 0 == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest);
			}
			m_isControlMode = false;
			message = "Plan Agent has been put into MONITOR MODE.";
            // Move state update request to PlanAgentUser::agentSetMonitor
// 			sendStateUpdateRequestToControlAgent();
		}

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
	}

   bool ActivePlanBulletinBoardImpl::isRegisteredManager (const std::string& session, const std::string pmId)
	{
		PMOBJREFMapIterator it;
		std::string compositePmId = session + pmId;
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
		{
			it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				return true;
			}
			else
			{
				return false;
			}

		}
	}

	void ActivePlanBulletinBoardImpl::registerPlanManager(const std::string& session, const std::string& pmId, TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerRef)
	{
		PMOBJREFMapIterator it;
		TA_Base_Bus::IPlanManagerCorbaDef_ptr myRef = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate(planManagerRef);
        std::string compositePmId = session + pmId;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
		{
			it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				CORBA::release((*it).second);
				m_pmObjRef.erase(compositePmId);
			}

			m_pmObjRef.insert(PMOBJREFMap::value_type(compositePmId,myRef));
		}

        //  TD 10849 Reassign all plans for this user to the new Plan Manager's session key
		PlanMapIterator planIt;
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);

		ACE_Write_Guard<ACE_RW_Mutex> planMapGuard(m_planMapLock);
		{
            for (planIt =  m_planMap.begin(); planIt!= m_planMap.end(); planIt++)
            {
                if ( userKey == (*planIt).second.m_userKey )
                {
					try
					{
						if(m_authenticationLib.getSessionInfo(session, session).WorkstationId != 
							m_authenticationLib.getSessionInfo((*planIt).second.m_session, (*planIt).second.m_session).WorkstationId)
						{
							continue;
						}
					}
					catch(...)
					{
						 LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Exception catch when try find workstatinid of session %s and session %s", session.c_str(), (*planIt).second.m_session.c_str());
						 continue;
					}
                    PlanDetail oldDetail = (*planIt).second;
				    (*planIt).second.m_session = session;
				    (*planIt).second.m_pmId = pmId;
		            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Plan [%lu] reassigned to owner %s [SessionId:%s -> %s, PmId:%s -> %s].", (*planIt).second.m_detail.plan.plan, (*planIt).second.m_detail.owner.in(), oldDetail.m_session.c_str(), session.c_str(), oldDetail.m_pmId.c_str(), pmId.c_str() );
                }
            }
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "PlanManager[SessionId:%s, PmId:%s] got registered.", session.c_str(), pmId.c_str() );
		if ( !m_isControlMode )
			return;

		//Send State update to the PlanAgent in monitor mode
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::RegisterPlanManagerUpdate_var registerPMUpdate = new TA_Base_Core::RegisterPlanManagerUpdate;
		registerPMUpdate->session = CORBA::string_dup(session.c_str());
		registerPMUpdate->pmId = CORBA::string_dup(pmId.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		registerPMUpdate->planManager = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate(planManagerRef);
		pasUpdateData->update.planManager(registerPMUpdate);

		try
		{
			sendStateUpdateMessage(pasUpdateData);
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "INFO: StateUpdate for PlanManager[SessionId:%s, PmId:%s] Registration has been sent.", session.c_str(), pmId.c_str());
		}
		catch (...)
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: StateUpdate for PlanManager[SessionId:%s, PmId:%s] Registration could not be sent.", session.c_str(), pmId.c_str());
		}
	}

	void ActivePlanBulletinBoardImpl::deregisterPlanManager(const std::string& session, const std::string& pmId)
	{
		PMOBJREFMapIterator it;

        std::string compositePmId = session + pmId;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
		{
			it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				CORBA::release((*it).second);
				m_pmObjRef.erase(compositePmId);
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Plan Manager[SessionId:%s] got deregistered.", session.c_str() );
			}
			else
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Plan Manager[SessionId:%s] is not registered hence cannot be deregistered.", session.c_str() );
		}

		if ( !m_isControlMode )
			return;

        TA_Base_Core::DeregisterPlanManagerUpdate dpmu;
        dpmu.session = CORBA::string_dup(session.c_str());
        dpmu.pmId = CORBA::string_dup( pmId.c_str() );

		//Send State update to the PlanAgent in monitor mode
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.deregisterPlanManager(dpmu);

		try
		{
			sendStateUpdateMessage(pasUpdateData);
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "INFO: StateUpdate for Plan Manager[SessionId:%s] Deregistration has been sent.", session.c_str());
		}
		catch ( ... )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: StateUpdate for Plan Manager[SessionId:%s] Deregistration could not be sent.", session.c_str());
		}
	}

	void ActivePlanBulletinBoardImpl::getPlanManagerObjRef( const TA_Base_Core::ActivePlanId& plan, TA_Base_Bus::IPlanManagerCorbaDef_ptr& pmCorbaRef)
	{
		pmCorbaRef = NULL;
		PMOBJREFMapIterator it;
		std::string session = getSession( plan );
        std::string pmId = getPlanManagerId(plan);

        std::string compositePmId = session + pmId;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
		{
			it = m_pmObjRef.find( compositePmId );

			if( it != m_pmObjRef.end() )
			{
				pmCorbaRef = (*it).second;
				return;
			}			
		}


        //Could not find PMObjRef for given sessionId and pmId, so search for another PM which has the same sessionId

        std::string compositeId = "";
        for ( it = m_pmObjRef.begin(); it != m_pmObjRef.end(); it++ )
        {
            compositeId = (*it).first;

            std::string::size_type pos = compositeId.find(session);

            if ( pos == 0 )
            {                
                pmCorbaRef = (*it).second;

                int sessionLen = session.length();
                std::string pmId = compositeId.substr(sessionLen, compositeId.length()-sessionLen); 
                setPlanManagerId(plan, pmId);
            }
        }

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Plan Manager reference not found for [SessionId:%s] and Active Plan[P:%u,I:%u]", session.c_str(), plan.plan, plan.instance );
	}

    std::string ActivePlanBulletinBoardImpl::getOperatorName( const TA_Base_Core::ActivePlanId& plan )
    {
        return getOwner(getSession( plan ) );
    }


	std::string ActivePlanBulletinBoardImpl::getOwner( const std::string& idString )
	{
        std::string session = idString;

         std::string::size_type pos = idString.find(PLAN_EDIT_LOCK_SEPARATOR);

        if ( pos != std::string::npos )
        {
            session = idString.substr(0, pos); 
        }

        try 
        {
            // The format is "<operator name> (<base profile>[ / <override profile>])"
            TA_Base_Bus::SessionInfo sessionInfo(m_authenticationLib.getSessionInfo(session, session));
		    std::auto_ptr<TA_Base_Core::IOperator> iOperator(
                TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));

            std::string profile("");
            
            for (std::vector<unsigned long>::const_iterator pIt(sessionInfo.ProfileId.begin());
                 pIt != sessionInfo.ProfileId.end(); pIt++)
            {
		        std::auto_ptr<TA_Base_Core::IProfile> iProfile(
                    TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*pIt, false));
                
                if (!profile.empty())
                {
                    profile += " / ";
                }

                profile += iProfile->getName();
            }

            TA_ASSERT(!profile.empty(), "Session has no valid profile associated with it");

            std::ostringstream opNameAndProfile;
            opNameAndProfile << iOperator->getName() << " (" << profile << ")";

		    return opNameAndProfile.str();
        }
        catch(const TA_Base_Core::AuthenticationSecurityException& ex)
        {
            std::string message = "ActivePlanBulletinBoardImpl::getOwner. Caught TA_Base_Core::AuthenticationSecurityException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            std::string message = "ActivePlanBulletinBoardImpl::getOwner. Caught TA_Base_Core::AuthenticationAgentException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            std::string message = "ActivePlanBulletinBoardImpl::getOwner. Caught TA_Base_Core::ObjectResolutionException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch ( ... )
        {
            std::string message = "Operator name and profile could not be retrieved from session id[";
            message += session;
            message += "]. Constructing default owner from session id.";
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
        }

        std::string owner("session: " + session);

		return owner;
	}

	unsigned long ActivePlanBulletinBoardImpl::getOwnerId( const std::string& idString )
	{
        std::string session = idString;

        std::string::size_type pos = idString.find(PLAN_EDIT_LOCK_SEPARATOR);

        if ( pos != std::string::npos )
        {
            session = idString.substr(0, pos); 
        }

        unsigned long ownerId;
        
        if (getUserKeyFromSession(session, ownerId))
        {
            return ownerId;
        }

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL,
             "Failed to convert session to operator key, returning 0");

		return 0;
	}

    bool ActivePlanBulletinBoardImpl::getUserKeyFromSession( const std::string& session, unsigned long& userKey )
    {
#if defined ( DEVELOPMENT_BUILD )
        return false; // TODO: delete this line before release
#endif //DEVELOPMENT_BUILD

        std::string message = "ActivePlanBulletinBoardImpl::getUserKeyFromSession. Caught " ;

        try
        {
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib.getSessionInfo(session, session);
		    userKey = sessionInfo.OperatorId.at(0);
            return true;
        }
        catch(const TA_Base_Core::AuthenticationSecurityException& ex)
        {
            message += "TA_Base_Core::AuthenticationSecurityException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            message += "TA_Base_Core::AuthenticationAgentException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            message += "TA_Base_Core::ObjectResolutionException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(...)
        {
            message += "Unknown exception";
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }

        return false;
    }

	bool ActivePlanBulletinBoardImpl::isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan)
	{
		std::string sessionId = getSession( plan );

		if ( sessionId.compare(session) == 0 )
			return true;
		try
		{
			if(m_authenticationLib.getSessionInfo(session, session).WorkstationId != 
				m_authenticationLib.getSessionInfo(sessionId, sessionId).WorkstationId)
			{
				return false;
			}
		}
		catch(...)
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, 
				"Exception catch when try find workstatinid of session %s and session %s",
				session.c_str(), sessionId.c_str());
			return false;
		}
        unsigned long userKey = 0xFFFFFFFF;
        if ( getUserKeyFromSession( session, userKey) )
        {
            if ( userKey == getUserKey(plan) )
            {
                setSession( plan, session );
                return true;
            }
        }

		return false;
	}

	unsigned long ActivePlanBulletinBoardImpl::getUserKey( const TA_Base_Core::ActivePlanId& plan )
	{
		ConstPlanMapIterator it;

		unsigned long rval = 0;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = (*it).second.m_userKey;
			}
			else
			{
				std::ostringstream what;
				what << "Plan:[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "getUserKey [P:%lu,I:%lu]: userKey = %lu.", plan.plan, plan.instance, rval );

		return( rval );
	}

    void ActivePlanBulletinBoardImpl::setPlanManagerId( const TA_Base_Core::ActivePlanId& plan, const std::string& pmId )
    {
		PlanMapIterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				(*it).second.m_pmId = pmId;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::setPlanManagerId():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "setPlanManagerId [P:%lu,I:%lu]: Plan Manager ID = %s.", plan.plan, plan.instance, pmId.c_str() );
    }

    void ActivePlanBulletinBoardImpl::setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session )
	{
		PlanMapIterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				(*it).second.m_session = session;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::setSession():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "setSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, session.c_str() );
	}

	std::string ActivePlanBulletinBoardImpl::getSession( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ConstPlanMapIterator it;
		std::string rval = "";

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = (*it).second.m_session;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "getSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, rval.c_str() );

		return( rval );
	}

    std::string ActivePlanBulletinBoardImpl::getPlanManagerId( const TA_Base_Core::ActivePlanId& plan ) const
    {
		ConstPlanMapIterator it;
		std::string rval = "";

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = (*it).second.m_pmId;
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getPlanManagerId():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "getPlanManagerId [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, rval.c_str() );

		return( rval );
    }

	bool ActivePlanBulletinBoardImpl::isActive( const TA_Base_Core::NodeId& plan, const TA_Base_Core::ActivePlanId& ignoreInstance ) const
	{
		bool active = false;

		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan, itLower, itUpper );
			it = itLower;

			while( (false == active) && (itUpper != it) )
			{
				// check that it is not the instance of the plan that we
				// should be ignoring

				if( (*it).second.m_detail.plan.plan != ignoreInstance.plan )
				{
					active = (*it).second.isActive();
				}
				else
				{
					if( (*it).second.m_detail.plan.instance != ignoreInstance.instance )
					{
						active = (*it).second.isActive();
					}
				}

				++it;
	        }
		}

		// No PlanNotFoundException as asking if a non-existant plan is
		// active is a valid question.
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu]: %s active.", plan, (active?"is":"is NOT") );

		return( active );
	}

    bool ActivePlanBulletinBoardImpl::isActive( const TA_Base_Core::ActivePlanId& plan ) const
    {
		bool active = false;
		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan.plan, itLower, itUpper );
			it = itLower;

			while( (false == active) && (itUpper != it) )
			{
				// check that it is not the instance of the plan that we
				// should be ignoring

				if( ((*it).second.m_detail.plan.plan == plan.plan) && ((*it).second.m_detail.plan.instance == plan.instance) )
				{
					active = (*it).second.isActive();
				}

				++it;
	        }
		}

		return( active );
    }
    
    bool ActivePlanBulletinBoardImpl::isAllPlanStopped() const
    {
		bool bActive = true;

        {
		    ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            
            for ( ConstPlanMapIterator itLoop = m_planMap.begin(); m_planMap.end() != itLoop; ++itLoop )
            {
                if ( itLoop->second.isActive() )
                {
                    bActive = false;
                    break;
                }
            }
        }

        return bActive;
    }

	TA_Base_Core::ExecutionHistory* ActivePlanBulletinBoardImpl::getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate)
	{
		ConstPlanMapIterator it;
		ConstExeHistoryMapIterator hIt;
		std::string rval = "";

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;

				what << "ActivePlanBulletinBoardImpl::getPlanExecutionHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		int totalESteps = (*it).second.m_exeHistoryMap.size();

		TA_Base_Core::ExecutionHistory_var history = new TA_Base_Core::ExecutionHistory;
        TA_Base_Core::UpdateId updateId = getNextUpdateId();
		history->activeUpdateId = updateId;
		history->details.length(totalESteps);
		hIt = (*it).second.m_exeHistoryMap.begin();

		for ( int i = 0; i < totalESteps; i++ )
		{
			history->details[i] = (*hIt).second;
			hIt++;
		}

        if ( bSendIdUpdate )
        {
            //Send StateUpdate for change in m_updateId
            sendStateUpdateMessage(updateId);
        }

		return history._retn();		
	}


	TA_Base_Core::CustomisationHistory* ActivePlanBulletinBoardImpl::getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate)
	{
		ConstPlanMapIterator it;
		ConstCustHistoryMapIterator cIt;

		std::string rval = "";

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getPlanCustomisationHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		int totalCSteps = (*it).second.m_custHistoryMap.size();

		TA_Base_Core::CustomisationHistory_var history = new TA_Base_Core::CustomisationHistory;
        TA_Base_Core::UpdateId updateId = getNextUpdateId();
		history->customUpdateId = updateId;
		history->details.length(totalCSteps);
		cIt = (*it).second.m_custHistoryMap.begin();

		for ( int i = 0; i < totalCSteps; i++ )
		{
			history->details[i] = (*cIt).second;
			cIt++;
		}

        if ( bSendIdUpdate )
        {
            //Send StateUpdate for change in m_updateId
            sendStateUpdateMessage( updateId );
        }

		return history._retn();
	}

	ActivePlanList* ActivePlanBulletinBoardImpl::getActiveInstances( const TA_Base_Core::NodeId& plan ) const
	{
		std::auto_ptr<ActivePlanList> instances(new ActivePlanList);
		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan, itLower, itUpper );

			for( it = itLower; it != itUpper; ++it )
			{
				if( (*it).second.isActive() )
				{
					instances->push_back( (*it).second.m_detail.plan );
				}
	        }
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu]: Found %lu instance(s).", plan, instances->size() );

		// No PlanNotFoundException as non-existant plans have zero instances, a
		// valid result.
		return( instances.release() );
	}

    bool ActivePlanBulletinBoardImpl::existsPlan(const TA_Base_Core::ActivePlanId plan)
    {
//Note: This is already being called from SendUpdate which has acquired the lock

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        if ( m_planMap.find(plan) != m_planMap.end() )
        {
            return true;
        }

        return false;
    }


	 bool ActivePlanBulletinBoardImpl::isGRCPlan (const TA_Base_Core::ActivePlanId plan)
	 {
		 std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan);
		 //for GRC plans assumption is they all come from GRC category
		 //this is enforced (to an extent) by displaying only GRC category for 
		 //automatic plan execution (from scheduler and GRC command menu) 
		 std::string rootCategory = planPath.substr (0,5);
		 
		 if (rootCategory.compare ("/GRC/") == 0)
		 {
			 //we want an additional check done here to see if the 
			 //plan contains any BRC steps (to the best of my knowledge 
			 //the only step which is considered BRC at the moment is 
			 //DATA_POINT_SET_STEP simply add more 
			 //cases if more step types turn out to be BRC)
			 TA_Base_Core::StepDetails_var stepDetails = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( plan.plan);
			 for (int i = 0; i < stepDetails->length (); i++)
			 {
				 TA_Base_Core::EStep	stepType = stepDetails [i].type;
				 switch (stepType)
				 {
				 case TA_Base_Core::DATA_POINT_SET_STEP:
					 return true;
				 default:
					 continue;//just keep cheking steps
				 }
			 }
			 
			 //if no BRCs found then this is not a GRC Plan
			 return false;
		 }
		 else
		 {
			 return false;
		 }
	 }
	 
	 
	 bool ActivePlanBulletinBoardImpl::isManualGRC (const TA_Base_Core::ActivePlanId plan)
	 {
		
		 //TD15753--
		 //This thread lock is misplaced causes deadlock on the ActivePlanBulletinBoard
		 //ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		 //--TD15753
		 
		 //first check if this is a GRC Plan
		 if (isGRCPlan (plan))
		 {
			 //the info about manual/auto plan execution state 
			 //is kept in ActivePlanDetail and is set to manual
			 //if plan is executed via runOperatorTriggeredPlan method
			 //of the PlanAgentCorbaDefImpl class
			 
			 return isManualPlan (plan);
		 }
		 else
		 {
			 return false;
		 }
	 }

	 bool ActivePlanBulletinBoardImpl::isManualPlan (const TA_Base_Core::ActivePlanId plan)
	 {
		 ACE_Read_Guard<ACE_RW_Mutex> guard (m_planMapLock);

		 TA_Base_Core::ActivePlanDetail_var actPlanDetail = getActivePlanDetail (plan);
		 return actPlanDetail->manuallyExecuted;
	 }

    TA_Base_Core::ActivePlanDetail*  ActivePlanBulletinBoardImpl::getActivePlanDetail( TA_Base_Core::ActivePlanId plan ) const
    {
		// get the list instances
		ConstPlanMapIterator it;
//Note: This is already being called from SendUpdate which has acquired the lock
//		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
//		{

            if ( (it = m_planMap.find(plan)) == m_planMap.end() ) return NULL;

            TA_Base_Core::ActivePlanDetail_var planDetail = new TA_Base_Core::ActivePlanDetail;
            planDetail->operator=((*it).second.m_detail);
            planDetail->owner = CORBA::string_dup((*it).second.m_session.c_str());

            return planDetail._retn();

//		}
    }


	TA_Base_Core::ActivePlanDetails* ActivePlanBulletinBoardImpl::getActivePlanDetail( TA_Base_Core::NodeId plan ) const
	{
		TA_Base_Core::ActivePlanDetails_var apd = new TA_Base_Core::ActivePlanDetails;

		// get the list instances
		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan, itLower, itUpper );

			PlanMap::difference_type instances = std::distance( itLower, itUpper);

			if( 0 >= instances )
			{
				apd->length(0);
			}
			else
			{
				apd->length(instances);

				int i = 0;

				for( it = itLower; it != itUpper; ++it )
				{
					apd[i] = (*it).second.m_detail;
					++i;
				}
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu]: Found details for %lu instances.", plan, apd->length() );

		// This method does not throw PlanNotFoundException as it is called
		// by the Plan Manager for each plan a user selects (active or not).
		// It is therefore valid to have a zero length list of plan details
		// returned.

		return apd._retn();
	}

	//lizettejl++ Failed Summary Steps (DP-changes)
	TA_Base_Core::FailedStepDetails ActivePlanBulletinBoardImpl::getFailedSteps( TA_Base_Core::ActivePlanId plan )
    {
		TA_Base_Core::FailedStepDetails failedStepDetails;
		
		TA_Base_Core::ExecutionHistory* executionHistory = getPlanExecutionHistory(plan);
		if (NULL != executionHistory)
		{
			std::vector<TA_Base_Core::ActiveStepDetail> stepDetailsList;
			for (CORBA::ULong i = 0, j = 0; i < executionHistory->details.length(); i++)
			{
				TA_Base_Core::ActiveStepDetail stepDetail =  executionHistory->details[i];
				//TD17340++ - compare to the step state, not the step type
				//if (stepDetail.state == TA_Base_Core::FAILURE_SUMMARY_STEP)
				if (stepDetail.state == TA_Base_Core::FAILED_ACTIVE_STEP_STATE)
				{   
					stepDetailsList.push_back(stepDetail);
				}				
			}
			int failedStepsCount = stepDetailsList.size();
			if (failedStepsCount > 0)
			{
				std::string remark;
 				ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
				{
					TA_Base_Core::ActivePlanDetail* planDetail = getActivePlanDetail(plan);
					if (NULL != planDetail)
					{
						 remark = planDetail->remark.in();  
					}
					delete planDetail;
					planDetail = NULL;
				}

				failedStepDetails.length(failedStepsCount);
				
				for ( int i=0; i<failedStepsCount; ++i )
				{
					TA_Base_Core::ActiveStepDetail stepDetail = stepDetailsList[i];
					TA_Base_Core::FailedStepDetail failedStep;
					failedStep.position =  stepDetail.position;
					failedStep.name = CORBA::string_dup(stepDetail.name);
					failedStep.error = CORBA::string_dup(remark.c_str());
					failedStepDetails[i] = failedStep;
				}
			}
			stepDetailsList.clear();
			delete executionHistory;
			executionHistory = NULL;
		}				
		
		//return failedStepDetails._retn();
		return failedStepDetails;
    }
	//++lizettejl
	void ActivePlanBulletinBoardImpl::setAssociatedAlarmId (const TA_Base_Core::ActivePlanId& plan, const char* alarmId)
	{
		PlanMapIterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
	          (*it).second.m_detail.assocAlarmId = CORBA::string_dup (alarmId);
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getState() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}

	TA_Base_Core::ActivePlanDetail* ActivePlanBulletinBoardImpl::getAssocAlarmActivePlanDetail (const char* alarmId)
	{
		TA_Base_Core::ActivePlanDetail_var apd = new TA_Base_Core::ActivePlanDetail;
		ConstPlanMapIterator it;

		for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
		{
			std::string assocAlrmId = CORBA::string_dup ((*it).second.m_detail.assocAlarmId);
			if (assocAlrmId.compare (alarmId) == 0)
			{
				TA_Base_Core::ActivePlanDetail apDetail = (*it).second.m_detail;
				apd = &apDetail;
				return apd._retn(); 
			}
		}
		
		
		return NULL;
	}

	TA_Base_Core::ActivePlanDetails* ActivePlanBulletinBoardImpl::getActivePlanDetail( bool stateUpdate ) const
	{
		TA_Base_Core::ActivePlanDetails_var apd = new TA_Base_Core::ActivePlanDetails;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			if( 0 >= m_planMap.size() )
			{
				apd->length(0);
			}
			else
			{
				ConstPlanMapIterator it;
				apd->length(m_planMap.size());

				int i = 0;

				for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
				{
					apd[i] = (*it).second.m_detail;

                    if ( stateUpdate )
                    {
                        //The owner is replaced by sessionId only when this method is used for stateUpdate
                        apd[i].owner = CORBA::string_dup((*it).second.m_session.c_str());
                    }

					++i;
				}
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Found details for %lu plans/instances.", apd->length() );

		// No PlanNotFoundException as it is valid for there to be no
		// plans running.
		return apd._retn();
	}

	TA_Base_Core::EActivePlanState ActivePlanBulletinBoardImpl::getState( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
                TA_Base_Core::EActivePlanState state = (*it).second.m_detail.activePlanState;
				return( (*it).second.m_detail.activePlanState );
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getState() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}


	ACE_Future<TA_Base_Core::EActivePlanState> ActivePlanBulletinBoardImpl::getFuture( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ConstPlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
				return( (*it).second.m_result );
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getFuture() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}


	EWaitResult ActivePlanBulletinBoardImpl::waitForOperatorResponse( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const
	{
		ACE_Future<TA_Base_Core::EActivePlanState> future = getFuture( plan );

			TA_Base_Core::EActivePlanState result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;
		EWaitResult rval = UNDEFINED_WR;

		if( -1 == future.get( result, &tv ) )
		{
			if( ETIME != ACE_OS::last_error() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::waitForOperatorResponse() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " ACE_Future::get failed with errno: " << ACE_OS::last_error()
					 << ".";

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}
			return( TIMEOUT_WR );
		}

		switch( result )
		{
			case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
			{
				rval = SUCCESS_WR;
			}
			break;

			default:
			{
				rval = FAILURE_WR;

			}
			break;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: wait for completion %s.", plan.plan, plan.instance, ((SUCCESS_WR==rval)?"successful":"failed") );

		return( rval );
	}

	EWaitResult ActivePlanBulletinBoardImpl::waitForCompletion( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const
	{
		ACE_Future<TA_Base_Core::EActivePlanState> future = getFuture( plan );

		TA_Base_Core::EActivePlanState result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;
		EWaitResult rval = UNDEFINED_WR;

		if( -1 == future.get( result, &tv ) )
		{
			if( ETIME != ACE_OS::last_error() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::waitForCompletion() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " ACE_Future::get failed with errno: " << ACE_OS::last_error()
					 << ".";

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}

			return( TIMEOUT_WR );
		}

		switch( result )
		{
			case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
			{
				rval = SUCCESS_WR;
			}
			break;

			default:
			{
				rval = FAILURE_WR;
			}
			break;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: wait for completion %s.", plan.plan, plan.instance, ((SUCCESS_WR==rval)?"successful":"failed") );

		return( rval );
	}


	EWaitResult ActivePlanBulletinBoardImpl::waitForTermination( TA_Base_Core::NodeId plan, ACE_Time_Value& tv ) const
	{
		ConstPlanMapIterator itLower;
		ConstPlanMapIterator itUpper;
		ConstPlanMapIterator it;

		TA_Base_Core::EActivePlanState result = TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE;
		EWaitResult rval = UNDEFINED_WR;

		// Would have preferred to make a list of all the ACE_Future objects
		// but they are not stl::list friendly.

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			getInstances( plan, itLower, itUpper );
			it = itLower;

			while( (rval != TIMEOUT_WR) && (it != itUpper) )
			{
				if( -1 == (*it).second.m_result.get( result, &tv ) )
				{
					if( ETIME != ACE_OS::last_error() )
					{
						std::ostringstream what;
						what << "ActivePlanBulletinBoardImpl::waitForTermination() : ACE_Future::get failed with errno: " << ACE_OS::last_error()
							 << ".";

						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
						TA_THROW( ThirdPartyLibException( what.str() ) );
					}
					else
					{
						rval = TIMEOUT_WR;
					}
				}
				else
				{
					rval = SUCCESS_WR;
				}

				++it;
			}
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu]: wait for termination %s.", plan, ((SUCCESS_WR==rval)?"successful":"timed out") );
		return( rval );
	}

	void ActivePlanBulletinBoardImpl::getInstances( const TA_Base_Core::NodeId& plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const
	{
		TA_Base_Core::ActivePlanId idLower;
		TA_Base_Core::ActivePlanId idUpper;

		idLower.plan = plan;
		idUpper.plan = plan;
		idLower.instance = 1;
		idUpper.instance = std::numeric_limits<CORBA::ULong>::max();

		// get the bounds for all instances of the
		// given plan
		itLower = m_planMap.lower_bound( idLower );
		itUpper = m_planMap.upper_bound( idUpper );
	}

    unsigned long ActivePlanBulletinBoardImpl::getFirstAvailableInstanceNumber(const TA_Base_Core::NodeId& plan)
    {
        // TD10997: There can only be so many plan instances executing at any given time.
        //          Do not allow the specified plan to be instantiated if the active plan
        //          limit has been reached. 
        //
        //    Note: We should be able to remove this restriction once we are able to properly
        //          publish and control instances that are queued and waiting to start 
        //          execution (see defect comments in TD).

        unsigned long activeInstanceCount(0);

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        for (ConstPlanMapIterator it(m_planMap.begin()); it != m_planMap.end(); it++)
        {
            if (it->second.isActive())
            {
                activeInstanceCount++;
            }
        }

        if (activeInstanceCount >= PlanAgentUser::getInstance()->getActivePlanThreadPoolSize())
        {
            std::ostringstream what;
			what << "The number of currently active instances has reached the servicable limit of "
                 << PlanAgentUser::getInstance()->getActivePlanThreadPoolSize() << ". No new instances can be created.";

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, what.str().c_str());
            TA_THROW( TA_Base_Core::PlanAgentException( what.str(), TA_Base_Core::PAE_NO_RESOURCE) );
        }

        // Find the first free instance number for the given plan. Start with 1 and work upwards.
        TA_Base_Core::ActivePlanId activePlanId;
        activePlanId.plan = plan;
        activePlanId.instance = 1;

        while (activePlanId.instance < std::numeric_limits<CORBA::ULong>::max())
        {
            if (m_planMap.find(activePlanId) == m_planMap.end())
            {
                return activePlanId.instance;
            }

            activePlanId.instance++;
        }

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "No space left to store plan information");
        TA_THROW( TA_Base_Core::PlanAgentException( "No space left to store plan information", TA_Base_Core::PAE_NO_RESOURCE) );

        return 0; // pacify the compiler
    }

	//marvin++
	//TD13527
	ActivePlanBulletinBoardImpl::EntityPlanRestrictionMultiMapIterator ActivePlanBulletinBoardImpl::findEntityPlanFromMultimap(const TA_Base_Core::ActivePlanId& plan)
	{
		ACE_Read_Guard<ACE_RW_Mutex> entityPlanRestrictionMapWriteGuard(m_entityPlanRestrictionMultiMapGuard);
		return std::find(m_entityPlanRestrictionMultiMap.begin(), m_entityPlanRestrictionMultiMap.end(), EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));
	}
	//++marvin
	//TD13527

    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
												  TA_Base_Core::EActivePlanState initialState, const ACE_Future<TA_Base_Core::EActivePlanState>& planResult, bool manExec)
	{
		std::string owner = getOwner(session);
		std::string remark = (initialState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE? "Customisation in progress" : "Plan started");

		//marvin++
		//TD13527
		if(plan.entitykey != -1)
		{
			//enforce one running plan per entity when entitykey is not empty
			EntityPlanRestrictionMultiMapIterator value = findEntityPlanFromMultimap(plan);
			if(value != m_entityPlanRestrictionMultiMap.end())
			{
				//throw plan agent exception
				std::ostringstream planException;
				planException << "Entity " << plan.entitykey << "is restricted to running only a single instance of a plan" 
					<< "Currently running plan id " << value->second;

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, planException.str().c_str());
				TA_THROW( TA_Base_Core::PlanAgentException( planException.str(), TA_Base_Core::PAE_EXCEEDED_RUNNING_PLANS_FOR_ENTITY) );
			}

			ACE_Write_Guard<ACE_RW_Mutex> entityPlanRestrictionMapWriteGuard(m_entityPlanRestrictionMultiMapGuard);
			m_entityPlanRestrictionMultiMap.insert(EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));
		}
		//marvin++
		//TD13527

        plan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);

        //this is to be filled in latter whith an appropriate value
        //if an alarm is raised by this plan
        std::string assocAlarmId (DEFAULT_ASSOC_ALARM_ID);//default value

		PlanDetail newDetail(session, pmId, userKey, getNextUpdateId(), owner, plan.plan, initialState, remark, planResult, manExec, assocAlarmId);

        // TD10997: Find the first free instance for the given plan. This also ensures that 
        //          the maximum number of active instances for all plans does not get exceeded.
        if (m_isControlMode)
        {
		    plan.instance = getFirstAvailableInstanceNumber(plan.plan);
        }

		// Keep a copy of the instance in the detail, otherwise we would have to set it 
        // each time we retrieved it from the map.
        newDetail.set(plan);

        // Try adding the new plan instance to the plan map.
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(plan, newDetail) );

		if( false == insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << plan.plan << ",I:" << plan.instance << "]: ";
			what << "STL Map insertion failed";
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: instance reserved.", plan.plan, plan.instance );

        return true;
	}


    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, TA_Base_Core::EActivePlanState initialState, const ACE_Future<TA_Base_Core::EActivePlanState>& planResult )
	{
		std::string owner = getOwner(session);
		std::ostringstream remark;
        remark << "Plan spawned by '" 
               << TA_Base_Bus::PlanDataReader::instance()->getNodePath(parentPlan.plan) 
               << "' instance " << parentPlan.instance;

        childPlan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        getUserKeyFromSession( session, userKey);
		  
		  //this is to be filled in latter whith an appropriate value
		  //if an alarm is raised by this plan
		  std::string assocAlarmId (DEFAULT_ASSOC_ALARM_ID);//default value

		PlanDetail newDetail(session, pmId, userKey, getNextUpdateId(), owner, childPlan.plan, initialState, remark.str(), planResult, false, assocAlarmId);//not sure about second last param

        // TD10997: Find the first free instance for the child plan. This also ensures that 
        //          the maximum number of active instances for all plans does not get exceeded.
        if (m_isControlMode)
        {
		    childPlan.instance = getFirstAvailableInstanceNumber(childPlan.plan);
        }

		// Keep a copy of the instance in the detail, otherwise we would have to set it 
        // each time we retrieved it from the map.
        newDetail.set(childPlan);

        // Try adding the new plan instance to the plan map.
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(childPlan, newDetail) );

		if( false == insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << childPlan.plan << ",I:" << childPlan.instance << "]: ";
			what << "STL Map insertion failed";
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: instance reserved.", childPlan.plan, childPlan.instance );

        return true;
	}


    void ActivePlanBulletinBoardImpl::undoReserveInstance( TA_Base_Core::ActivePlanId plan )
    {
        PlanMapIterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

            if ( it != m_planMap.end() )
            {
                m_planMap.erase(it);
            }
		}
    }

    void ActivePlanBulletinBoardImpl::sendUpdateMessage(const TA_Base_Core::ActivePlanId plan)
    {
		PlanMapIterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
                sendUpdateMessage( (*it).second );
            }
        }
    }


	void ActivePlanBulletinBoardImpl::releaseInstance( const TA_Base_Core::ActivePlanId& plan )
	{
		PlanMap::size_type numRemoved = 0;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{	
			 numRemoved = m_planMap.erase( plan );
		}

		if( 1 != numRemoved )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::releaseInstance() : [P:" << plan.plan << ",I:" << plan.instance
				 << "]: Not in Bulletin Board or not released";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: %d instance released.", plan.plan, plan.instance, numRemoved );
	}

	void ActivePlanBulletinBoardImpl::reassignInstance ( const std::string& session, TA_Base_Core::ActivePlanId& plan)
	{

	}

    void ActivePlanBulletinBoardImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
		PlanMapIterator it;

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;
                int historySteps = historyUpdate.history.details.length();

                for (int i=0; i < historySteps; i++)
                {
					CustHistoryMapIterator chIt;
                    const TA_Base_Core::StepCustomisableDetail* custDetail = &(historyUpdate.history.details[i]);
                    TA_Base_Core::StepNumber step = custDetail->position;

					if ( (chIt = (*it).second.m_custHistoryMap.find(step)) != (*it).second.m_custHistoryMap.end() )
					{
                        (*chIt).second.position = custDetail->position;
                        (*chIt).second.skip = custDetail->skip;
						(*chIt).second.delay = custDetail->delay;
					}
					else
					{
						(*it).second.m_custHistoryMap.insert(CustHistoryMap::value_type(step, *custDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.customUpdateId);

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::processCustomisationHistoryUpdate() : [P:" << historyUpdate.plan.plan << ",I:" << historyUpdate.plan.instance << "]:"
				 << " Not found in bulletin board.";

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
		}
    }

    void ActivePlanBulletinBoardImpl::processExecutionHistoryUpdate( TA_Base_Core::ExecutionHistoryUpdate& historyUpdate )
    {
		PlanMapIterator it;

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;
                int historySteps = historyUpdate.history.details.length();

                for (int i=0; i < historySteps; i++)
                {
					ExeHistoryMapIterator ehIt;
                    TA_Base_Core::ActiveStepDetail* stepDetail = &(historyUpdate.history.details[i]);

                    TA_Base_Core::StepNumber step = stepDetail->position;

					if ( (ehIt = (*it).second.m_exeHistoryMap.find(step)) != (*it).second.m_exeHistoryMap.end() )
					{
                        (*ehIt).second.position = stepDetail->position;
                        (*ehIt).second.name = stepDetail->name;
						(*ehIt).second.state = stepDetail->state;
					}
					else
					{
						(*it).second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(step, *stepDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.activeUpdateId);

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::processExecutionHistoryUpdate() : [P:" << historyUpdate.plan.plan << ",I:" << historyUpdate.plan.instance << "]:"
				 << " Not found in bulletin board.";

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, what.str().c_str());
		}
    }

	void ActivePlanBulletinBoardImpl::update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, const std::string& stepName, const std::string remark )
    {
        PlanMapIterator it;

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;
				if( step != (*it).second.m_detail.currentStepDetail.position ||
					stepName != (*it).second.m_detail.currentStepDetail.name.in() ||
					TA_Base_Core::LOADED_ACTIVE_STEP_STATE != (*it).second.m_detail.currentStepDetail.state )
				{
					(*it).second.m_detail.currentStepDetail.position = step;
					(*it).second.m_detail.currentStepDetail.name = CORBA::string_dup( stepName.c_str() );
					(*it).second.m_detail.currentStepDetail.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
					(*it).second.m_detail.remark = CORBA::string_dup(remark.c_str());
					(*it).second.m_detail.activeUpdateId = getNextUpdateId();
					(*it).second.m_timeOut = getStepTimeout (plan, step);
					(*it).second.m_stepStarted = ACE_OS::gettimeofday();
					sendUpdateMessage( (*it).second.m_detail );

					ExeHistoryMapIterator ehIt;

					if ( (ehIt = (*it).second.m_exeHistoryMap.find(step)) != (*it).second.m_exeHistoryMap.end() )
					{
						(*ehIt).second.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
					}
					else
					{
						TA_Base_Core::ActiveStepDetail detail;
						detail.position = step;
						detail.name = CORBA::string_dup( stepName.c_str() );
						detail.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

						(*it).second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(step, detail));
					}
				}
			}
		}

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu,S:%lu]: loaded.", plan.plan, plan.instance, step );
		}
	}

	//marvin++
	//TD13527
	void ActivePlanBulletinBoardImpl::deleteEntityPlanMapRestrictionIfApplicable(const TA_Base_Core::ActivePlanId& plan,TA_Base_Core::EActivePlanState state)
	{
		if ( state == TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE ||
			state == TA_Base_Core::FAILED_ACTIVE_PLAN_STATE)
		{
			//clear map related to entity restrictions once plan has finished
			if(plan.entitykey != -1)
			{
				ACE_Write_Guard<ACE_RW_Mutex> entityPlanRestrictionMapWriteGuard(m_entityPlanRestrictionMultiMapGuard);
				EntityPlanRestrictionMultiMapIterator value = std::find(m_entityPlanRestrictionMultiMap.begin(), m_entityPlanRestrictionMultiMap.end(), EntityPlanRestrictionMultiMapValue(plan.entitykey, plan.plan));

				if(value != m_entityPlanRestrictionMultiMap.end())
				{
					m_entityPlanRestrictionMultiMap.erase(value);
				}
				else
				{
					std::ostringstream planWarn;
					planWarn << "Entity and plan id pair not found on m_entityPlanRestrictionMultiMap deletion failed for entity " << plan.entitykey << " restricted to running a single instance of plan " << value->second;
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, planWarn.str().c_str());
				}
			}
		}
	}
	//++marvin
	//TD13527

	void ActivePlanBulletinBoardImpl::update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, TA_Base_Core::EActiveStepState state, const std::string remark )
    {
        PlanMapIterator it;

		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;
				if(( state != (*it).second.m_detail.currentStepDetail.state ) ||
					( step  != (*it).second.m_detail.currentStepDetail.position ) ||
                    ( 0 != remark.compare( (*it).second.m_detail.remark.in() )))
				{
					(*it).second.m_detail.currentStepDetail.position = step;
					(*it).second.m_detail.currentStepDetail.state   = state;
                    (*it).second.m_detail.remark = CORBA::string_dup(remark.c_str());

					if ( TA_Base_Core::PAUSED_ACTIVE_STEP_STATE == state )
					{
						(*it).second.set( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE );
					}

					(*it).second.m_detail.activeUpdateId = getNextUpdateId();
					sendUpdateMessage( (*it).second.m_detail );

					ExeHistoryMapIterator ehIt;

					if ( (ehIt = (*it).second.m_exeHistoryMap.find(step)) != (*it).second.m_exeHistoryMap.end() )
					{
						(*ehIt).second.state = state;
					}
					else
					{
						TA_Base_Core::ActiveStepDetail detail;
						detail.position = step;
						detail.name = CORBA::string_dup((*it).second.m_detail.currentStepDetail.name.in());
						detail.state = state;

						(*it).second.m_exeHistoryMap.insert(ExeHistoryMap::value_type(step, detail));
					}
				}
			}
		}

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu,S:%lu]: state is %lu.", plan.plan, plan.instance, step, state );
		}
	}

	void ActivePlanBulletinBoardImpl::update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EActivePlanState state, const std::string remark )
	{
		PlanMapIterator it;

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false;

		//marvin++
		//TD13527
		deleteEntityPlanMapRestrictionIfApplicable(plan,state);
		//++marvin
		//TD13527

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;
                TA_Base_Core::EActivePlanState oldState = (*it).second.m_detail.activePlanState;

				if( (*it).second.set( state ) )
				{
					(*it).second.m_detail.activeUpdateId = getNextUpdateId();
					(*it).second.m_detail.remark = CORBA::string_dup(remark.c_str());

                    if ( state == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE )
                    {
                        // Do not change step state here
//                         (*it).second.m_detail.currentStepDetail.state = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
                    }
                    else if ( (state == TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE) &&
                              (oldState == TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE) )
                    {
                        (*it).second.m_detail.currentStepDetail.state = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;
                    }
					sendUpdateMessage( (*it).second.m_detail );

// 					if ( !((*it).second.isNonActive()) ) return;
                    
                    //Plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE
                    if ( it->second.isNonActive() )
                    {
                        ExeHistoryMapIterator itHisLoop = it->second.m_exeHistoryMap.begin();
                        while ( it->second.m_exeHistoryMap.end() != itHisLoop )
                        {
                            TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;

                            if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                                ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                            {
                                it->second.m_exeHistoryMap.erase( itHisLoop++ );
                            }
                            else
                            {
                                ++itHisLoop;
                            }
                        }
                    }
				}
			}
		}

		
		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: state is %lu.", plan.plan, plan.instance, state );
		}
	}

    void ActivePlanBulletinBoardImpl::update( const TA_Base_Core::ActivePlanId& plan, const std::string& session, const std::string& pmId )
	{
		//used only for change of owner 
		PlanMapIterator it;

		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;

				if( session != (*it).second.m_session )
				{
					TA_Base_Core::EActivePlanState currentState = (*it).second.m_detail.activePlanState;
                    (*it).second.m_detail.activeUpdateId = getNextUpdateId();
                    (*it).second.m_session = session;
                    (*it).second.m_pmId = pmId;
                    unsigned long userKey = 0xFFFFFFFF;
                    getUserKeyFromSession( session, userKey );
                    (*it).second.m_userKey = userKey;
                    std::string owner = getOwner(session);
                    (*it).second.m_detail.owner = CORBA::string_dup( owner.c_str() );
                    std::string remark = owner;
                    remark += " took control of the Plan";
					(*it).second.m_detail.remark = CORBA::string_dup( remark.c_str() );
					(*it).second.m_detail.activePlanState = TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE;

                    // Actually, the session id and plan manager id could not be sync to monitor agent by this update
					sendUpdateMessage( (*it).second.m_detail );

					(*it).second.m_detail.activePlanState = currentState;
				}				
			}
		}

		if( !foundInstance )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}
		else
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu]: SessionId is %s.", plan.plan, plan.instance, session.c_str() );
		}
	}

	void ActivePlanBulletinBoardImpl::update( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail )
	{
		PlanMapIterator it;
		bool foundInstance = false; 

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{				
				CustHistoryMapIterator chIt;

				if ( (chIt = (*it).second.m_custHistoryMap.find(detail.position)) != (*it).second.m_custHistoryMap.end() )
				{
					(*chIt).second.position = detail.position;
					(*chIt).second.skip = detail.skip;
					(*chIt).second.delay = detail.delay;
				}
				else
				{					
					(*it).second.m_custHistoryMap.insert(CustHistoryMap::value_type(detail.position, detail));
				}

				TA_Base_Core::PlanCustomisationUpdate_var updateData = new TA_Base_Core::PlanCustomisationUpdate;
				updateData->plan = plan;
                TA_Base_Core::UpdateId updateId = getNextUpdateId();
				updateData->customUpdateId = updateId;
				updateData->detail = detail;

				sendCustomUpdateMessage( updateData );

                //TODO: Send StateUpdate for change in m_updateId
                sendStateUpdateMessage( updateId );
			}
		}
	}
    
    void ActivePlanBulletinBoardImpl::updateStepStartTime( const TA_Base_Core::ActivePlanId& plan, const ACE_Time_Value& stepStartTime )
    {
        {
            // life cycle start for the plan map lock
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            PlanMapIterator itFound = m_planMap.find( plan );

            if( itFound  != m_planMap.end() )
            {
                itFound->second.m_stepStarted = ACE_OS::gettimeofday();
            }
            else
            {
                std::ostringstream what;
                what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:"
				 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
            }
            // life cycle end for the plan map lock
        }
    }

    void ActivePlanBulletinBoardImpl::cullStoppedPlan(TA_Base_Core::ActivePlanId plan)
    {
        std::string remark = "Plan expired";

		ACE_Guard<ACE_Mutex> guard(m_cullLock);
		{
		    update( plan, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, remark );
		    releaseInstance( plan );
		}
    }

    void ActivePlanBulletinBoardImpl::cullAllPlans( void )
    {
		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.

		ACE_Guard<ACE_Mutex> guard(m_cullLock);
		{
			std::list<TA_Base_Core::ActivePlanId> plansToCull;

			// make sure the read lock goes out of scope before
			// we try to take the write lock when erasing the old
			// plans
			{
				PlanMapIterator it;	

				ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

				for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
				{
				    plansToCull.push_back( (*it).second.m_detail.plan );
				}
			}

			if( plansToCull.size() > 0 )
			{
				std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

				for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
				{
					std::string remark = "Plan expired";	
					update( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, remark );
					releaseInstance( *cullIt );
				}
			}
		}
    }

	void ActivePlanBulletinBoardImpl::cullStoppedPlans( void )
	{
        if ( !m_isControlMode )
            return;

		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.

		ACE_Guard<ACE_Mutex> guard(m_cullLock);
		{
			std::list<TA_Base_Core::ActivePlanId> plansToCull;

			// make sure the read lock goes out of scope before
			// we try to take the write lock when erasing the old
			// plans
			{
				PlanMapIterator it;	

				ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

				for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
				{
					if( (*it).second.isNonActive() )
					{
						ACE_Time_Value diff(ACE_OS::gettimeofday() - (*it).second.m_lastModified);

                        if( diff.sec() > PlanAgentUser::getInstance()->getActivePlanExpiryTime() )
						{
							plansToCull.push_back( (*it).second.m_detail.plan );
						}
					}
				}
			}

			if( plansToCull.size() > 0 )
			{
				std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

				for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
				{
					std::string remark = "Plan expired";	
					update( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, remark );
					releaseInstance( *cullIt );
				}
			}
		}
	}

	void ActivePlanBulletinBoardImpl::removeExpiredActivePlans(void)
	{
		//ACE_Guard <ACE_Mutex> guard(m_expiredLock);
		{
			std::list<TA_Base_Core::ActivePlanId> planToRemove;
			PlanMapIterator it;

			ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			for ( it = m_planMap.begin(); it != m_planMap.end(); ++it )
			{

			
			}

		}
	}


	void ActivePlanBulletinBoardImpl::pauseExpiredInteractivePlans( void )
	{

		 if ( !m_isControlMode )
            return;

		// Make sure only one thread tries to pause the expired plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.

		ACE_Guard<ACE_Mutex> guard(m_cullLock);
		{
			std::list<TA_Base_Core::ActivePlanId> plansToPause;

			// make sure the read lock goes out of scope before
			// we try to take the write lock when erasing the old
			// plans
			{
				PlanMapIterator it;	

				ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

				for( it = m_planMap.begin(); it != m_planMap.end(); ++it )
				{
					//plans with non-interactive steps will have 0 timeout
					//so to shorten time spend in the lock test this first
					if  (!((*it).second.m_timeOut > ACE_Time_Value::zero))
                    {
						break;
                    }

					//only interested in active plans
					if(( (*it).second.m_detail.activePlanState == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE ) || 
                       ( (*it).second.m_detail.activePlanState == TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE ))
					{
						ACE_Time_Value diff(ACE_OS::gettimeofday() - (*it).second.m_stepStarted);
						
						if (diff.sec() > (*it).second.m_timeOut.sec())
						{
							plansToPause.push_back( (*it).second.m_detail.plan );
						}
					}
				}
			}

			if( plansToPause.size() > 0 )
			{
				
				std::list<TA_Base_Core::ActivePlanId>::iterator pauseIt;

				for( pauseIt = plansToPause.begin(); pauseIt != plansToPause.end(); ++pauseIt )
				{
					std::string remark = "Plan paused as it timed out - possible network failure";	
					update( *pauseIt, TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE, remark);
				}
			}
		}
	}

	void ActivePlanBulletinBoardImpl::sendUpdateMessage( const PlanDetail& detail )
	{
		if ( !m_isControlMode )
			return;

        TA_Base_Core::UpdateId id = detail.m_detail.activeUpdateId;
        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "**PlanDetail UpdateId=%u.", id);

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();
        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

		CORBA::Any commsData;

		commsData <<= detail.m_detail;

		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate,
										 commsData,
                                         &filterData );
		//Send StateUpdate to the monitor PlanAgent
        try
        {
            TA_Base_Core::ActivePlanDetail_var planDetail = getActivePlanDetail(detail.m_detail.plan);
            sendStateUpdateMessage(planDetail);
        }
        catch ( ... )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: StateUpdate message with ActivePlanDetail could not be sent.");
        }
	}

	void ActivePlanBulletinBoardImpl::sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData )
	{
		if ( !m_isControlMode )
			return;

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;

        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

		CORBA::Any commsData;
		commsData <<= configData;

		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::PlanConfigUpdate,
										 commsData,
                                         &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData )
	{
		if ( !m_isControlMode )
			return;

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);


		CORBA::Any commsData;
		commsData <<= updateData;

		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::PlanCustomisationUpdate,
										 commsData,
                                         &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData )
	{
		/**
		* sendStateUpdateMessage
		*
		* Used to send state update information to a Monitor or Control agent instance.
		* 
		* @param    messageType    	The specific type of the message being sent
		* @param    entityKey    	The entity will receive this update.
		* @param    agentName    	The specific name of the agent that will receive the update
		* @param    updateInfo    	The update information that will update the monitor agent.
		* @param    filterableData  Any additional data a subscriber may filter upon
		*
		void sendStateUpdateMessage( const MessageType& messageType,
									 unsigned long entityKey,
								     const std::string& agentName,
								     const CORBA::Any& updateInfo,
								     const FilterData* filterableData = NULL );*/

		if ( !m_isControlMode )
			return;

        stateUpdateData.pasUpdateId = getNextPasUpdateId();

        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Msg-: PAS Update message sent ( update id: %u ), ( type: %u )", stateUpdateData.pasUpdateId, stateUpdateData.update._d() );

		CORBA::Any stateData;
		stateData <<= stateUpdateData;

		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
                                                PlanAgentUser::getInstance()->getAgentEntityName(),
												stateData,
												NULL
											 );
    }

    // Pack all states update message to only one message to avoid message lost when Monitor Agent startup
    void ActivePlanBulletinBoardImpl::sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq )
    {
        if ( !m_isControlMode )
            return;

        for ( int nSeqLoop = 0; nSeqLoop < refStateUpdateDataSeq.length(); ++nSeqLoop )
        {
            refStateUpdateDataSeq[nSeqLoop].pasUpdateId = getNextPasUpdateId();
            LOG_GENERIC( SOURCE_INFO, DEBUG_LOG_LEVEL, "-Msg-: Generate update id for all update message ( update id: %u ), ( type: %u )", 
                refStateUpdateDataSeq[nSeqLoop].pasUpdateId, refStateUpdateDataSeq[nSeqLoop].update._d() );
        }

        CORBA::Any stateData;
        stateData <<= refStateUpdateDataSeq;

        m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
                                                PlanAgentUser::getInstance()->getAgentEntityName(),
                                                stateData,
                                                NULL );
    }

    void ActivePlanBulletinBoardImpl::sendOperatorPausedStateUpdate( const TA_Base_Core::ActivePlanId plan )
    {
		ConstPlanMapIterator it;
        TA_Base_Core::ActivePlanDetail planDetail;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
                planDetail = (*it).second.m_detail;
			}
		}

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.operatorPausedPlanDetail(planDetail);
        sendStateUpdateMessage(pasUpdateData);

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "INFO: StateUpdate sent for PausedForOperator state");
    }

	void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( const TA_Base_Core::ActivePlanDetail& planDetail )
	{
		/**
		* sendStateUpdateMessage
		*
		* Used to send state update information to a Monitor or Control agent instance.
		* 
		* @param    messageType    	The specific type of the message being sent
		* @param    entityKey    	The entity will receive this update.
		* @param    agentName    	The specific name of the agent that will receive the update
		* @param    updateInfo    	The update information that will update the monitor agent.
		* @param    filterableData  Any additional data a subscriber may filter upon
		*
		
		void sendStateUpdateMessage( const MessageType& messageType,
									 unsigned long entityKey,
								     const std::string& agentName,
								     const CORBA::Any& updateInfo,
								     const FilterData* filterableData = NULL );*/
		if ( !m_isControlMode )
			return;

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.planDetail(planDetail);
        sendStateUpdateMessage(pasUpdateData);

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "INFO: StateUpdate sent for activePlanDetail: Plan[%u:%u]", planDetail.plan.plan, planDetail.plan.instance);
	}

    void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( const TA_Base_Core::UpdateId updateId )
    {
		if ( !m_isControlMode )
			return;

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.activeUpdateId(updateId);
        sendStateUpdateMessage(pasUpdateData);

        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "INFO: StateUpdate sent for activeUpdateId.");
    }

	void ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent(void)
	{
        //Send update for registered PlanManagers
		PMOBJREFMapIterator it;

        std::vector<TA_Base_Core::PASUpdateData*> vecPasUpdateDatas;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_pmObjMapLock);
		{
			std::string strSessionID = "";
			std::string strPmID = "";
            for ( it = m_pmObjRef.begin(); it != m_pmObjRef.end(); it++)
            {
				//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
				if (false != getSessionIDAndPmID((*it).first, strSessionID, strPmID))
				{
					//Send State update to the PlanAgent in monitor mode
					TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData;
                    vecPasUpdateDatas.push_back( pasUpdateData );

					pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
					TA_Base_Core::RegisterPlanManagerUpdate_var registerPMUpdate = new TA_Base_Core::RegisterPlanManagerUpdate;
					registerPMUpdate->session = CORBA::string_dup(strSessionID.c_str());
					registerPMUpdate->pmId = CORBA::string_dup(strPmID.c_str());					
					registerPMUpdate->planManager = TA_Base_Bus::IPlanManagerCorbaDef::_duplicate((*it).second);
					pasUpdateData->update.planManager(registerPMUpdate);

//                     sendStateUpdateMessage(pasUpdateData);
				}
				else
				{
					LOG_GENERIC(SOURCE_INFO, ERROR_LOG_LEVEL, "Invalid Composite session string [%s]", (*it).first.c_str());
				}
			}
		}

        //ActivePlanDetail update
        TA_Base_Core::ActivePlanDetails_var pAPD = getActivePlanDetail(true);

        int nPlans = pAPD->length();

        for (int p=0; p<nPlans; p++)
        {
            TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData;
            vecPasUpdateDatas.push_back( pasUpdateData );

            TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
            psu->session = CORBA::string_dup(pAPD->operator[](p).owner.in());
			//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
			psu->pmId =  CORBA::string_dup(getPlanManagerId(pAPD->operator[](p).plan).c_str());
            psu->plan = pAPD->operator[](p).plan;
            psu->manuExec = pAPD->operator[](p).manuallyExecuted;
            psu->remark = CORBA::string_dup(pAPD->operator[](p).remark.in());

            if ( pAPD->operator[](p).activePlanState == TA_Base_Core::PENDING_ACTIVE_PLAN_STATE )
            {
                pasUpdateData->update.customisePlanUpdate(psu);
            }
            else
            {
                pasUpdateData->update.runPlanUpdate(psu);
            }

//             sendStateUpdateMessage(pasUpdateData);
        }

        for (int i=0; i<nPlans; i++)
        {
		    TA_Base_Core::PASUpdateData* pasDetailUpdateData = new TA_Base_Core::PASUpdateData;
            vecPasUpdateDatas.push_back( pasDetailUpdateData );

            pasDetailUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ActivePlanDetail_var apd = new TA_Base_Core::ActivePlanDetail;

            apd->plan = pAPD->operator[](i).plan;
            apd->path = CORBA::string_dup(pAPD->operator[](i).path.in());
            apd->activePlanState = pAPD->operator[](i).activePlanState;
            apd->currentStepDetail = pAPD->operator[](i).currentStepDetail;
            apd->currentStepDetail.name = CORBA::string_dup(pAPD->operator[](i).currentStepDetail.name.in());
            apd->activeUpdateId = pAPD->operator[](i).activeUpdateId;
            apd->remark = CORBA::string_dup(pAPD->operator[](i).remark.in());
            apd->owner = CORBA::string_dup(pAPD->operator[](i).owner.in());
            apd->timeOfLastStateTransition = CORBA::string_dup(pAPD->operator[](i).timeOfLastStateTransition.in());
            apd->timeOfInstantiation = CORBA::string_dup(pAPD->operator[](i).timeOfInstantiation.in());
            apd->manuallyExecuted = pAPD->operator[](i).manuallyExecuted;

//ActivePlanUpdate
            pasDetailUpdateData->update.planDetail(apd);

//             sendStateUpdateMessage(pasDetailUpdateData);

//Customisation History Update
		    TA_Base_Core::PASUpdateData* pasCustomiseUpdateData = new TA_Base_Core::PASUpdateData;
            vecPasUpdateDatas.push_back( pasCustomiseUpdateData );

            pasCustomiseUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::CustomisationHistory* customisationHistory = getPlanCustomisationHistory(apd->plan, false);

            if ( customisationHistory->details.length() != 0 )
            {
                TA_Base_Core::CustomisationHistoryUpdate_var chu = new TA_Base_Core::CustomisationHistoryUpdate;
                chu->plan = apd->plan;
                chu->history = *customisationHistory;
                pasCustomiseUpdateData->update.customisationHistory(chu);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
            }
            else
            {
                pasCustomiseUpdateData->update.activeUpdateId(customisationHistory->customUpdateId);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
           }

//Execution History Update
		    TA_Base_Core::PASUpdateData* pasHistoryUpdateData = new TA_Base_Core::PASUpdateData;
            vecPasUpdateDatas.push_back( pasHistoryUpdateData );

            pasHistoryUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ExecutionHistory* executionHistory = getPlanExecutionHistory(apd->plan, false);

            if ( executionHistory->details.length() != 0 )
            {
                TA_Base_Core::ExecutionHistoryUpdate_var ehu = new TA_Base_Core::ExecutionHistoryUpdate;
                ehu->plan = apd->plan;
                ehu->history = *executionHistory;
                pasHistoryUpdateData->update.executionHistory(ehu);
//                 sendStateUpdateMessage(pasHistoryUpdateData);
           }
           else
           {
                pasHistoryUpdateData->update.activeUpdateId(executionHistory->activeUpdateId);
//                 sendStateUpdateMessage(pasHistoryUpdateData);
           }

//Active State update
           if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE != apd->activePlanState )
           {
               TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData;
               vecPasUpdateDatas.push_back( pasUpdateData );
               
               pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
               TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
               psu->session = CORBA::string_dup(pAPD[i].owner.in());
               psu->pmId = CORBA::string_dup(getPlanManagerId(pAPD[i].plan).c_str());
               psu->plan = apd->plan;
               psu->remark = CORBA::string_dup(pAPD[i].remark.in());
               psu->manuExec = pAPD[i].manuallyExecuted;
               if ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.stopPlan(psu);
               }
               else if ( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.pausePlan(psu);
               }
               else if ( TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.operatorPausedPlanDetail(apd);
               }
               else
               {
                    vecPasUpdateDatas.pop_back();
                    delete pasUpdateData;
                    pasUpdateData = NULL;
               }
           }
        }

        // Send all state update message anyway because Monitor Agent need to confirm state update request is received by Control Agent
        TA_Base_Core::PASUpdateDataSeq seqPasUpdateData;
        seqPasUpdateData.length( vecPasUpdateDatas.size() );
        for ( int nSeqLoop = 0; nSeqLoop < seqPasUpdateData.length(); ++nSeqLoop )
        {
            if ( NULL != vecPasUpdateDatas[nSeqLoop] )
            {
                seqPasUpdateData[nSeqLoop] = *(vecPasUpdateDatas[nSeqLoop]);
                delete vecPasUpdateDatas[nSeqLoop];
                vecPasUpdateDatas[nSeqLoop] = NULL;
            }
        }
        
        vecPasUpdateDatas.clear();
        sendAllStatesUpdateMessage(seqPasUpdateData);
        
        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent");
	}


	void ActivePlanBulletinBoardImpl::sendStateUpdateRequestToControlAgent( void )
	{
		if ( m_isControlMode )
			return;

		TA_Base_Core::PASUpdateRequest_var pasRequest = new TA_Base_Core::PASUpdateRequest;
        time_t long_time;
        time( &long_time );
        pasRequest->pasRequestId = long_time;
		pasRequest->entityName = CORBA::string_dup(PlanAgentUser::getInstance()->getAgentEntityName().c_str());

		CORBA::Any requestData;
		requestData <<= pasRequest;

        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Msg-: Sent state update request to Control Agent");
		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
												PlanAgentUser::getInstance()->getAgentEntityName(),
												requestData,
												NULL
											 );
	}

	ActivePlanBulletinBoardImpl::PlanDetail::PlanDetail( const PlanDetail& rhs ) :
		m_detail(rhs.m_detail),
		m_lastModified(rhs.m_lastModified),
		m_timeOut (rhs.m_timeOut),
		m_stepStarted (rhs.m_stepStarted),
		m_result(rhs.m_result),
		m_session (rhs.m_session),
		m_pmId (rhs.m_pmId),
		m_userKey (rhs.m_userKey)
	{
	}


	ActivePlanBulletinBoardImpl::PlanDetail::PlanDetail( const std::string& session,
                                                        const std::string& pmId,
                                                        const unsigned long userKey,
                                                        const TA_Base_Core::UpdateId updateId,
																		  const std::string& owner,
																		  const TA_Base_Core::NodeId& planId,
																		  const TA_Base_Core::EActivePlanState& initialState,
														              const std::string& remark,
														              const ACE_Future<TA_Base_Core::EActivePlanState>& result , 
														              const bool manExec, 
																		  const std::string& assocAlarmId) :
		m_result(result)
	{
			m_session				 = session;
            m_pmId                   = pmId;
            m_userKey                = userKey;
			m_detail.plan.plan       = planId;
			m_detail.plan.instance   = 0;
			m_detail.plan.param		 = CORBA::string_dup("");
            m_detail.plan.location   = PlanAgentUser::getInstance()->getAgentLocationKey();
			m_detail.path            = CORBA::string_dup( TA_Base_Bus::PlanDataReader::instance()->getNodePath(planId).c_str() );
			m_detail.owner			 = CORBA::string_dup( owner.c_str() );
			m_detail.activeUpdateId	 = updateId;
			m_detail.activePlanState = initialState;
			m_detail.currentStepDetail.name     = CORBA::string_dup( "undefined" );
			m_detail.currentStepDetail.position = 0;
			m_detail.currentStepDetail.state    = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;
			m_detail.remark = CORBA::string_dup( remark.c_str() );
			m_detail.manuallyExecuted = manExec;
		    m_detail.assocAlarmId     = CORBA::string_dup(assocAlarmId.c_str());

			std::string tmpTime = getLocalTime();
			m_detail.timeOfLastStateTransition = CORBA::string_dup( tmpTime.c_str() );
			m_detail.timeOfInstantiation       = CORBA::string_dup( tmpTime.c_str() );
			m_lastModified = ACE_OS::gettimeofday();
			m_timeOut = 0;
			m_stepStarted = 0;
	}


	ActivePlanBulletinBoardImpl::PlanDetail::PlanDetail( const TA_Base_Core::ActivePlanDetail& detail )
	{
		m_detail.plan.plan                  = detail.plan.plan;
		m_detail.plan.instance              = detail.plan.instance;
        m_detail.plan.location              = detail.plan.location;
		m_detail.plan.param					= CORBA::string_dup( detail.plan.param );
		m_detail.path                       = CORBA::string_dup( detail.path );
		m_detail.owner						= detail.owner;
		m_detail.activeUpdateId				= detail.activeUpdateId;
		m_detail.activePlanState            = detail.activePlanState;
		m_detail.currentStepDetail.name     = CORBA::string_dup( detail.currentStepDetail.name );
		m_detail.currentStepDetail.position = detail.currentStepDetail.position;
		m_detail.manuallyExecuted = detail.manuallyExecuted;
		m_detail.assocAlarmId     = detail.assocAlarmId;

		if ( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE == detail.activePlanState )
		{
			m_detail.currentStepDetail.state = TA_Base_Core::PAUSED_ACTIVE_STEP_STATE;
		}
		else
		{
			m_detail.currentStepDetail.state    = detail.currentStepDetail.state;
		}

		m_detail.remark                      = CORBA::string_dup( detail.remark );
		m_detail.timeOfLastStateTransition  = CORBA::string_dup( detail.timeOfLastStateTransition );
		m_detail.timeOfInstantiation        = CORBA::string_dup( detail.timeOfInstantiation );
		m_lastModified = ACE_OS::gettimeofday();
	}

	void ActivePlanBulletinBoardImpl::PlanDetail::set( const TA_Base_Core::ActivePlanId& plan )
	{
		m_detail.plan = plan;
	}

	bool ActivePlanBulletinBoardImpl::PlanDetail::set( TA_Base_Core::EActivePlanState state )
	{
		if( state != m_detail.activePlanState )
		{
			m_detail.activePlanState = state;
			m_detail.timeOfLastStateTransition = CORBA::string_dup(getLocalTime().c_str());
			m_lastModified = ACE_OS::gettimeofday();
			if( isNonActive() )
			{
				m_result.set(state);
			}
			return( true );
		}

		return( false );
	}

	std::string ActivePlanBulletinBoardImpl::PlanDetail::getGmtTime( void )
	{
		time_t tLoc;
		struct tm tGmt;
        const unsigned int bufferLen = 27;
        char temp[bufferLen];

		ACE_OS::time( &tLoc );
		ACE_OS::gmtime_r( &tLoc, &tGmt );
		return( ACE_OS::asctime_r(&tGmt, temp, bufferLen) );
	}

	std::string ActivePlanBulletinBoardImpl::PlanDetail::getLocalTime( void )
	{
        struct tm newtime;
        time_t long_time;

        time( &long_time );                // Get time as long integer.
        ACE_OS::localtime_r( &long_time, &newtime ); // Convert to local time.

        const size_t MAX_TIME_LENGTH(40);
        char localTime[MAX_TIME_LENGTH];

		// TD12474 ++
		TA_Base_Core::DateFormat dateFormat; 
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);

		::strftime(localTime, MAX_TIME_LENGTH, dateFormatStr.c_str(), &newtime); 
		// ++ TD12474 

        return localTime;
	}
	
	
	///////////////////////////////////////////////////////////////
	//returns timeout for step specified by step id in active plan
	//increased by  30 sec to allow for possible network delays. 
	///////////////////////////////////////////////////////////////
	ACE_Time_Value ActivePlanBulletinBoardImpl::getStepTimeout (const TA_Base_Core::ActivePlanId& planId, TA_Base_Core::StepNumber step)
	{
        ACE_Time_Value timeOut = ACE_Time_Value::zero;
		TA_Base_Core::StepDetails_var stepDetails = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( planId.plan);
		TA_Base_Core::EStep	stepType = stepDetails [step - 1].type;
		TA_Base_Core::StepParameters_var stepPars = TA_Base_Bus::PlanDataReader::instance()->getStepParameters (planId.plan, step, true);
		
		switch (stepType)
		{//non-interactive steps - leave step time-out unchanged
		case TA_Base_Core::END_STEP:
		case TA_Base_Core::ABORT_STEP:
		case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
		case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
		case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
		case TA_Base_Core::DATA_POINT_CHECK_STEP:
		case TA_Base_Core::DATA_POINT_SET_STEP:
		case TA_Base_Core::JUMP_STEP:
		case TA_Base_Core::TERMINATE_PLAN_STEP:
		case TA_Base_Core::VMS_SET_SCENE_STEP:
		case TA_Base_Core::VMS_BLANKOUT_STEP:
		case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
		case TA_Base_Core::VIDEO_WALL_STEP:
		case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
		case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
		case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
			break;
			//interactive steps
		case TA_Base_Core::DECISION_STEP:
			{
				TA_Base_Core::DecisionParameters* decision = &(stepPars->decision());
				timeOut = decision->responseTimeout;
				break;
			}
		case TA_Base_Core::LOG_STEP:
			{
				TA_Base_Core::LogParameters* log = &(stepPars->log ());
				timeOut = log->responseTimeout;
				break;
			}
		case TA_Base_Core::PROMPT_STEP:
			{
				TA_Base_Core::PromptParameters* prompt = &(stepPars->prompt ());
				timeOut = prompt->dismissTimeout;
				break;
			}
		case TA_Base_Core::RUN_PLAN_STEP:
			{	
				TA_Base_Core::RunPlanParameters* runPlan = &(stepPars->runPlan ());
				if (runPlan->manualLaunch)
					timeOut = TA_Base_Bus::IPlanManagerCorbaDef::RUNPLAN_RESPONSE_TIMEOUT;
			}
		case TA_Base_Core::LAUNCH_GUI_APP_STEP:
			{
				timeOut = TA_Base_Bus::IPlanManagerCorbaDef::LAUNCHGUIAPP_RESPONSE_TIMEOUT;
			}
		case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
			{
				timeOut =TA_Base_Bus::IPlanManagerCorbaDef::LAUNCHSCHEMATIC_RESPONSE_TIMEOUT;
			}
		default:
			break;
		}

        if (timeOut > ACE_Time_Value::zero)
        {
			timeOut += NETWORK_DELAY;//add 30 sec. for network delay
        }
		
		return timeOut;
		
	}

	bool ActivePlanBulletinBoardImpl::PlanDetail::isNonActive( void ) const
	{
		switch( m_detail.activePlanState )
		{
            case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
            // don't break

            case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
            // don't break

            case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
            // don't break

            case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                return( true );
                break;

			default:
				return( false );
			    break;
		}
	}

	bool ActivePlanBulletinBoardImpl::PlanDetail::isActive( void ) const
	{
        switch( m_detail.activePlanState )
        {
            case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
                // don't break

            case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
                // don't break

            case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
                // don't break

            case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
                // don't break

            case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
            
                return( true );
                break;
            
            default:
            
                return( false );
                break;
        }
    }

	TA_Base_Core::UpdateId ActivePlanBulletinBoardImpl::getNextUpdateId(void)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);
        {
		    if ( m_updateId >= TA_Base_Core::MAX_UPDATE_ID  )
            {
                if ( TA_Base_Core::MIN_UPDATE_ID < 1 )
                    m_updateId = 0;
                else
			        m_updateId = TA_Base_Core::MIN_UPDATE_ID - 1;
            }

		    return ++m_updateId;
        }
	}

	void ActivePlanBulletinBoardImpl::setUpdateId(TA_Base_Core::UpdateId updateId)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);
        {
            if ( (m_updateId != TA_Base_Core::MAX_UPDATE_ID) && ( updateId <= m_updateId ) )
                return;

		    m_updateId = updateId;
        }
	}

    TA_Base_Core::UpdateId ActivePlanBulletinBoardImpl::getNextPasUpdateId(void)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);
        {
		    if ( m_pasUpdateId >= TA_Base_Core::MAX_UPDATE_ID  )
            {
                if ( TA_Base_Core::MIN_UPDATE_ID < 1 )
                    m_pasUpdateId = 0;
                else
			        m_pasUpdateId = TA_Base_Core::MIN_UPDATE_ID - 1;
            }

            //Note: m_pasUpdateId MUST be incremented and then sent and NOT otherway(i.e. send and increment).
            //      This way the after update is received by the Monitoring PlanAgent, the state of the m_pasUpdateId
            //      is the same as that in the Controlling PlanAgent.

		    return ++m_pasUpdateId; //Not m_pasUpdateId++
        }
    }

    void ActivePlanBulletinBoardImpl::setPasUpdateId(TA_Base_Core::UpdateId updateId)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);
        {
            if ( (m_pasUpdateId != TA_Base_Core::MAX_UPDATE_ID) && ( updateId < m_pasUpdateId ) )
                return;

            m_pasUpdateId = updateId;
        }
    }

	bool ActivePlanBulletinBoardImpl::getSessionIDAndPmID(const std::string& strCompositePmId, std::string& strSessionID, std::string& strPmID)
	{
		FUNCTION_ENTRY("getSessionIDAndPmID");
		bool bSuccess = false;
		
		if(strCompositePmId.size() >= 2 * INDICATION_SESSION_AND_PMID)
		{
			strSessionID = strCompositePmId.substr(0, INDICATION_SESSION_AND_PMID);
			strPmID = strCompositePmId.substr(INDICATION_SESSION_AND_PMID, INDICATION_SESSION_AND_PMID);
			bSuccess = true;
		}

		FUNCTION_EXIT;
		return bSuccess;
	}

    void ActivePlanBulletinBoardImpl::clearAllPlanStates()
    {
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        m_planMap.clear();
    }
    
    void ActivePlanBulletinBoardImpl::updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refNewDetail )
    {
		bool foundInstance = false;
        {
            // Life cycle start for plan map lock
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            PlanMapIterator itDst = m_planMap.find( refNewDetail.plan );
            if( itDst  != m_planMap.end() )
            {
                foundInstance = true;

                if (( itDst->second.isNonActive() ) && ( itDst->second.m_detail.activePlanState != refNewDetail.activePlanState ))
                {
                    // If old plan state is non active, the plan state should not be updated to other state but could update the step state.
                    return;
                }

                // if current step detail is changed, update the execution history
                std::string strTemComp = itDst->second.m_detail.currentStepDetail.name.in();
                if (( itDst->second.m_detail.currentStepDetail.position != refNewDetail.currentStepDetail.position ) ||
                    ( itDst->second.m_detail.currentStepDetail.state != refNewDetail.currentStepDetail.state ) || 
                    ( 0 != strTemComp.compare( refNewDetail.currentStepDetail.name.in() )))
                {
                    if ( TA_Base_Core::LOADED_ACTIVE_STEP_STATE == refNewDetail.currentStepDetail.state )
                    {
                        itDst->second.m_timeOut = getStepTimeout ( refNewDetail.plan, refNewDetail.currentStepDetail.position );
					    itDst->second.m_stepStarted = ACE_OS::gettimeofday();
                    }

                    itDst->second.m_detail.currentStepDetail.state = refNewDetail.currentStepDetail.state;
                    itDst->second.m_detail.currentStepDetail.name = CORBA::string_dup( refNewDetail.currentStepDetail.name.in() );
                    itDst->second.m_detail.currentStepDetail.position = refNewDetail.currentStepDetail.position;

                    if ( TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE != itDst->second.m_detail.currentStepDetail.state )
                    {
                        ExeHistoryMapIterator itHisItem = itDst->second.m_exeHistoryMap.find( refNewDetail.currentStepDetail.position );

                        if ( itDst->second.m_exeHistoryMap.end() != itHisItem )
                        {
                            itHisItem->second.state = refNewDetail.currentStepDetail.state;
                            itHisItem->second.name = CORBA::string_dup( refNewDetail.currentStepDetail.name.in() );
                            itHisItem->second.position = refNewDetail.currentStepDetail.position;
                        }
                        else
                        {
                            TA_Base_Core::ActiveStepDetail newDetail;
                            newDetail.position = refNewDetail.currentStepDetail.position;
                            newDetail.name = CORBA::string_dup( refNewDetail.currentStepDetail.name.in() );
                            newDetail.state = refNewDetail.currentStepDetail.state;
                        
                            itDst->second.m_exeHistoryMap.insert( ExeHistoryMap::value_type( refNewDetail.currentStepDetail.position, newDetail ));
                        }
                    }
                }

                if ( itDst->second.m_detail.activePlanState != refNewDetail.activePlanState )
                {
                    deleteEntityPlanMapRestrictionIfApplicable( refNewDetail.plan, refNewDetail.activePlanState );

                    if ( TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE != refNewDetail.activePlanState )
                    {
                        itDst->second.set( refNewDetail.activePlanState );
                    }

                    // If plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE in execution history
                    if ( itDst->second.isNonActive() )
                    {
                        ExeHistoryMapIterator itHisLoop = itDst->second.m_exeHistoryMap.begin();
                        while ( itDst->second.m_exeHistoryMap.end() != itHisLoop )
                        {
                            TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;
                            
                            if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                                ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                            {
                                itDst->second.m_exeHistoryMap.erase( itHisLoop++ );
                            }
                            else
                            {
                                ++itHisLoop;
                            }
                        }
                    }
                }

                // Update active plan detail
                itDst->second.m_detail.manuallyExecuted = refNewDetail.manuallyExecuted;
                itDst->second.m_detail.remark = CORBA::string_dup( refNewDetail.remark.in() );
                itDst->second.m_detail.activeUpdateId = getNextUpdateId();
            }
            // Life cycle end for plan map lock
        }
        
        if( !foundInstance )
        {
            std::ostringstream what;
            
            what << "ActivePlanBulletinBoardImpl::updateActivePlanDetail() : [P:" << refNewDetail.plan.plan << ",I:" << refNewDetail.plan.instance << "]:"
                << " Not found in bulletin board.";
            
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, refNewDetail.plan.plan, refNewDetail.plan.instance) );
        }
        else
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "[P:%lu,I:%lu,S:%lu]: state is %lu.", refNewDetail.plan.plan, refNewDetail.plan.instance, refNewDetail.currentStepDetail.position, refNewDetail.activePlanState );
		}
    }
} // TA_Base_App

