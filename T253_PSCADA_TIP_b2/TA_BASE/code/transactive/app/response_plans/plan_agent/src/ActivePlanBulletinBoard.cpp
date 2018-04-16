/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanBulletinBoard.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #19 $
  *
  *  Last modification: $DateTime: 2015/07/20 17:48:36 $
  *  Last modified by:  $Author: qi.huang $
  *
  */

#include <algorithm>
#include "ace/OS.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/PlanAgentStateUpdate_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"
#include "app/response_plans/plan_agent/src/SortablePlanId.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/common/src/StringConstants.h"

const time_t NETWORK_DELAY = 3u;  //seconds allowance for a network delay -feel free to adjust if proven unsuitable
const char* const DEFAULT_ASSOC_ALARM_ID = "defaultId"; //default value for associated alarm ID - only those plans which raise alarms will 
											//have this value set to the one supplied by alarm agent (unique for unique alarms)

// This needs to exist so we can put ActivePlandId in a std::map

namespace TA_Base_App
{

	ActivePlanBulletinBoardImpl::ActivePlanBulletinBoardImpl()
	{
		m_updateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );
		m_pasUpdateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );

		m_hangupMessageReceived = false;
		m_isControlMode = false;
		m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::PlanAgentBroadcastComms::Context );
        m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender ( TA_Base_Core::PlanAgentStateUpdate::Context );

		TA_Base_Bus::RightsLibraryFactory rlf;

		m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL);
        m_authenticationLib = new TA_Base_Bus::AuthenticationLibrary();

        TA_Base_Core::IResource* pRes = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( PlanAgentUser::getInstance()->getAgentEntityKey() );
        m_resourceId = pRes->getKey();
        delete pRes;
	}

	ActivePlanBulletinBoardImpl::~ActivePlanBulletinBoardImpl()
	{
		deleteMessageSenders ();
		deleteRightsLib ();
	}

	void ActivePlanBulletinBoardImpl::deleteMessageSenders()
	{
		if ( NULL != m_commsSender )
		{
		    delete m_commsSender;
		    m_commsSender = NULL;
		}

		if ( NULL != m_stateSender )
		{
            delete m_stateSender;
		    m_stateSender = NULL;
		}
	}

	void ActivePlanBulletinBoardImpl::deleteRightsLib ()
	{
		if ( NULL != m_rightsLibrary )
		{
			delete m_rightsLibrary;
			m_rightsLibrary = NULL;
		}

        if ( NULL != m_authenticationLib )
        {
            delete m_authenticationLib;
            m_authenticationLib = NULL;
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
        return isActionPermitted( session, m_resourceId, actionKey, reason );
	}

	bool ActivePlanBulletinBoardImpl::isActionPermitted(const std::string& session, unsigned long resourceKey, unsigned long actionKey, std::string& reason)
	{
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        bool bActionPermitted = false;

		try
        {
            if ( NULL != m_rightsLibrary )
            {
                bActionPermitted = m_rightsLibrary->isActionPermittedOnResource( session.c_str(), resourceKey, actionKey, reason, decisionModule);
            }
		}
        catch (const TA_Base_Core::RightsException& ex)
        {
			reason = "Error: Rights Exception. ";
            reason += ex.what();
        }
		catch ( ... )
		{
			reason = "Error: The Rights Library could not determine the access rights.";
        }

        return bActionPermitted;
	}

    bool ActivePlanBulletinBoardImpl::isControlMode(void)
    {
        return m_isControlMode;
    }

	void ActivePlanBulletinBoardImpl::setControlMode( bool controlMode )
	{
		std::string message;
		if ( controlMode )
		{
			if ( NULL == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate);
			}
			m_isControlMode = true;

			message = "Plan Agent has been put into CONTROL MODE.";
		}
		else
		{
			if ( NULL == m_stateSender )
			{
				m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
					TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest);
			}
			m_isControlMode = false;
			message = "Plan Agent has been put into MONITOR MODE.";
            // Move state update request to PlanAgentUser::agentSetMonitor
// 			sendStateUpdateRequestToControlAgent();
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str());
	}

    std::string ActivePlanBulletinBoardImpl::getOperatorName( const TA_Base_Core::ActivePlanId& plan )
    {
        return getOwner(getSession( plan ) );
    }

    std::string ActivePlanBulletinBoardImpl::getOwner( const std::string& session )
	{
        std::string owner = "session: " + session;

        if ( NULL == m_authenticationLib )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return owner;
        }

        try 
        {
            // The format is "<operator name> (<base profile>[ / <override profile>])"
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(session, session);
		    std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));

            std::string profile("");

            for (std::vector<unsigned long>::const_iterator pIt = sessionInfo.ProfileId.begin(); pIt != sessionInfo.ProfileId.end(); pIt++)
            {
		        std::auto_ptr<TA_Base_Core::IProfile> iProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*pIt, false));

                if (!profile.empty())
                {
                    profile += " / ";
                }

                profile += iProfile->getName();
            }

            if ( profile.empty() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Session has no valid profile associated with it" );
            }

            std::ostringstream opNameAndProfile;
            opNameAndProfile << iOperator->getName() << " (" << profile << ")";

		    owner = opNameAndProfile.str().c_str();
        }
        catch(const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
        }
        catch ( ... )
        {
            std::string message = "Operator name and profile could not be retrieved from session id[";
            message += session;
            message += "]. Constructing default owner from session id.";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, message.c_str() );
        }

		return owner;
	}

	unsigned long ActivePlanBulletinBoardImpl::getOwnerId( const std::string& session )
	{
        unsigned long ownerId = 0xFFFFFFFF;
        unsigned long userLocation = 0xFFFFFFFF;
        unsigned long userWrokstation = 0xFFFFFFFF;

        if ( getUserInfoFromSession( session, ownerId, userLocation, userWrokstation ))
        {
            return ownerId;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
             "Failed to convert session to operator key, returning 0");

		return 0;
	}

    bool ActivePlanBulletinBoardImpl::getUserInfoFromSession( const std::string& session, unsigned long& userKey, unsigned long& userLocation, unsigned long& userWorkstation )
    {
        if (( NULL == m_authenticationLib ) || session.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return false;
        }

        try
        {
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(session, session);

            userKey = sessionInfo.UserId;
            userLocation = sessionInfo.LocationId;
            userWorkstation = sessionInfo.WorkstationId;

            return (!sessionInfo.OperatorId.empty());
        }
        catch(const std::exception& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception" );
        }

        return false;
    }

	bool ActivePlanBulletinBoardImpl::isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan)
	{
		std::string sessionId = getSession( plan );

		if ( sessionId.compare(session) == 0 )
        {
			return true;
        }

        if ( NULL == m_authenticationLib )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "AuthenticationLibrary is deleted" );
            return false;
        }

		try
		{
			if(m_authenticationLib->getSessionInfo(session, session).WorkstationId != 
				m_authenticationLib->getSessionInfo(sessionId, sessionId).WorkstationId)
			{
				return false;
			}
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Exception catch when try find workstatinid of session %s and session %s",
				session.c_str(), sessionId.c_str());
			return false;
		}

        unsigned long userKey = 0xFFFFFFFF;
        unsigned long userLocation = 0xFFFFFFFF;
        unsigned long userWrokstation = 0xFFFFFFFF;
        if ( getUserInfoFromSession( session, userKey, userLocation, userWrokstation ))
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
		unsigned long rval = 0;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			ConstPlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = it->second.userKey;
			}
			else
			{
				std::ostringstream what;
				what << "Plan:[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getUserKey [P:%lu,I:%lu]: userKey = %lu.", plan.plan, plan.instance, rval );

		return rval;
	}

    void ActivePlanBulletinBoardImpl::setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			PlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				it->second.detail.session = session.c_str();
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::setSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, session.c_str() );
	}

	std::string ActivePlanBulletinBoardImpl::getSession( const TA_Base_Core::ActivePlanId& plan ) const
	{
		std::string rval = "";

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
			ConstPlanMapIterator it = m_planMap.find( plan );

			if( it != m_planMap.end() )
			{
				rval = it->second.detail.session.in();
			}
			else
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getSession [P:%lu,I:%lu]: session ID = %s.", plan.plan, plan.instance, rval.c_str() );

		return rval;
	}

	bool ActivePlanBulletinBoardImpl::isActive( const unsigned long plan, const TA_Base_Core::ActivePlanId& ignoreInstance ) const
	{
		bool active = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            ConstPlanMapIterator itLower;
            ConstPlanMapIterator itUpper;
            ConstPlanMapIterator it;
            getInstances( plan, itLower, itUpper );
			it = itLower;

			while( (!active) && (itUpper != it) )
			{
				// check that it is not the instance of the plan that we
				// should be ignoring

				if( it->second.detail.plan.plan != ignoreInstance.plan )
				{
					active = it->second.isActive();
				}
				else
				{
					if( it->second.detail.plan.instance != ignoreInstance.instance )
					{
						active = it->second.isActive();
					}
				}

				++it;
	        }
		}

		// No PlanNotFoundException as asking if a non-existant plan is active is a valid question.
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: %s active.", plan, (active?"is":"is NOT") );

		return active;
	}

    bool ActivePlanBulletinBoardImpl::isActive( const TA_Base_Core::ActivePlanId& plan ) const
    {
        bool active = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            ConstPlanMapIterator itFound = m_planMap.find( plan );

            if ( itFound != m_planMap.end() )
            {
                active = itFound->second.isActive();
            }
        }

        return active;
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
        TA_Base_Core::ExecutionHistory_var history = new TA_Base_Core::ExecutionHistory();
        unsigned long updateId = getNextUpdateId();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;

				what << "ActivePlanBulletinBoardImpl::getPlanExecutionHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}

		    int totalESteps = it->second.exeHistoryMap.size();

		    history->activeUpdateId = updateId;
		    history->details.length(totalESteps);
            ExeHistoryMapIt hIt = it->second.exeHistoryMap.begin();

		    for ( int i = 0; i < totalESteps; i++ )
		    {
			    history->details[i] = (*hIt).second;
			    hIt++;
            }
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
        TA_Base_Core::CustomisationHistory_var history = new TA_Base_Core::CustomisationHistory();
        unsigned long updateId = getNextUpdateId();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( plan );

			if( it == m_planMap.end() )
			{
				std::ostringstream what;
				what << "ActivePlanBulletinBoardImpl::getPlanCustomisationHistory():[P:" << plan.plan << ",I:" << plan.instance << "]:"
					 << " Not found in bulletin board.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}

		    int totalCSteps = it->second.custHistoryMap.size();

		    history->customUpdateId = updateId;
		    history->details.length(totalCSteps);
		    CustHistoryMapIt cIt = it->second.custHistoryMap.begin();

		    for ( int i = 0; i < totalCSteps; i++ )
		    {
			    history->details[i] = (*cIt).second;
			    cIt++;
            }
        }

        if ( bSendIdUpdate )
        {
            //Send StateUpdate for change in m_updateId
            sendStateUpdateMessage( updateId );
        }

		return history._retn();
	}

    void ActivePlanBulletinBoardImpl::getActiveInstances( const unsigned long plan, std::list<TA_Base_Core::ActivePlanId>& lstActiveInstances, const unsigned long ulFilterEntity, const bool bFilter ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        ConstPlanMapIterator itLower;
        ConstPlanMapIterator itUpper;
		getInstances( plan, itLower, itUpper );

        lstActiveInstances.clear();
		for( ConstPlanMapIterator it = itLower; it != itUpper; ++it )
		{
			if( it->second.isActive() && ( !bFilter || ( it->second.detail.triggerEntity == ulFilterEntity )))
			{
				lstActiveInstances.push_back( it->second.detail.plan );
			}
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: Found %lu instance(s) for entity %lu.", plan, lstActiveInstances.size(), ulFilterEntity );
	}

    bool ActivePlanBulletinBoardImpl::existsPlan(const TA_Base_Core::ActivePlanId& plan)
    {
//Note: This is already being called from SendUpdate which has acquired the lock

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        if ( m_planMap.find(plan) != m_planMap.end() )
        {
            return true;
        }

        return false;
    }


	 bool ActivePlanBulletinBoardImpl::isGRCPlan (const TA_Base_Core::ActivePlanId& plan)
	 {
		 std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan.plan);
		 //for GRC plans assumption is they all come from GRC category
		 //this is enforced (to an extent) by displaying only GRC category for 
		 //automatic plan execution (from scheduler and GRC command menu) 
		 std::string rootCategory = planPath.substr (0,5);

         return (rootCategory.compare( "/GRC/" ) == 0);
	 }


	 bool ActivePlanBulletinBoardImpl::isManualPlan(const TA_Base_Core::ActivePlanId& plan)
	 {
         bool bManual = false;

         {
             ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
             ConstPlanMapIterator it = m_planMap.find( plan );

             if( it != m_planMap.end() )
             {
                 bManual = it->second.detail.manuallyExecuted;
             }
             else
             {
                 std::ostringstream what;
                 what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";

                 TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
             }
         }

		 return bManual;
	 }

    const PlanDetail&  ActivePlanBulletinBoardImpl::getActivePlanDetail( const TA_Base_Core::ActivePlanId& plan ) const
    {
		// get the list instances
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        ConstPlanMapIterator itFound = m_planMap.find(plan);

        if ( itFound == m_planMap.end() )
        {
            std::ostringstream what;
            what << "ActivePlanBulletinBoardImpl::getSession():[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
        }

        return itFound->second;
    }


	TA_Base_Core::ActivePlanDetails* ActivePlanBulletinBoardImpl::getActivePlanDetails( unsigned long plan ) const
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
					apd[i] = it->second.detail;
					++i;
				}
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: Found details for %lu instances.", plan, apd->length() );

		// This method does not throw PlanNotFoundException as it is called
		// by the Plan Manager for each plan a user selects (active or not).
		// It is therefore valid to have a zero length list of plan details
		// returned.

		return apd._retn();
	}

	void ActivePlanBulletinBoardImpl::setAssociatedAlarmId(const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum, const std::string& alarmId)
	{
        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            PlanMapIterator it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
                ExeHistoryMapIt itStepFound = it->second.exeHistoryMap.find( ulStepNum );

                if ( it->second.exeHistoryMap.end() != itStepFound )
                {
                    itStepFound->second.alarmId = alarmId.c_str();

                    TA_Base_Core::PlanExecuteStepUpdate stStepUpdate = { plan, itStepFound->second };
                    sendActiveStepStateMessage( stStepUpdate );
                }
			}
			else
			{
				std::ostringstream what;
				what << "[P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board when setAssociatedAlarmId.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}

    std::map<unsigned long, std::string> ActivePlanBulletinBoardImpl::getAssocAlarmIds( const TA_Base_Core::ActivePlanId& plan )
    {
        std::map<unsigned long, std::string> mapAlarmIds;

        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator it = m_planMap.find(plan);

        if ( m_planMap.end() != it )
        {
            for ( ExeHistoryMapIt itStepLoop = it->second.exeHistoryMap.begin(); itStepLoop != it->second.exeHistoryMap.end(); ++itStepLoop )
            {
                std::string strAlarmId = itStepLoop->second.alarmId.in();
                if ( !strAlarmId.empty() )
                {
                    mapAlarmIds.insert( std::map<unsigned long, std::string>::value_type( itStepLoop->first, strAlarmId ));
                }
            }
        }

        return mapAlarmIds;
    }

    void ActivePlanBulletinBoardImpl::resetAssocAlarmId( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator it = m_planMap.find(plan);

        if ( m_planMap.end() != it )
        {
            ExeHistoryMapIt itStepFound = it->second.exeHistoryMap.find( ulStepNum );

            if ( it->second.exeHistoryMap.end() != itStepFound )
            {
                itStepFound->second.alarmId = CORBA::string_dup("");

                TA_Base_Core::PlanExecuteStepUpdate stStepUpdate = { plan, itStepFound->second };
                sendActiveStepStateMessage( stStepUpdate );
            }
        }
    }

	const TA_Base_Core::ActivePlanDetail& ActivePlanBulletinBoardImpl::getAssocAlarmActivePlanDetail(const std::string& alarmId)
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        bool bFound = false;

        PlanMapIterator it = m_planMap.begin();

	    for( ; it != m_planMap.end(); ++it )
	    {
            for ( ExeHistoryMapIt itStepLoop = it->second.exeHistoryMap.begin(); itStepLoop != it->second.exeHistoryMap.end(); ++itStepLoop )
            {
                if (alarmId.compare(itStepLoop->second.alarmId.in()) == 0)
                {
                    bFound = true;
                    break;
                }
            }

            if ( bFound )
            {
                break;
            }
        }

        if ( !bFound )
        {
            std::ostringstream what;
            what << "No plan instance associated with alarm [" << alarmId.c_str() << "]";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, 0, 0) );
        }

        return it->second.detail; 
	}

	TA_Base_Core::ActivePlanStepsDetails* ActivePlanBulletinBoardImpl::getAllActivePlanDetails() const
	{
		TA_Base_Core::ActivePlanStepsDetails_var apd = new TA_Base_Core::ActivePlanStepsDetails();

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			if( 0 >= m_planMap.size() )
			{
				apd->length(0);
			}
			else
			{
				apd->length(m_planMap.size());

				int i = 0;

				for( ConstPlanMapIterator it = m_planMap.begin(); it != m_planMap.end(); ++it, ++i )
				{
                    apd[i].plan = it->second.detail;
                    apd[i].steps.length( it->second.exeHistoryMap.size() );

                    int nAciveStepSIze = 0;

                    for( ExeHistoryMap::const_iterator itExec = it->second.exeHistoryMap.begin(); 
                        itExec != it->second.exeHistoryMap.end(); ++itExec )
                    {
                        int nStepState = itExec->second.state;

                        if (( TA_Base_Core::LOADED_ACTIVE_STEP_STATE == nStepState ) || 
                            ( TA_Base_Core::PAUSED_ACTIVE_STEP_STATE == nStepState ) || 
                            ( TA_Base_Core::WAITING_ACTIVE_STEP_STATE == nStepState ) || 
                            ( TA_Base_Core::DELAYED_ACTIVE_STEP_STATE == nStepState ))
                        {
                            apd[i].steps[nAciveStepSIze++] = itExec->second;
                        }
                    }

                    apd[i].steps.length( nAciveStepSIze );
				}
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Found details for %lu plans/instances.", apd->length() );

		// No PlanNotFoundException as it is valid for there to be no
		// plans running.
		return apd._retn();
	}

	int ActivePlanBulletinBoardImpl::getPlanState( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			ConstPlanMapIterator it = m_planMap.find(plan);

			if ( m_planMap.end() != it )
			{
                TA_Base_Core::EActivePlanState state = it->second.detail.activePlanState;
				return( it->second.detail.activePlanState );
			}
			else
			{
				std::ostringstream what;
				what << "[P:" << plan.plan << ",I:" << plan.instance << "]: Not found in bulletin board when getPlanState.";

                TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
			}
		}
	}

    int ActivePlanBulletinBoardImpl::getStepState( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum ) const
    {
        int eStepState = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;

        ACE_Read_Guard<ACE_RW_Mutex> guard( m_planMapLock );
        {
            ConstPlanMapIterator it = m_planMap.find( plan );

            if ( m_planMap.end() != it )
            {
                ExeHistoryMap::const_iterator itStep = it->second.exeHistoryMap.find( ulStepNum );

                if ( it->second.exeHistoryMap.end() != itStep )
                {
                    eStepState = itStep->second.state;
                }
            }
        }

        return eStepState;
    }

	void ActivePlanBulletinBoardImpl::getInstances( const unsigned long plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const
	{
		TA_Base_Core::ActivePlanId idLower;
		TA_Base_Core::ActivePlanId idUpper;

		idLower.plan = plan;
		idUpper.plan = plan;
		idLower.instance = 1;
		idUpper.instance = std::numeric_limits<unsigned long>::max();

		// get the bounds for all instances of the given plan
		itLower = m_planMap.lower_bound( idLower );
		itUpper = m_planMap.upper_bound( idUpper );
	}

    unsigned long ActivePlanBulletinBoardImpl::getFirstAvailableInstanceNumber(const unsigned long plan)
    {
        // Find the first free instance number for the given plan. Start with 1 and work upwards.
        TA_Base_Core::ActivePlanId activePlanId;
        activePlanId.plan = plan;
        activePlanId.instance = 1u;
        bool bFound = false;

        while (activePlanId.instance < std::numeric_limits<unsigned long>::max())
        {
            if (m_planMap.find(activePlanId) == m_planMap.end())
            {
                bFound = true;
                break;
            }

            activePlanId.instance++;
        }

        if ( !bFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No space left to store plan information");
            TA_THROW( TA_Base_Core::PlanAgentException( "No space left to store plan information", TA_Base_Core::PAE_NO_RESOURCE) );
        }

        return activePlanId.instance;
    }
    
    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, 
        TA_Base_Core::ActivePlanId& plan, const unsigned long entity, const bool bPending, bool manExec )
	{
		std::string owner = getOwner(session);
		std::string remark = (bPending ? RemarkPlanPending : RemarkPlanExecuting);

        plan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        unsigned long userLocation = 0xFFFFFFFF;
        unsigned long userWrokstation = 0xFFFFFFFF;
        getUserInfoFromSession( session, userKey, userLocation, userWrokstation );

        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        if (m_isControlMode)
        {
		    plan.instance = getFirstAvailableInstanceNumber(plan.plan);
        }

        //this is to be filled in latter with an appropriate value
        //if an alarm is raised by this plan
        std::string assocAlarmId = DEFAULT_ASSOC_ALARM_ID;//default value

		// Keep a copy of the instance in the detail, otherwise we would have to set it each time we retrieved it from the map.
        PlanDetail newDetail(session, entity, userKey, userLocation, userWrokstation, getNextUpdateId(), owner, plan, bPending, remark, manExec, assocAlarmId);

        // Try adding the new plan instance to the plan map.
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(plan, newDetail) );

		if( !insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << plan.plan << ",I:" << plan.instance << "]: " << "STL Map insertion failed";
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
        }

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: instance reserved.", plan.plan, plan.instance );

        return true;
	}


    bool ActivePlanBulletinBoardImpl::reserveInstance( const std::string& session, 
        const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bPending )
	{
		std::string owner = getOwner(session);
		std::ostringstream remark;
        remark << "Plan spawned by '" 
               << TA_Base_Bus::PlanDataReader::instance()->getNodePath(parentPlan.plan) 
               << "' instance " << parentPlan.instance;

        childPlan.location = PlanAgentUser::getInstance()->getAgentLocationKey();
        unsigned long userKey = 0xFFFFFFFF;
        unsigned long userLocation = 0xFFFFFFFF;
        unsigned long userWrokstation = 0xFFFFFFFF;
        getUserInfoFromSession( session, userKey, userLocation, userWrokstation );

        // TD10997: Find the first free instance for the child plan. This also ensures that 
        //          the maximum number of active instances for all plans does not get exceeded.
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        if (m_isControlMode)
        {
		    childPlan.instance = getFirstAvailableInstanceNumber(childPlan.plan);
        }

        //this is to be filled in latter with an appropriate value
        //if an alarm is raised by this plan
        std::string assocAlarmId = DEFAULT_ASSOC_ALARM_ID;//default value

		// Keep a copy of the instance in the detail, otherwise we would have to set it 
        // each time we retrieved it from the map.
        PlanDetail newDetail(session, 0u, userKey, userLocation, userWrokstation, getNextUpdateId(), owner, childPlan, bPending, remark.str(), false, assocAlarmId);//not sure about second last param

        // Try adding the new plan instance to the plan map.
        std::pair<PlanMapIterator, bool> insertResult = m_planMap.insert( PlanMap::value_type(childPlan, newDetail) );

		if( !insertResult.second )
		{
			std::ostringstream what;
			what << "[P:" << childPlan.plan << ",I:" << childPlan.instance << "]: " << "STL Map insertion failed";
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, what.str().c_str());
			TA_THROW( ThirdPartyLibException( what.str() ) );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: instance reserved.", childPlan.plan, childPlan.instance );

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

    void ActivePlanBulletinBoardImpl::sendUpdateMessage(const TA_Base_Core::ActivePlanId& plan)
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

		PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
            sendActivePlanUpdateMessage( it->second.detail );
        }
    }

	void ActivePlanBulletinBoardImpl::releaseInstance( const TA_Base_Core::ActivePlanId& plan )
	{
		PlanMap::size_type numRemoved = 0;

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
			 numRemoved = m_planMap.erase( plan );
		}

		if( 1 != numRemoved )
		{
			std::ostringstream what;

			what << "ActivePlanBulletinBoardImpl::releaseInstance() : [P:" << plan.plan << ",I:" << plan.instance << "]: Not in Bulletin Board or not released";

            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: %d instance released.", plan.plan, plan.instance, numRemoved );
	}

    void ActivePlanBulletinBoardImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;

                for (unsigned long i=0; i < historyUpdate.history.details.length(); i++)
                {
                    const TA_Base_Core::StepCustomisableDetail& custDetail = historyUpdate.history.details[i];

                    CustHistoryMapIt chIt = it->second.custHistoryMap.find(custDetail.id);

					if ( chIt != it->second.custHistoryMap.end() )
					{
                        chIt->second = custDetail;
					}
					else
					{
						it->second.custHistoryMap.insert(CustHistoryMap::value_type(custDetail.id, custDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.customUpdateId);

		if( !foundInstance )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Plan [%lu, %lu] not found when processCustomisationHistoryUpdate", historyUpdate.plan.plan, historyUpdate.plan.instance );
		}
    }

    void ActivePlanBulletinBoardImpl::processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate )
    {
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			PlanMapIterator it = m_planMap.find( historyUpdate.plan );

			if( it  != m_planMap.end() )
			{
                foundInstance = true;

                for (unsigned long i=0; i < historyUpdate.history.details.length(); i++)
                {
                    const TA_Base_Core::ActiveStepDetail& stepDetail = historyUpdate.history.details[i];

                    ExeHistoryMapIt ehIt = it->second.exeHistoryMap.find(stepDetail.id);

					if ( ehIt != it->second.exeHistoryMap.end() )
					{
                        ehIt->second = stepDetail;
					}
					else
					{
						it->second.exeHistoryMap.insert(ExeHistoryMap::value_type(stepDetail.id, stepDetail));
					}
                }
            }
        }

        setUpdateId(historyUpdate.history.activeUpdateId);

		if( !foundInstance )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Plan [%lu, %lu] not found when processExecutionHistoryUpdate", historyUpdate.plan.plan, historyUpdate.plan.instance );
		}
    }

	void ActivePlanBulletinBoardImpl::initStep( const TA_Base_Core::ActivePlanId& plan, const unsigned long stepId, const unsigned long stepPos, const std::string& stepName )
    {
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
			ExeHistoryMapIt ehIt = it->second.exeHistoryMap.find(stepId);

			if ( ehIt == it->second.exeHistoryMap.end() )
			{
                TA_Base_Core::ActiveStepDetail cpInsertStepDetail;
				ehIt = it->second.exeHistoryMap.insert(ExeHistoryMap::value_type(stepId, cpInsertStepDetail)).first;
			}

            if ( ehIt != it->second.exeHistoryMap.end() )
            {
                ehIt->second.id = stepId;
                ehIt->second.pos = stepPos;
                ehIt->second.name = stepName.c_str();
                ehIt->second.remark = RemarkExecutingStep;
                ehIt->second.state = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;

                sendActiveStepUpdateMessage( plan, ehIt->second );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: loaded.", plan.plan, plan.instance, stepId );
		}
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan [%lu, %lu] Not found in bulletin board", plan.plan, plan.instance );
        }
	}
    
	void ActivePlanBulletinBoardImpl::updateStepState( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, const int state, const std::string& remark )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator it = m_planMap.find( plan );

        if( it  != m_planMap.end() )
        {
            ExeHistoryMapIt ehIt = it->second.exeHistoryMap.find(step);

            bool bStateUpdated = false;

            if ( ehIt != it->second.exeHistoryMap.end() )
            {
                bStateUpdated = ( ehIt->second.state != state );

                // Update the state and remark only
                ehIt->second.state = static_cast<TA_Base_Core::EActiveStepState>(state);
                ehIt->second.remark = remark.c_str();
            }

            if ( bStateUpdated )
            {
                sendActiveStepUpdateMessage( plan, ehIt->second );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: state is %lu.", plan.plan, plan.instance, step, state );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan [%lu, %lu] Not found in bulletin board", plan.plan, plan.instance );
        }
	}

	void ActivePlanBulletinBoardImpl::updateStepRemark( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, const std::string& remark )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator it = m_planMap.find( plan );

        if( it  != m_planMap.end() )
        {
            ExeHistoryMapIt ehIt = it->second.exeHistoryMap.find(step);

            bool bRemarkUpdated = false;

            if ( ehIt != it->second.exeHistoryMap.end() )
            {
                bRemarkUpdated = ( remark.compare( ehIt->second.remark.in() ) != 0 );

                // Update the remark only
                ehIt->second.remark = remark.c_str();
            }

            if ( bRemarkUpdated )
            {
                sendActiveStepUpdateMessage( plan, ehIt->second );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: remark is %lu.", plan.plan, plan.instance, step, remark.c_str() );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan [%lu, %lu] Not found in bulletin board", plan.plan, plan.instance );
        }
	}

	void ActivePlanBulletinBoardImpl::updatePlanState( const TA_Base_Core::ActivePlanId& plan, const int state, const std::string& remark )
	{
		// helps us spend as little time inside the lock as possible
		bool foundInstance = false;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
		{
			PlanMapIterator it = m_planMap.find( plan );

			if( it  != m_planMap.end() )
			{
				foundInstance = true;
                TA_Base_Core::EActivePlanState oldState = it->second.detail.activePlanState;

				if( it->second.set( state ) )
				{
					it->second.detail.activeUpdateId = getNextUpdateId();
					it->second.detail.remark = remark.c_str();

                    //Plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE
                    if ( it->second.isNonActive() )
                    {
                        ExeHistoryMapIt itHisLoop = it->second.exeHistoryMap.begin();
                        
                        while ( it->second.exeHistoryMap.end() != itHisLoop )
                        {
                            TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;

                            if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                                ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                            {
                                itHisLoop->second.state = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;

                                sendActiveStepUpdateMessage( plan, itHisLoop->second );

                                it->second.exeHistoryMap.erase( itHisLoop++ );
                            }
                            else
                            {
                                ++itHisLoop;
                            }
                        }
                    }

					sendActivePlanUpdateMessage( it->second.detail );
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
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: state is %lu.", plan.plan, plan.instance, state );
		}
	}

    void ActivePlanBulletinBoardImpl::updateOwner( const TA_Base_Core::ActivePlanId& plan, const std::string& session )
    {
        //used only for change of owner 

        // helps us spend as little time inside the lock as possible
        bool foundInstance = false; 

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            PlanMapIterator it = m_planMap.find( plan );

            if( it  != m_planMap.end() )
            {
                foundInstance = true;

                if( session.compare( it->second.detail.session.in() ) != 0 )
                {
                    it->second.detail.activeUpdateId = getNextUpdateId();
                    unsigned long userKey = 0xFFFFFFFF;
                    unsigned long userLocation = 0xFFFFFFFF;
                    unsigned long userWrokstation = 0xFFFFFFFF;
                    getUserInfoFromSession( session, userKey, userLocation, userWrokstation );
                    it->second.userKey = userKey;
                    it->second.userLocation = userLocation;
                    it->second.userWorkstation = userWrokstation;
                    std::string owner = getOwner(session);
                    it->second.detail.owner = owner.c_str();
                    it->second.detail.session = session.c_str();
                    std::string remark = owner;
                    remark += RemarkOperatorTookControl;
                    it->second.detail.remark = remark.c_str();

                    // Actually, the session id and plan manager id could not be sync to monitor agent by this update
                    sendActivePlanUpdateMessage( it->second.detail );
                }				
            }
        }

        if( !foundInstance )
        {
            std::ostringstream what;
            what << "ActivePlanBulletinBoardImpl::update() : [P:" << plan.plan << ",I:" << plan.instance << "]:" << " Not found in bulletin board.";
            TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD, plan.plan, plan.instance) );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: SessionId is %s.", plan.plan, plan.instance, session.c_str() );
        }
    }

	void ActivePlanBulletinBoardImpl::updateCustom( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

		PlanMapIterator it = m_planMap.find( plan );

		if( it  != m_planMap.end() )
		{
			CustHistoryMapIt chIt = it->second.custHistoryMap.find(detail.id);

			if ( chIt != it->second.custHistoryMap.end() )
			{
				chIt->second.id = detail.id;
				chIt->second.skip = detail.skip;
				chIt->second.delay = detail.delay;
			}
			else
			{
				it->second.custHistoryMap.insert(CustHistoryMap::value_type(detail.id, detail));
			}

			TA_Base_Core::PlanCustomisationUpdate_var updateData = new TA_Base_Core::PlanCustomisationUpdate();
			updateData->plan = plan;
            unsigned long updateId = getNextUpdateId();
			updateData->customUpdateId = updateId;
			updateData->detail = detail;

			sendCustomUpdateMessage( updateData );

            //TODO: Send StateUpdate for change in m_updateId
            sendStateUpdateMessage( updateId );
		}
	}

    void ActivePlanBulletinBoardImpl::cullStoppedPlan(const TA_Base_Core::ActivePlanId& plan)
    {
        ACE_Guard<ACE_Mutex> guard(m_cullLock);
	    updatePlanState( plan, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
	    releaseInstance( plan );
    }

    void ActivePlanBulletinBoardImpl::cullAllPlans()
    {
		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.
        ACE_Guard<ACE_Mutex> guard(m_cullLock);
		std::list<TA_Base_Core::ActivePlanId> plansToCull;

		// make sure the read lock goes out of scope before
		// we try to take the write lock when erasing the old
		// plans
		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			for( PlanMapIterator it = m_planMap.begin(); it != m_planMap.end(); ++it )
			{
			    plansToCull.push_back( it->second.detail.plan );
			}
		}

		if( plansToCull.size() > 0 )
		{
			std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

			for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
			{
				updatePlanState( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
				releaseInstance( *cullIt );
			}
		}
    }

	void ActivePlanBulletinBoardImpl::cullStoppedPlans()
	{
        if ( !m_isControlMode )
        {
            return;
        }

		// Make sure only one thread tries to release the stopped plans.
		// It would not cause a problem of more than one did but it is a
		// waste of resources having many threads taking a write
		// lock out on the active plan map.

        ACE_Guard<ACE_Mutex> guard(m_cullLock);
		std::list<TA_Base_Core::ActivePlanId> plansToCull;

		// make sure the read lock goes out of scope before
		// we try to take the write lock when erasing the old
		// plans
		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

			for( PlanMapIterator it = m_planMap.begin(); it != m_planMap.end(); ++it )
			{
				if( it->second.isNonActive() )
				{
					time_t diff = ACE_OS::time() - it->second.lastModified;

                    if( diff > PlanAgentUser::getInstance()->getActivePlanExpiryTime() )
					{
						plansToCull.push_back( it->second.detail.plan );
					}
				}
			}
		}

		if( plansToCull.size() > 0 )
		{
			std::list<TA_Base_Core::ActivePlanId>::iterator cullIt;

			for( cullIt = plansToCull.begin(); cullIt != plansToCull.end(); ++cullIt )
            {
                updatePlanState( *cullIt, TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE, RemarkPlanExpired );
				releaseInstance( *cullIt );
			}
		}
    }

    void ActivePlanBulletinBoardImpl::broadcastAgentResetMessage()
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();
        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

        CORBA::Any commsData;
        commsData <<= CORBA::Any::from_string( locationKey.c_str(), 0 );
        m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::ActivePlanReset, commsData, &filterData );
    }

    void  ActivePlanBulletinBoardImpl::broadcastActivePlanDetailMessage( const TA_Base_Core::ActivePlanDetail& detail )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[PlanUpdate] Update [%lu], Plan [%lu:%s], Location [%lu], Instance [%lu], Owner [%s], Plan State [%d] received",
            detail.activeUpdateId, detail.plan.plan, detail.path.in(), detail.plan.location, detail.plan.instance, detail.owner.in(), detail.activePlanState);

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();
        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

        CORBA::Any commsData;
        commsData <<= detail;
        m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, commsData, &filterData );
    }

    void  ActivePlanBulletinBoardImpl::broadcastActiveStepDetailMessage( const TA_Base_Core::PlanExecuteStepUpdate& detail )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[StepUpdate] Plan [%lu], Location [%lu], Instance [%lu], Step [%lu], State [%d] sent",
            detail.plan.plan, detail.plan.location, detail.plan.instance, detail.detail.id, detail.detail.state);

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();
        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

        CORBA::Any commsData;
        commsData <<= detail;
        m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentBroadcastComms::ActiveStepUpdate, commsData, &filterData );
    }

	void ActivePlanBulletinBoardImpl::sendActivePlanUpdateMessage( const TA_Base_Core::ActivePlanDetail& detail )
    {
        if ( !m_isControlMode )
        {
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "**PlanDetail UpdateId=%u.", detail.activeUpdateId);

        try
        {
            broadcastActivePlanDetailMessage( detail );
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: ActivePlanUpdate message with ActivePlanDetail could not be sent.");
        }

		//Send StateUpdate to the monitor PlanAgent
        try
        {
            sendActivePlanStateMessage(detail);
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: StateUpdate message with ActivePlanDetail could not be sent.");
        }
    }

    void ActivePlanBulletinBoardImpl::sendActiveStepUpdateMessage( const TA_Base_Core::ActivePlanId& refPlan, const TA_Base_Core::ActiveStepDetail& detail )
    {
        if ( !m_isControlMode )
        {
            return;
        }

        TA_Base_Core::PlanExecuteStepUpdate stStepUpdate = { refPlan, detail };

        try
        {
            broadcastActiveStepDetailMessage( stStepUpdate );
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: ActiveStepUpdate message with PlanExecuteStepUpdate could not be sent.");
        }

        //Send StateUpdate to the monitor PlanAgent
        try
        {
            sendActiveStepStateMessage( stStepUpdate );
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: StateUpdate message with PlanExecuteStepUpdate could not be sent.");
        }
    }

	void ActivePlanBulletinBoardImpl::sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;

        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

		CORBA::Any commsData;
		commsData <<= configData;

		m_commsSender->sendBroadcastCommsMessage( 
            TA_Base_Core::PlanAgentBroadcastComms::PlanConfigUpdate, commsData, &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData )
    {
        if ( !m_isControlMode || ( NULL == m_commsSender ))
        {
            return;
        }

        TA_Base_Core::FilterData filterData;
        std::ostringstream lKey;
        lKey << PlanAgentUser::getInstance()->getAgentLocationKey();
        std::string locationKey = lKey.str();

        //LocationKey
        TA_Base_Core::NameValuePair filter("LocationKey", locationKey);
        filterData.push_back(&filter);

        CORBA::Any commsData;
        commsData <<= updateData;

        m_commsSender->sendBroadcastCommsMessage( 
            TA_Base_Core::PlanAgentBroadcastComms::PlanCustomisationUpdate, commsData, &filterData );
	}

	void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData )
	{
		if ( !m_isControlMode || ( NULL == m_stateSender ))
        {
			return;
        }

        stateUpdateData.pasUpdateId = getNextPasUpdateId();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: PAS Update message sent ( update id: %u ), ( type: %u )", stateUpdateData.pasUpdateId, stateUpdateData.update._d() );

		CORBA::Any stateData;
		stateData <<= stateUpdateData;

		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
            PlanAgentUser::getInstance()->getAgentEntityKey(), PlanAgentUser::getInstance()->getAgentEntityName(), stateData, NULL );
    }

    // Pack all states update message to only one message to avoid message lost when Monitor Agent startup
    void ActivePlanBulletinBoardImpl::sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq )
    {
        if ( !m_isControlMode || ( NULL == m_stateSender ))
        {
            return;
        }

        for ( unsigned int nSeqLoop = 0; nSeqLoop < refStateUpdateDataSeq.length(); ++nSeqLoop )
        {
            refStateUpdateDataSeq[nSeqLoop].pasUpdateId = getNextPasUpdateId();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "-Msg-: Generate update id for all update message ( update id: %u ), ( type: %u )", 
                refStateUpdateDataSeq[nSeqLoop].pasUpdateId, refStateUpdateDataSeq[nSeqLoop].update._d() );
        }

        CORBA::Any stateData;
        stateData <<= refStateUpdateDataSeq;

        m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdate,
            PlanAgentUser::getInstance()->getAgentEntityKey(), PlanAgentUser::getInstance()->getAgentEntityName(), stateData, NULL );
    }

	void ActivePlanBulletinBoardImpl::sendActivePlanStateMessage( const TA_Base_Core::ActivePlanDetail& planDetail )
	{
		if ( !m_isControlMode )
        {
			return;
        }

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.planDetail(planDetail);
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StateUpdate sent for ActivePlanDetail: Plan[%lu:%lu]", planDetail.plan.plan, planDetail.plan.instance );
    }

    void ActivePlanBulletinBoardImpl::sendActiveStepStateMessage( const TA_Base_Core::PlanExecuteStepUpdate& detail )
    {
        if ( !m_isControlMode )
        {
            return;
        }

        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        pasUpdateData->update.executedStep(detail);
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StateUpdate sent for ActivePlanStepDetail: Plan[%lu, %lu, %lu]", detail.plan.plan, detail.plan.instance, detail.detail.id );
    }

    void ActivePlanBulletinBoardImpl::sendStateUpdateMessage( const unsigned long updateId )
    {
		if ( !m_isControlMode )
        {
			return;
        }

		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		pasUpdateData->update.activeUpdateId(updateId);
        sendStateUpdateMessage(pasUpdateData);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "INFO: StateUpdate sent for activeUpdateId.");
    }

	void ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent(void)
	{
        //Send update for registered PlanManagers
        std::vector<TA_Base_Core::PASUpdateData*> vecPasUpdateDatas;

        //ActivePlanDetail update
        TA_Base_Core::ActivePlanStepsDetails_var pAPD = getAllActivePlanDetails();

        unsigned int nPlans = pAPD->length();

        for (unsigned int p = 0; p < nPlans; p++)
        {
            TA_Base_Core::ActivePlanDetail& refPlanDetail = pAPD[p].plan;
            TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasUpdateData );

            TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
            psu->session = refPlanDetail.session;
            psu->plan = refPlanDetail.plan;
            psu->manuExec = refPlanDetail.manuallyExecuted;
            psu->remark = refPlanDetail.remark;
            pasUpdateData->update.runPlanUpdate(psu);

//             sendStateUpdateMessage(pasUpdateData);
        }

        // Plan shared data update
        TA_Base_Core::ActivePlanSharedDataSeq_var varPlanSharedDatas = getAllPlanSharedData();

        for ( unsigned int p = 0; p < varPlanSharedDatas->length(); ++p )
        {
            TA_Base_Core::PASUpdateData* pasStepStatusUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasStepStatusUpdateData );
            pasStepStatusUpdateData->update.sharedData( varPlanSharedDatas[p] );
        }

        for (unsigned int i = 0; i < nPlans; i++)
        {
            TA_Base_Core::ActivePlanDetail& refPlanDetail = pAPD[i].plan;
		    TA_Base_Core::PASUpdateData* pasDetailUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasDetailUpdateData );

            pasDetailUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ActivePlanDetail_var apd = new TA_Base_Core::ActivePlanDetail();

            apd->plan = refPlanDetail.plan;
            apd->path = refPlanDetail.path;
            apd->activePlanState = refPlanDetail.activePlanState;
            apd->activeUpdateId = refPlanDetail.activeUpdateId;
            apd->remark = refPlanDetail.remark;
            apd->owner = refPlanDetail.owner;
            apd->session = refPlanDetail.session;
            apd->lastStateTransitionTime = refPlanDetail.lastStateTransitionTime;
            apd->instantiationTime = refPlanDetail.instantiationTime;
            apd->manuallyExecuted = refPlanDetail.manuallyExecuted;
            apd->timer = refPlanDetail.timer;

//ActivePlanUpdate
            pasDetailUpdateData->update.planDetail(apd);
//             sendStateUpdateMessage(pasDetailUpdateData);

//Customization History Update
		    TA_Base_Core::PASUpdateData* pasCustomiseUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasCustomiseUpdateData );

            pasCustomiseUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::CustomisationHistory_var customisationHistory = getPlanCustomisationHistory(apd->plan, false);

            if ( customisationHistory->details.length() != 0 )
            {
                TA_Base_Core::CustomisationHistoryUpdate_var chu = new TA_Base_Core::CustomisationHistoryUpdate();
                chu->plan = apd->plan;
                chu->history = customisationHistory;
                pasCustomiseUpdateData->update.customisationHistory(chu);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
            }
            else
            {
                pasCustomiseUpdateData->update.activeUpdateId(customisationHistory->customUpdateId);
//                 sendStateUpdateMessage(pasCustomiseUpdateData);
            }

//Execution History Update
		    TA_Base_Core::PASUpdateData* pasHistoryUpdateData = new TA_Base_Core::PASUpdateData();
            vecPasUpdateDatas.push_back( pasHistoryUpdateData );

            pasHistoryUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
            TA_Base_Core::ExecutionHistory_var executionHistory = getPlanExecutionHistory(apd->plan, false);

            if ( executionHistory->details.length() != 0 )
            {
                TA_Base_Core::ExecutionHistoryUpdate_var ehu = new TA_Base_Core::ExecutionHistoryUpdate();
                ehu->plan = apd->plan;
                ehu->history = executionHistory;
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
               TA_Base_Core::PASUpdateData* pasUpdateData = new TA_Base_Core::PASUpdateData();
               vecPasUpdateDatas.push_back( pasUpdateData );

               pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
               TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
               psu->session = refPlanDetail.session;
               psu->plan = apd->plan;
               psu->remark = refPlanDetail.remark;
               psu->manuExec = refPlanDetail.manuallyExecuted;
               if ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.stopPlan(psu);
               }
               else if ( TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE == apd->activePlanState )
               {
                   pasUpdateData->update.pausePlan(psu);
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
        for ( unsigned int nSeqLoop = 0; nSeqLoop < seqPasUpdateData.length(); ++nSeqLoop )
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
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ActivePlanBulletinBoardImpl::sendAllStateUpdatesToMonitorAgent");
	}


	void ActivePlanBulletinBoardImpl::sendStateUpdateRequestToControlAgent( void )
    {
        if ( m_isControlMode || ( NULL == m_stateSender ))
        {
            return;
        }

		TA_Base_Core::PASUpdateRequest_var pasRequest = new TA_Base_Core::PASUpdateRequest();
        pasRequest->pasRequestId = ACE_OS::time();
		pasRequest->entityName = PlanAgentUser::getInstance()->getAgentEntityName().c_str();

		CORBA::Any requestData;
		requestData <<= pasRequest;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Sent state update request to Control Agent");
		m_stateSender->sendStateUpdateMessage( TA_Base_Core::PlanAgentStateUpdate::PlanAgentStateUpdateRequest,
												PlanAgentUser::getInstance()->getAgentEntityKey(),
												PlanAgentUser::getInstance()->getAgentEntityName(),
												requestData,
												NULL
											 );
	}

	unsigned long ActivePlanBulletinBoardImpl::getNextUpdateId(void)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);

	    if ( m_updateId >= TA_Base_Core::MAX_UPDATE_ID )
        {
            m_updateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : (TA_Base_Core::MIN_UPDATE_ID - 1u);
        }

	    return ++m_updateId;
	}

	void ActivePlanBulletinBoardImpl::setUpdateId(unsigned long updateId)
	{ 
        ACE_Guard<ACE_Mutex> guard(m_updateIdLock);

        if (( m_updateId != TA_Base_Core::MAX_UPDATE_ID ) && ( updateId <= m_updateId ))
        {
            return;
        }

	    m_updateId = updateId;
	}

    unsigned long ActivePlanBulletinBoardImpl::getNextPasUpdateId(void)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);

	    if ( m_pasUpdateId >= TA_Base_Core::MAX_UPDATE_ID  )
        {
			m_pasUpdateId = ( TA_Base_Core::MIN_UPDATE_ID < 1u ) ? 0u : ( TA_Base_Core::MIN_UPDATE_ID - 1u );
        }

        //Note: m_pasUpdateId MUST be incremented and then sent and NOT otherway(i.e. send and increment).
        //      This way the after update is received by the Monitoring PlanAgent, the state of the m_pasUpdateId
        //      is the same as that in the Controlling PlanAgent.

	    return ++m_pasUpdateId; //Not m_pasUpdateId++
    }

    void ActivePlanBulletinBoardImpl::setPasUpdateId(unsigned long updateId)
    {
		ACE_Guard<ACE_Mutex> guard(m_pasUpdateIdLock);

        if (( m_pasUpdateId != TA_Base_Core::MAX_UPDATE_ID ) && ( updateId < m_pasUpdateId ))
        {
            return;
        }

        m_pasUpdateId = updateId;
    }

    void ActivePlanBulletinBoardImpl::clearAllPlanStates()
    {
        {
		    ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);
            m_planMap.clear();
        }
    }
    
    void ActivePlanBulletinBoardImpl::updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refDetail )
    {
        // Life cycle start for plan map lock
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator itDst = m_planMap.find( refDetail.plan );
        if( itDst  != m_planMap.end() )
        {
            if (( itDst->second.isNonActive() ) && ( itDst->second.detail.activePlanState != refDetail.activePlanState ))
            {
                // If old plan state is non active, the plan state should not be updated to other state but could update the step state.
                return;
            }

            // Update latest step detail
            itDst->second.detail = refDetail;

            if ( itDst->second.detail.activePlanState != refDetail.activePlanState )
            {
                itDst->second.set( refDetail.activePlanState );

                // If plan is no more active, Remove any step which has a state as LOADED_ACTIVE_STEP_STATE in execution history
                if ( itDst->second.isNonActive() )
                {
                    ExeHistoryMapIt itHisLoop = itDst->second.exeHistoryMap.begin();
                    while ( itDst->second.exeHistoryMap.end() != itHisLoop )
                    {
                        TA_Base_Core::EActiveStepState eStepState = itHisLoop->second.state;

                        if (( eStepState == TA_Base_Core::LOADED_ACTIVE_STEP_STATE ) || 
                            ( eStepState == TA_Base_Core::PAUSED_ACTIVE_STEP_STATE ))
                        {
                            itDst->second.exeHistoryMap.erase( itHisLoop++ );
                        }
                        else
                        {
                            ++itHisLoop;
                        }
                    }
                }
            }

            // Update active plan detail
            itDst->second.detail.activeUpdateId = getNextUpdateId();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Plan [P:%lu,I:%lu]: state updated to %lu.", 
                refDetail.plan.plan, refDetail.plan.instance, refDetail.activePlanState );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan [%lu, %lu] Not found in bulletin board", refDetail.plan.plan, refDetail.plan.instance );
        }
    }

    void ActivePlanBulletinBoardImpl::updateActiveStepDetail( const TA_Base_Core::ActivePlanId& refPlanId, const TA_Base_Core::ActiveStepDetail& refDetail )
    {
        // Life cycle start for plan map lock
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator itDst = m_planMap.find( refPlanId );

        if( itDst  != m_planMap.end() )
        {
            ExeHistoryMapIt ehIt = itDst->second.exeHistoryMap.find(refDetail.id);

            if ( ehIt == itDst->second.exeHistoryMap.end() )
            {
                TA_Base_Core::ActiveStepDetail stNewStep = refDetail;
                itDst->second.exeHistoryMap.insert(ExeHistoryMap::value_type(stNewStep.id, stNewStep));
            }
            else
            {
                ehIt->second = refDetail;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu,S:%lu]: state updated to %lu.", 
                refPlanId.plan, refPlanId.instance, refDetail.id, refDetail.state );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan [%lu, %lu] Not found in bulletin board", refPlanId.plan, refPlanId.instance );
        }
    }

    void ActivePlanBulletinBoardImpl::updateActivePlanTimer( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulTimer )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);
        PlanMapIterator itDst = m_planMap.find( plan );
        if( itDst  != m_planMap.end() )
        {
            itDst->second.detail.timer = ulTimer;
        }
    }

    TA_Base_Core::ActivePlanSharedData* ActivePlanBulletinBoardImpl::getPlanSharedData( const TA_Base_Core::ActivePlanId& plan )
    {
        TA_Base_Core::ActivePlanSharedData_var varStatus = new TA_Base_Core::ActivePlanSharedData();

        ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator itPlan = m_planMap.find( plan );

        if( itPlan != m_planMap.end() )
        {
            varStatus->plan = itPlan->second.detail.plan;
            varStatus->sharedData.length( itPlan->second.sharedData.size() );

            int nIndex = 0;

            for ( KeyValueMapIt itLoop = itPlan->second.sharedData.begin(); 
                itLoop != itPlan->second.sharedData.end(); ++itLoop )
            {
                varStatus->sharedData[nIndex].key = itLoop->first.c_str();
                varStatus->sharedData[nIndex].value = itLoop->second.c_str();
                ++nIndex;
            }

            if ( 0 < nIndex )
            {
                varStatus->sharedData.length( nIndex );
            }
        }

        return varStatus._retn();
    }

    TA_Base_Core::ActivePlanSharedDataSeq* ActivePlanBulletinBoardImpl::getAllPlanSharedData()
    {
        TA_Base_Core::ActivePlanSharedDataSeq_var ascss = new TA_Base_Core::ActivePlanSharedDataSeq();

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard(m_planMapLock);

            ascss->length( m_planMap.size() );
            int nPlanIndex = 0;

            for ( PlanMapIterator itPlanLoop = m_planMap.begin(); itPlanLoop != m_planMap.end(); ++itPlanLoop )
            {
                if ( itPlanLoop->second.sharedData.empty() )
                {
                    continue;
                }

                TA_Base_Core::ActivePlanSharedData_var varStatus = new TA_Base_Core::ActivePlanSharedData();
                ascss[nPlanIndex].plan = itPlanLoop->second.detail.plan;
                ascss[nPlanIndex].sharedData.length( itPlanLoop->second.sharedData.size() );

                int nIndex = 0;

                for ( KeyValueMapIt itLoop = itPlanLoop->second.sharedData.begin(); 
                    itLoop != itPlanLoop->second.sharedData.end(); ++itLoop )
                {
                    varStatus->sharedData[nIndex].key = itLoop->first.c_str();
                    varStatus->sharedData[nIndex].value = itLoop->second.c_str();
                    ++nIndex;
                }

                if ( 0 < nIndex )
                {
                    ++nPlanIndex;
                    ascss[nIndex].sharedData.length( nIndex );
                }
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found %d details for active plan shared data.", ascss->length() );
        return ascss._retn();
    }

    void ActivePlanBulletinBoardImpl::updateAllPlanSharedData( const TA_Base_Core::ActivePlanSharedDataSeq& statusSeq )
    {
        if ( m_isControlMode )
        {
            return;
        }

        for ( unsigned int nPlanLoop = 0u; nPlanLoop < statusSeq.length(); ++nPlanLoop )
        {
            std::map<std::string, std::string> mapSharedData;

            const TA_Base_Core::ActivePlanSharedData& refSharedData = statusSeq[nPlanLoop];

            for ( unsigned int nLoop = 0u; nLoop < refSharedData.sharedData.length(); ++nLoop )
            {
                mapSharedData[refSharedData.sharedData[nLoop].key.in()] = refSharedData.sharedData[nLoop].value.in();
            }

            updatePlanSharedData( refSharedData.plan, mapSharedData );
        }
    }

    void ActivePlanBulletinBoardImpl::updatePlanSharedData( const TA_Base_Core::ActivePlanId& plan, const std::map<std::string, std::string>& mapKeyValues )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_planMapLock);

        PlanMapIterator it = m_planMap.find( plan );

        if( it  != m_planMap.end() )
        {
            for ( std::map<std::string, std::string>::const_iterator itLoop = mapKeyValues.begin(); mapKeyValues.end() != itLoop; ++itLoop )
            {
                if ( itLoop->second.empty() )
                {
                    it->second.sharedData.erase( itLoop->first );
                }
                else
                {
                    it->second.sharedData[itLoop->first] = itLoop->second.c_str();
                }
            }

            if ( m_isControlMode )
            {
                TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();
                pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
                TA_Base_Core::ActivePlanSharedData_var varStatus = new TA_Base_Core::ActivePlanSharedData();
                varStatus->plan = plan;
                varStatus->sharedData.length( mapKeyValues.size() );

                int nIndex = 0;

                for ( KeyValueMap::const_iterator itLoop = mapKeyValues.begin(); itLoop != mapKeyValues.end(); ++itLoop )
                {
                    varStatus->sharedData[nIndex].key = itLoop->first.c_str();
                    varStatus->sharedData[nIndex].value = itLoop->second.c_str();
                    ++nIndex;
                }

                pasUpdateData->update.sharedData( varStatus );
                sendStateUpdateMessage( pasUpdateData );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "StateUpdate sent for plan shared data [%lu, %lu]", plan.plan, plan.instance);
            }
        }
    }
} // TA_Base_App

