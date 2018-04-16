/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Gregg Kirkpatrick
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Listens to FrontEndStateChangeSubject and sends messages for displaying to affected operators.
  *
  */

#include "DutyAgent.h"
#include "DutyAuditMessageSender.h"
#include "DutyAuditMessageConverter.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/threads/src/IWorkItem.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::SessionAccessFactory;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::LocationAccessFactory;

namespace 
{
    // The following wrapper functions catch exceptions and return NULL or empty
    // values if errors occur.  This allows them to be used inline in more complicated
    // expressions.

    // TODO LPP: It makes it difficult to write neat code when so many methods in 
    // classes (such as ISession) throw exceptions rather than simply returning
    // empty strings etc.

    // NULL is returned if session 'sessionStr' can't be found.
    // NOTE that ISession methods CAN throw exceptions.
    /*gives*/ 
	TA_Base_Bus::SessionInfo getSession(const std::string& sessionStr)
    {

		TA_Base_Bus::SessionInfo result;

        if (sessionStr.empty())
        {
            return result;
        }

        try
        {
			
			result = TA_Base_Bus::DutySessionCache::instance()->getSessionInfo(sessionStr);

        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "gGetSession(): TransactiveException", e.what() );
        }

        return result;
    }

    // Returns the location name (eg. "DBG") of the session if successful, 
    // otherwise an empty string. 'session' can be NULL. 
    std::string getLocationNameFromSession(TA_Base_Bus::SessionInfo& sessionInfo)
    {
      
        try
        { 
			if(sessionInfo.OperatorId.size()> 0 )
			{
				std::string result = TA_Base_Bus::DutyDbCache::instance()->getRegionName(sessionInfo.LocationId);
				return result;
			}
			else
				return "";
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "gGetLocationNameFromSession(): TransactiveException", e.what() );
        }
       

        return "";
    }

    // Returns the name of the operator if successful, otherwise "".
    // For example, "JulienC".  'session' can be NULL. 
    std::string getOperatorNameFromSession(TA_Base_Bus::SessionInfo& sessionInfo)
    {
  
        try
        {
			if( sessionInfo.OperatorId.size() > 0 )
			{
				
				return TA_Base_Bus::DutyDbCache::instance()->getOperatorName(sessionInfo.OperatorId[0]);				
			}
			else
			{
				return "";
			}
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getOperatorNameFromSession(): TransactiveException", e.what() );
        }
      
        return "";
    }

    // Returns the name of the profile if successful, otherwise "".
    // For example, "PSCO".  
    std::string getProfileNameFromSession(TA_Base_Bus::SessionInfo& sessionInfo)
    {
     
        try
        {
			if( sessionInfo.ProfileId.size() > 0)
			{
				return TA_Base_Bus::DutyDbCache::instance()->getProfileName(sessionInfo.ProfileId[0]);
			}
			else
			{
				return "";
			}
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getProfileNameFromSession(): TransactiveException", e.what() );
        }
     

        return "";
    }

    std::string getRegionNameFromKey(const TA_Base_Bus::DASType::DbKey& key)
    {
        try
        {
            return TA_Base_Bus::DutyDbCache::instance()->getRegionName(key);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getRegionNameFromKey(): TransactiveException", e.what() );
        }

        return "";
    }

    std::string getSubsystemNameFromKey(const TA_Base_Bus::DASType::DbKey& key)
    {
        try
        {
            return TA_Base_Bus::DutyDbCache::instance()->getSubsystemName(key);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getSubsystemNameFromKey(): TransactiveException", e.what() );
        }

        return "";
    }
}

    
namespace TA_Base_App
{
	DutyAuditMessageSender::DutyAuditMessageSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl>(*TA_Base_Bus::FrontEndStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		m_auditSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
			TA_Base_Core::DutyAgentAudit::Context) ),
			m_threadPool(0)
	{
		FUNCTION_ENTRY("DutyAuditMessageSender");
		m_threadPool = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
		FUNCTION_EXIT;
	}

	DutyAuditMessageSender::~DutyAuditMessageSender()
	{
		FUNCTION_ENTRY("~DutyAuditMessageSender");

		TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);
		m_threadPool = NULL;
		FUNCTION_EXIT
	}
	void DutyAuditMessageSender::update( TA_Base_Bus::FrontEndStateChangeSubjectImpl& frontEndStateChange, const TA_Base_Bus::DutyState & state )
	{
		FUNCTION_ENTRY("update");
// 		boost::shared_ptr<TA_Base_Bus::DutyState> dutyState(new TA_Base_Bus::DutyState(state));
// 		boost::shared_ptr<IWorkItem> workItem(new DutyAuditMessageWorkItem(*this, dutyState));
// 		m_threadPool->queueWorkItem(workItem);
		this->update(state);
		FUNCTION_EXIT;
	}
		void DutyAuditMessageSender::update(const TA_Base_Bus::DutyState & state )
	{
		FUNCTION_ENTRY("update");

		//TA_Base_Bus::DutyState* state = frontEndStateChange.getStateUpdate();

		// HACK: Should use DutyAuditMessageConverter to access the DutyMap.
        typedef std::map<TA_Base_Bus::DASType::DbKey, TA_Base_Bus::DutyState::Duty> DutyMap;
        DutyMap dutyMap = state.getDutyMap();

/*
		// yanrong++: CL-19418
#ifdef Duty_Dubug
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-19418] DutyAuditMessageSender::update, dutyInitAction=%s", state.getInitiatingAction().c_str());
		if( DutyAction::SESSION_BEGIN.compare(state.getInitiatingAction()) != 0)
		{
			const std::string outputFileName = "./DutyMapOutput.txt";
			std::ofstream outputFile(outputFileName.c_str(), std::ios::app);
			if(outputFile.is_open())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[CL-19418] output dutymap to file: %s", outputFileName.c_str());
				outputFile << "=============================================================" << std::endl;
				state.printEx(outputFile);
				//state.print(outputFile);
				outputFile.close();
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[CL-19418] open dutymap output file failed, file=%s", outputFileName.c_str());
			}
		}
#endif
		// ++yanrong
*/
        try
        {
			AuditMessageDataList auditMessageDataList; //Request to subsystem/s
			AuditMessageDataAllList auditMessageDataAllList; //Request to ALL subsystem

            DutyMap::const_iterator regionsIter;
            TA_Base_Bus::DutyState::SubDutyMap::const_iterator subsysIter;
            TA_Base_Bus::DASType::DutySet::const_iterator profileIt;

            std::string localSession = state.getInitiatingSession();
			//azenitha
			std::string temp = "TDACCESS: localSession = "; 
			temp += localSession; 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
			//azenitha

            // TD11925: Detail audit message should not be enabled as specified in the Duty Agent DN.
		    if( DutyAction::REQUEST_REGION_DUTY_TRANSFER == state.getInitiatingAction() )
		    {
                for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
                {
                    std::string regionName = getRegionNameFromKey( regionsIter->first );
                    std::string srcSession;

                    for( profileIt = regionsIter->second.m_region.m_add.begin(); profileIt != regionsIter->second.m_region.m_add.end(); profileIt++ )
                    {
                        srcSession = (*profileIt).m_session;      // should be the same as localSession
                    }

                    for( profileIt = regionsIter->second.m_region.m_remove.begin(); profileIt != regionsIter->second.m_region.m_remove.end(); profileIt++ )
                    {
                        sendAuditMessage(
                            TA_Base_Core::DutyAgentAudit::RegionDutyTransferReceived,
                            localSession, (*profileIt).m_session, false,
                            "", regionName,
                            "Region Duty Transfer" );

                        // each profile that lost location duty needs to generate an audit msg
                        if (!srcSession.empty())
                        {
                            if (localSession.compare(srcSession) == 0)
                            {
                                // generate subsystem list
                                std::string subsystemIdListGained;
                                std::string subsystemIdListLost;
                                for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
                                {
                                    std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);
                                    if (subsysIter->second.m_add.size() != 0)
                                    {
                                        if (subsystemIdListGained.empty())
                                        {
                                            subsystemIdListGained = subsystemName;
                                        }
                                        else
                                        {
                                            subsystemIdListGained += ", " + subsystemName;
                                        }
                                    }

                                    if (subsysIter->second.m_remove.size() != 0)
                                    {
                                        if (subsystemIdListLost.empty())
                                        {
                                            subsystemIdListLost = subsystemName;
                                        }
                                        else
                                        {
                                            subsystemIdListLost += ", " + subsystemName;
                                        }
                                    }
                                }

                                sendAuditMessage(
                                    TA_Base_Core::DutyAgentAudit::RegionDutyTransfer,
                                    localSession, (*profileIt).m_session, true,
                                    "", regionName,
                                    "Region Duty Transfer" );

                                sendAuditMessage(
                                    TA_Base_Core::DutyAgentAudit::SubsystemDutyLost,
                                    localSession, (*profileIt).m_session, false,
                                    subsystemIdListLost, regionName,
                                    "Region Duty Transfer" );

                                sendAuditMessage(
                                    TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
                                    localSession, (*profileIt).m_session, true,
                                    subsystemIdListGained, regionName,
                                    "Region Duty Transfer" );
                            }
                            else
                            {
                                LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"InitiatingSession :%s is different from srcSession: %s", localSession.c_str(), srcSession.c_str());
                            }
                        }
                    }
                }
		    }
		    else if( DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER == state.getInitiatingAction() )
		    {
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: START New request =========="); 

				clearPendingAuditMessages(auditMessageDataList);

                for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
                {
                    std::string regionName = getRegionNameFromKey( regionsIter->first );
					//azenitha
					std::string temp = "TDACCESS: regionName = "; 
					temp += regionName; 
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
					//azenitha

                    for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
                    {
                        std::string srcSession;
                        std::string destSession;
                        std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);
						//azenitha
						std::string temp = "TDACCESS: subsystemName = "; 
						temp += subsystemName;
						LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
						//azenitha

                        if (!subsysIter->second.m_timedOut.m_session.empty())
                        {
							//azenitha
							if (localSession.compare(subsysIter->second.m_timedOut.m_session) != 0)
							{
								pendingAuditMessageToSend(localSession, subsysIter->second.m_timedOut.m_session, subsystemName, regionName, AUDIT_TIMEOUT,auditMessageDataList);
							}
							else
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == m_timedOut.m_session, ignore");
							}
							//azenitha
                        }
                        else if (!subsysIter->second.m_deniedBy.m_session.empty())
                        {
							//azenitha
							if (localSession.compare(subsysIter->second.m_deniedBy.m_session) != 0)
							{
								pendingAuditMessageToSend(localSession, subsysIter->second.m_deniedBy.m_session, subsystemName, regionName, AUDIT_DENY,auditMessageDataList);
							}
							else
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == m_deniedBy.m_session, ignore");
							}
							//azenitha
                        }
                        else /*if (!destSession.empty())*/
                        {
							// yanrong++: CL-19418
							std::vector<TA_Base_Bus::DutyPrimitive> addDutys;
							std::vector<TA_Base_Bus::DutyPrimitive> remDutys;
							// ++yanrong

							for( profileIt = subsysIter->second.m_remove.begin(); profileIt != subsysIter->second.m_remove.end(); profileIt++ )
							{
								destSession = (*profileIt).m_session;

								//azenitha
								std::string temp = "TDACCESS: destSession = "; 
								temp += destSession; 
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
								//azenitha

								// yanrong++: CL-19418
								/*
								//azenitha
								if (localSession.compare(destSession) != 0)
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: Add to list from m_remove"); 
									pendingAuditMessageToSend(localSession, destSession, subsystemName, regionName, AUDIT_ACCEPT);
								}
								else
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == destSession, ignore"); 
								}
								//azenitha
								*/
								TA_Base_Bus::IDutyPolicy::RIGHTS_TYPE rightsType = 
									TA_Base_Bus::DutyDbCache::instance()->getRightsType(profileIt->m_profile,
									subsysIter->first, 
									subsysIter->second.m_remState);
								bool isExclusiveDuty = (rightsType == TA_Base_Bus::IDutyPolicy::RT_EXCLUSIVE);

								if(isExclusiveDuty)
								{
									remDutys.push_back(*profileIt);
								}
								// ++yanrong
							}

							for( profileIt = subsysIter->second.m_add.begin(); profileIt != subsysIter->second.m_add.end(); profileIt++ )
							{
								// srcSession should be the same as localSession
								srcSession = (*profileIt).m_session;

								//azenitha
								std::string temp = "TDACCESS: srcSession = "; 
								temp += srcSession; 
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
								//azenitha

								// yanrong++: CL-19418
								/*
								//azenitha
								if (localSession.compare(srcSession) != 0)
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: Add to list from m_add"); 
									pendingAuditMessageToSend(localSession, srcSession, subsystemName, regionName, AUDIT_ACCEPT);
								}
								else
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == srcSession, ignore"); 
								}
								//azenitha	
								*/
								TA_Base_Bus::IDutyPolicy::RIGHTS_TYPE rightsType = 
									TA_Base_Bus::DutyDbCache::instance()->getRightsType(profileIt->m_profile, 
									subsysIter->first, 
									subsysIter->second.m_addState);
								bool isExclusiveDuty = (rightsType == TA_Base_Bus::IDutyPolicy::RT_EXCLUSIVE);

								if(isExclusiveDuty)
								{
									addDutys.push_back(*profileIt);
								}
								// ++yanrong
							}

							// yanrong++: CL-19418
							if((remDutys.size() > 1) || (addDutys.size() > 1))
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[CL-19418] More than one exclusive duty lost/gain while duty transfer");
							}

							if((remDutys.size() > 0) && (addDutys.size() > 0))
							{
								srcSession = addDutys[0].m_session;
								destSession = remDutys[0].m_session;
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[CL-19418] pendingAutitMessage (AUDIT_ACCEPT), srcSession=%s, destSession=%s",
									srcSession.c_str(),
									destSession.c_str());
								pendingAuditMessageToSend(srcSession, destSession, subsystemName, regionName, AUDIT_ACCEPT,auditMessageDataList);
							}
							// ++yanrong
                        }
                        /*else if ( (subsysIter->second.m_timedOut.m_session.empty()) && 
                                  (subsysIter->second.m_deniedBy.m_session.empty()) &&
                                  (destSession.empty()) && (!srcSession.empty()) )
                        {
                            // either subsystem is non-exclusive or previously no operator have duty
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
                                localSession, "", true,
                                subsystemName, regionName,
                                "Subsystem duty gain" );
                        }
                        else
                        {
                            // do nothing
                        }*/
                    }
                }

				sendPendingAuditMessages(auditMessageDataList);

				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: END New request =========="); 
		    }
            else if( DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER == state.getInitiatingAction() )
		    {
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: START New request ALL =========="); 

				clearPendingAuditMessagesAll(auditMessageDataAllList);

                for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
                {
                    std::string regionName = getRegionNameFromKey( regionsIter->first );
					//azenitha
					std::string temp = "TDACCESS: regionName = "; 
					temp += regionName; 
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
					//azenitha

                    for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
                    {
                        std::string srcSession;
                        std::string destSession;
                        std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);
						//azenitha
						std::string temp = "TDACCESS: subsystemName = "; 
						temp += subsystemName; 
						LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
						//azenitha

                        if (!subsysIter->second.m_timedOut.m_session.empty())
                        {
							//azenitha
							if (localSession.compare(subsysIter->second.m_timedOut.m_session) != 0)
							{
								pendingAuditMessageToSendAll(localSession, subsysIter->second.m_timedOut.m_session, AUDIT_TIMEOUT,auditMessageDataAllList);
							}
							else
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == m_timedOut.m_session, ignore"); 
							}
							//azenitha
                        }
                        else if (!subsysIter->second.m_deniedBy.m_session.empty())
                        {
							//azenitha
							if (localSession.compare(subsysIter->second.m_deniedBy.m_session) != 0)
							{
								pendingAuditMessageToSendAll(localSession, subsysIter->second.m_deniedBy.m_session, AUDIT_DENY,auditMessageDataAllList);
							}
							else
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == m_deniedBy.m_session, ignore"); 
							}
							//azenitha
                        }
						else /*if (!destSession.empty())*/
						{
							for( profileIt = subsysIter->second.m_remove.begin(); profileIt != subsysIter->second.m_remove.end(); profileIt++ )
							{
								destSession = (*profileIt).m_session;

								//azenitha
								std::string temp = "TDACCESS: destSession = "; 
								temp += destSession; 
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
								//azenitha

								//azenitha
								if (localSession.compare(destSession) != 0)
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: Add to list from m_remove"); 
									pendingAuditMessageToSendAll(localSession, destSession, AUDIT_ACCEPT,auditMessageDataAllList);
								}
								else
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == destSession, ignore");
								}
								//azenitha
							}

							for( profileIt = subsysIter->second.m_add.begin(); profileIt != subsysIter->second.m_add.end(); profileIt++ )
							{
								// srcSession should be the same as localSession
								srcSession = (*profileIt).m_session;

								//azenitha
								std::string temp = "TDACCESS: srcSession = "; 
								temp += srcSession; 
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
								//azenitha

								//azenitha
								if (localSession.compare(srcSession) != 0)
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: Add to list from m_add"); 
									pendingAuditMessageToSendAll(localSession, srcSession, AUDIT_ACCEPT,auditMessageDataAllList);
								}
								else
								{
									LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: localSession == srcSession, ignore"); 
								}
								//azenitha
							}
						}
                        /*else if ( (subsysIter->second.m_timedOut.m_session.empty()) && 
                                  (subsysIter->second.m_deniedBy.m_session.empty()) &&
                                  (destSession.empty()) && (!srcSession.empty()) )
                        {
                            // either subsystem is non-exclusive or previously no operator have duty
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyAllGain,
                                localSession, "", true,
                                subsystemName, regionName,
                                "Subsystem duty all gain" );
							break;
                        }
                        else
                        {
                            // do nothing
                        }*/
                    }
                }

				sendPendingAuditMessagesAll(auditMessageDataAllList);

				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "TDACCESS: END New request ALL =========="); 
		    }
            else if (DutyAction::SYSTEMWIDE_DELEGATION == state.getInitiatingAction())
		    {
                {
					TA_Base_Bus::SessionInfo session( getSession(localSession) );
                    std::string opName( getOperatorNameFromSession(session) );

			        TA_Base_Core::DescriptionParameters params;
			        TA_Base_Core::NameValuePair param1("OperatorName", opName);
			        params.push_back(&param1);

			        m_auditSender->sendAuditMessage(
				        TA_Base_Core::DutyAgentAudit::SystemWideDelegation,
				        TA_Base_Bus::TransactiveAgentInfo::m_entity,
				        params, "",
				        "", "", "", "" );
                }

                for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
                {
                    std::string regionName = getRegionNameFromKey( regionsIter->first );

                    for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
                    {
                        std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);

                        for( profileIt = subsysIter->second.m_add.begin(); profileIt != subsysIter->second.m_add.end(); profileIt++ )
				        {
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
                                (*profileIt).m_session, "", true,
                                subsystemName, regionName,
                                "SystemWideDelegation" );
				        }

				        for( profileIt = subsysIter->second.m_remove.begin(); profileIt != subsysIter->second.m_remove.end(); profileIt++ )
				        {
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyLost,
                                "", (*profileIt).m_session, false,
                                subsystemName, regionName,
                                "SystemWideDelegation" );
				        }
                    }
                }
		    }
		    else if( DutyAction::AUTOMATIC_DELEGATION == state.getInitiatingAction() )
		    {
				// CL-20456 ++
				// CL-20456: Automatic delegation do not pass session info
				//			 DutyAuditMessageConverter need a session to initialize
				//			 Do not use DutyAuditMessageConverter, 
				// cancel this
				// 		        DutyAuditMessageConverter damc;
				// 		        state.convert(damc); // raise exception
				{
					TA_Base_Core::DescriptionParameters params;
					
					for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
					{
						std::string regionName = getRegionNameFromKey( regionsIter->first );
						std::auto_ptr<TA_Base_Core::NameValuePair> regionPair(
							new TA_Base_Core::NameValuePair("RegionNameList", regionName));
						params.push_back(regionPair.release());
					}
					// this message's log_message flag is false!!
					// set to true to see it in DB
					m_auditSender->sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::AutomaticDelegation,
						TA_Base_Bus::TransactiveAgentInfo::m_entity,
						params, "",
						localSession, "", "", "" );
					for( TA_Base_Core::DescriptionParameters::iterator itPair = params.begin(); itPair != params.end(); ++itPair )
					{
						delete *itPair;
						*itPair = NULL;
					}
					params.clear();
				}
				// ++ CL-20456
                for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
                {
                    std::string regionName = getRegionNameFromKey( regionsIter->first );

                    for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
                    {
                        std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);

                        for( profileIt = subsysIter->second.m_add.begin(); profileIt != subsysIter->second.m_add.end(); profileIt++ )
				        {
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
                                (*profileIt).m_session, "", true,
                                subsystemName, regionName,
							  "SystemAutoDelegation" );
				        }

				        for( profileIt = subsysIter->second.m_remove.begin(); profileIt != subsysIter->second.m_remove.end(); profileIt++ )
				        {
                            sendAuditMessage(
                                TA_Base_Core::DutyAgentAudit::SubsystemDutyLost,
                                "", (*profileIt).m_session, false,
                                subsystemName, regionName,
							  "SystemAutoDelegation" );
				        }
                    }
                }
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "DutyAuditMessageSender send message for AUTOMATIC_DELEGATION" ); // CL-20456
		    }
		    else if( DutyAction::SUBSYSTEM_DEGRADED == state.getInitiatingAction() )
		    {
			    sendSubsystemsMessage( TA_Base_Core::DutyAgentAudit::SubsystemDegraded, state );
		    }
		    else
		    {
			    return;
		    }
        }
        catch (TA_Base_Core::TransactiveException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Unkown exception");
        }

		FUNCTION_EXIT;
	}


	void DutyAuditMessageSender::sendSubsystemsMessage( const TA_Base_Core::MessageType& messageType, const TA_Base_Bus::DutyState& state )
	{
		DutyAuditMessageConverter damc;
		state.convert(damc);
			
		std::vector<TA_Base_Core::DescriptionParameters*>& detailDesc =
			damc.buildSubsystemDetailDesc();

		std::vector<TA_Base_Core::DescriptionParameters*>::const_iterator descIt;

		for( descIt=detailDesc.begin(); descIt!=detailDesc.end(); descIt++ )
		{
			m_auditSender->sendAuditMessage(
				messageType,
				TA_Base_Bus::TransactiveAgentInfo::m_entity,
				*(*descIt), "DutyAuditMessageSender::sendSubsystemsMessage",
				state.getInitiatingSession(), "", "", "" );
		}
	}


    // Returns a string of the form "left/right".
    std::string DutyAuditMessageSender::concatStr(const std::string& left, const std::string& right)
    {
		if ((!left.empty()) && (!right.empty()))
        {
            return left + "/" + right;
        }
        
        return left + right;       
    }


    // If srcIsSender is true, the src session is the sender (originator) of the audit message,
    // otherwise the dst session is the sender.
    void DutyAuditMessageSender::sendAuditMessage(
        const TA_Base_Core::MessageType& messageType,
        const std::string& srcSessionStr,
        const std::string& dstSessionStr,
        bool srcIsSender,                         
        const std::string& subsystemIdList,
        const std::string& regionIdList,
        const std::string& msgDetailsStr )
    {
        TA_Base_Bus::SessionInfo srcSession( getSession(srcSessionStr) );
        std::string srcOpName( getOperatorNameFromSession(srcSession)) ;

        TA_Base_Bus::SessionInfo dstSession( getSession(dstSessionStr) );
        std::string dstOpName( getOperatorNameFromSession(dstSession) );

        TA_Base_Core::DescriptionParameters params;
		params.reserve(7);

        TA_Base_Core::NameValuePair param1("SrcOperatorId", concatStr(srcOpName, getProfileNameFromSession(srcSession)) );
        params.push_back(&param1);
        TA_Base_Core::NameValuePair param2("SrcLocationId", getLocationNameFromSession(srcSession));
        params.push_back(&param2);
        TA_Base_Core::NameValuePair param3("DstOperatorId", concatStr(dstOpName, getProfileNameFromSession(dstSession)) );
        params.push_back(&param3);
        TA_Base_Core::NameValuePair param4("DstLocationId", getLocationNameFromSession(dstSession));
        params.push_back(&param4);
        TA_Base_Core::NameValuePair param5("SubsystemIdList", subsystemIdList);
        params.push_back(&param5);
        TA_Base_Core::NameValuePair param6("RegionIdList", regionIdList);
        params.push_back(&param6);
        TA_Base_Core::NameValuePair param7("DataPointValue", srcIsSender ? srcOpName : dstOpName);  // "VALUE" field
        params.push_back(&param7);

		//azenitha
		{
			std::string temp = "TDACCESS: srcOpName = "; 
			temp += srcOpName; 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
			std::string temp2 = "TDACCESS: profile = "; 
			temp2 += getProfileNameFromSession(srcSession); 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp2.c_str()); 
			std::string temp3 = "TDACCESS: location = "; 
			temp3 += getLocationNameFromSession(srcSession); 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp3.c_str()); 
		}

		{
			std::string temp = "TDACCESS: dstOpName = "; 
			temp += dstOpName; 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp.c_str()); 
			std::string temp2 = "TDACCESS: profile = "; 
			temp2 += getProfileNameFromSession(dstSession); 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp2.c_str()); 
			std::string temp3 = "TDACCESS: location = "; 
			temp3 += getLocationNameFromSession(dstSession); 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, temp3.c_str()); 
		}		
		//azenitha

		// yanrong++: CL-19418
		std::string audiParamString;
		TA_Base_Core::gGetStringFromParameters(params, audiParamString);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[CL-19418] SendAuditMessage, messageType=%s, message parameters: %s",
			messageType.getTypeKey().c_str(), audiParamString.c_str());
		// ++yanrong

		try
		{
			m_auditSender->sendAuditMessage(
				messageType,
				TA_Base_Bus::TransactiveAgentInfo::m_entity,
				params, 
				msgDetailsStr,
				srcIsSender ? srcSessionStr : dstSessionStr, 
				"", "", "" );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"TDACCESS: Unknown exception");
		}			
    }


	void DutyAuditMessageSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl> FrontEndStateParent;
		FrontEndStateParent::detach();

		typedef TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>  AgentTerminateParent;
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}

	//azenitha
	void DutyAuditMessageSender::clearPendingAuditMessages( AuditMessageDataList& auditMessageDataList )
	{
		FUNCTION_ENTRY("clearPendingAuditMessages");

		//m_auditMessageDataList.clear();
		auditMessageDataList.clear();

		FUNCTION_EXIT;
	}

	void DutyAuditMessageSender::clearPendingAuditMessagesAll( AuditMessageDataAllList& auditMessageDataAllList )
	{
		FUNCTION_ENTRY("clearPendingAuditMessagesAll");

		//m_auditMessageDataAllList.clear();
		auditMessageDataAllList.clear();

		FUNCTION_EXIT;
	}

	void DutyAuditMessageSender::sendPendingAuditMessages( AuditMessageDataList& auditMessageDataList )
	{
		FUNCTION_ENTRY("sendPendingAuditMessages");

		for (int i=0; i<auditMessageDataList.size(); i++)
		{
			switch (auditMessageDataList[i].auditMessageType)
			{
				case AUDIT_ACCEPT:
				{
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyRequestAccept"); 
                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestAccept,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, false,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty accept" );

					{
						for (int j=0; j<auditMessageDataList[i].regionNames.size(); j++)
						{
							LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyLost"); 
							sendAuditMessage(
								TA_Base_Core::DutyAgentAudit::SubsystemDutyLost,
								auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, false,
								auditMessageDataList[i].subsystemName, auditMessageDataList[i].regionNames[j],
								"Subsystem duty lost" );
						}
					}

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyRequestAcceptReceived"); 
                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestAcceptReceived,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, true,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty accept received" );

					{
						for (int j=0; j<auditMessageDataList[i].regionNames.size(); j++)
						{
							LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyGain"); 
							sendAuditMessage(
								TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
								auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, true,
								auditMessageDataList[i].subsystemName, auditMessageDataList[i].regionNames[j],
								"Subsystem duty gain" );
						}
					}

					break;
				}
				case AUDIT_DENY:
				{
                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestRejected,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, false,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty denied" );

                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestRejectedReceived,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, true,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty denied received" );
					
					break;
				}
				case AUDIT_TIMEOUT:
				{
                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestTimeout,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, false,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty timeout" );

					{
						for (int j=0; j<auditMessageDataList[i].regionNames.size(); j++)
						{
							sendAuditMessage(
								TA_Base_Core::DutyAgentAudit::SubsystemDutyLost,
								auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, false,
								auditMessageDataList[i].subsystemName, auditMessageDataList[i].regionNames[j],
								"Subsystem duty lost" );
						}
					}

                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestTimeoutReceived,
                        auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, true,
                        auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
                        "Request subsystem duty timeout received" );

					{
						for (int j=0; j<auditMessageDataList[i].regionNames.size(); j++)
						{
							sendAuditMessage(
								TA_Base_Core::DutyAgentAudit::SubsystemDutyGain,
								auditMessageDataList[i].srcSession, auditMessageDataList[i].destSession, true,
								auditMessageDataList[i].subsystemName, auditMessageDataList[i].regionNames[j],
								"Subsystem duty gain" );
						}
					}

					break;
				}
				default:
				{
					break;
				}
			}
		}
		
		FUNCTION_EXIT;
	}

	void DutyAuditMessageSender::sendPendingAuditMessagesAll( AuditMessageDataAllList& auditMessageDataAllList )
	{
		FUNCTION_ENTRY("sendPendingAuditMessagesAll");

		for (int i=0; i<auditMessageDataAllList.size(); i++)
		{
			switch (auditMessageDataAllList[i].auditMessageType)
			{
				case AUDIT_ACCEPT:
				{
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyAllRequestAccept"); 
					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestAccept,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, false,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Request all subsystem duty accept" );

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyAllLost"); 
					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllLost,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, false,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Subsystem duty all lost" );

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyAllRequestAcceptReceived"); 
					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestAcceptReceived,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, true,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Request all subsystem duty accept received" );

					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Sent SubsystemDutyAllGain"); 
					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllGain,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, true,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Subsystem duty all gain" );

					break;
				}
				case AUDIT_DENY:
				{
					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestRejected,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, false,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Request all subsystem duty denied" );

					sendAuditMessage(
						TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestRejectedReceived,
						auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, true,
						"" /*subsystemName is not used*/, "" /*regionName is not used*/,
						"Request all subsystem duty denied received" );

					break;
				}
				case AUDIT_TIMEOUT:
				{
                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestTimeout,
                        auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, false,
                        "" /*subsystemName is not used*/, "" /*regionName is not used*/,
                        "Request all subsystem duty timeout" );

                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyAllLost,
                        auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, false,
                        "" /*subsystemName is not used*/, "" /*regionName is not used*/,
                        "Subsystem duty all lost" );

                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestTimeoutReceived,
                        auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, true,
                        "" /*subsystemName is not used*/, "" /*regionName is not used*/,
                        "Request all subsystem duty timeout received" );

                    sendAuditMessage(
                        TA_Base_Core::DutyAgentAudit::SubsystemDutyAllGain,
                        auditMessageDataAllList[i].srcSession, auditMessageDataAllList[i].destSession, true,
                        "" /*subsystemName is not used*/, "" /*regionName is not used*/,
                        "Subsystem duty all gain" );

					break;
				}
				default:
				{
					break;
				}
			}
		}
		
		FUNCTION_EXIT;
	}

	void DutyAuditMessageSender::pendingAuditMessageToSend(
		const std::string& srcSession,
		const std::string& destSession,
		const std::string& subsystemName,
		const std::string& regionName,
		const AuditMessageType& auditMessageType,
		AuditMessageDataList& auditMessageDataList)
	{
		FUNCTION_ENTRY("sendPendingAuditMessages");

		//TODO: use iterator

		//Add first element
		if (auditMessageDataList.size() == 0)
		{
			AuditMessageData auditMessageData;
			auditMessageData.srcSession = srcSession;
			auditMessageData.destSession = destSession;
			auditMessageData.subsystemName = subsystemName;
			auditMessageData.regionNames.push_back(regionName);
			auditMessageData.auditMessageType = auditMessageType;
			auditMessageDataList.push_back(auditMessageData);
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add first element");
		}
		else
		{
			//Find if existing
			bool bFound = false;
			for (int i=0; i<auditMessageDataList.size(); i++)
			{
				if (((auditMessageDataList[i].srcSession).compare(srcSession) == 0) &&
					((auditMessageDataList[i].destSession).compare(destSession) == 0) &&
					((auditMessageDataList[i].subsystemName).compare(subsystemName) == 0))
				{
					if (auditMessageDataList[i].regionNames.size() == 0)
					{
						auditMessageDataList[i].regionNames.push_back(regionName);
						LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add first region");
					}
					else
					{
						//Add regionName if not yet existing
						bool bFound2 = false;
						for (int j=0; j<auditMessageDataList[i].regionNames.size(); j++)
						{
							if ((auditMessageDataList[i].regionNames[j]).compare(regionName) == 0)
							{
								//Duplicate region, do not add
								bFound2 = true;
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Duplicate region, do not add");
								break;
							}
						}

						if (!bFound2)
						{
							auditMessageDataList[i].regionNames.push_back(regionName);
							LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add new region");
						}
					}

					//Duplicate subsystem, do not add
					bFound = true;
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Duplicate subsystem, do not add");
					break;
				}
			}

			if (!bFound)
			{
				AuditMessageData auditMessageData;
				auditMessageData.srcSession = srcSession;
				auditMessageData.destSession = destSession;
				auditMessageData.subsystemName = subsystemName;
				auditMessageData.regionNames.push_back(regionName);
				auditMessageData.auditMessageType = auditMessageType;
				auditMessageDataList.push_back(auditMessageData);
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add new element");
			}
		}
	
		FUNCTION_EXIT;
	}

	void DutyAuditMessageSender::pendingAuditMessageToSendAll(
		const std::string& srcSession,
		const std::string& destSession,
		const AuditMessageType& auditMessageType,
		AuditMessageDataAllList& auditMessageDataAllList)
	{
		FUNCTION_ENTRY("pendingAuditMessageToSendAll");

		//TODO: use iterator

		//Add first element
		if (auditMessageDataAllList.size() == 0)
		{
			AuditMessageDataAll auditMessageDataAll;
			auditMessageDataAll.srcSession = srcSession;
			auditMessageDataAll.destSession = destSession;
			auditMessageDataAll.auditMessageType = auditMessageType;
			auditMessageDataAllList.push_back(auditMessageDataAll);
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add first element");
		}
		else
		{
			//Find if existing
			bool bFound = false;
			for (int i=0; i<auditMessageDataAllList.size(); i++)
			{
				if (((auditMessageDataAllList[i].srcSession).compare(srcSession) == 0) &&
					((auditMessageDataAllList[i].destSession).compare(destSession) == 0))
				{
					//Duplicate, do not add
					bFound = true;
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Duplicate, do not add");
					break;
				}
			}

			if (!bFound)
			{
				AuditMessageDataAll auditMessageDataAll;
				auditMessageDataAll.srcSession = srcSession;
				auditMessageDataAll.destSession = destSession;
				auditMessageDataAll.auditMessageType = auditMessageType;
				auditMessageDataAllList.push_back(auditMessageDataAll);
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"TDACCESS: Add new element");
			}
		}
	
		FUNCTION_EXIT;
	}
	DutyAuditMessageWorkItem::DutyAuditMessageWorkItem(DutyAuditMessageSender& parent,boost::shared_ptr<TA_Base_Bus::DutyState> dutyState):
		parent_(parent),
		dutyState_(dutyState)
	{

	}
	DutyAuditMessageWorkItem::~DutyAuditMessageWorkItem()
	{

	}
	void DutyAuditMessageWorkItem::executeWorkItem()
	{
		parent_.update(*dutyState_);
	}
	//azenitha
}
