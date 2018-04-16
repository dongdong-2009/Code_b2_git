#if !defined(RIGHTSAGENTIMPL_H_INCLUDED)
#define RIGHTSAGENTIMPL_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/rights_agent/src/RightsAgentImpl.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2014/07/11 18:24:37 $
 * Last modified by:  $Author: haijun.li $
 *
 * This class implements the IRightsAgentCorbaDef interface.
 */

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

namespace TA_Base_Bus
{
    class GenericAgent; // Forward Declaration
	class AuthenticationLibrary;
}

namespace TA_Base_App
{
	class RightsAgentUser;
    class RightsAgentImpl : public virtual POA_TA_Base_Bus::IRightsAgentCorbaDef,
                                     public virtual TA_Base_Core::ServantBase
    {
    public:
        RightsAgentImpl(const std::string& agentName, 	RightsAgentUser* parentClass);
	    virtual ~RightsAgentImpl();

        TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse isActionPermittedOnResource(const char* sessionId,
																			      CORBA::ULong resourceKey, 
																				  CORBA::ULong actionKey,
																				  CORBA::String_out reasonString, 
																				  TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& 
																				  decisionModule);
		TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq* isActionPermittedOnResources(const char* sessionId,
																			      const TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq& resourceKeys, 
																				  CORBA::ULong actionKey,
																				  CORBA::String_out reasonString, 
																				  TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& 
																				  decisionModule);

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse isActionPermittedOnResourceWithoutDuty(const char* sessionId,
																			      CORBA::ULong resourceKey, 
																				  CORBA::ULong actionKey,
																				  CORBA::String_out reasonString, 
																				  TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& 
																				  decisionModule);
		TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq* areActionsPermittedOnResource(const char* sessionId,
			CORBA::ULong resourceKey, 
			const ::TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq& actionKeys,
			CORBA::String_out reasonString, 
			TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule);

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse isActionPermittedOnLocSub(const char* sessionId,			
																					CORBA::ULong actionKey,
																					CORBA::ULong locationKey,
																					CORBA::ULong subsystemKey,
																					CORBA::String_out reasonString, 
																					TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule);

		// yanrong
        void sessionUpdate(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			bool bSessionEnd);

		void agentSetControl();

		void stationOnLine(unsigned long locaitonKey);
		void stationOffLine(unsigned long locationKey);

    private:
        RightsAgentImpl(const RightsAgentImpl& theRightsImpl);
        RightsAgentImpl& operator=(const RightsAgentImpl& rhs);

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
	        privateIsActionPermittedOnResource(
				const char* sessionId, 
				CORBA::ULong resourceKey, 
				CORBA::ULong actionKey, 
				CORBA::String_out reasonString, 
				TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule);

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
			privateIsActionPermittedOnLocSub(
			const char* sessionId,		
			CORBA::ULong actionKey,
			CORBA::ULong locationKey,
			CORBA::ULong subsystemKey,
			CORBA::String_out reasonString, 
				TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule);

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
	        RightsAgentImpl::privateIsActionPermittedOnResourceWithoutDuty(
				const char* sessionId, 
				CORBA::ULong resourceKey, 
				CORBA::ULong actionKey, 
				CORBA::String_out reasonString, 
				TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule);
				
		//void initAuthAgent();
		void populateProfileList( const char* sessionId, std::list<unsigned long>& profileList,TA_Base_Bus::SessionInfo & aSessionInfo  );

		void checkOperationMode();
		
	private:
		TA_Base_Bus::XacmlFactory m_rightsFactory;
		std::auto_ptr<TA_Base_Bus::PolicySet> m_policy;

		//xinsong ++ 
		//just use authentication library, not directly use Authentication Agent interface

//		typedef TA_Base_Core::NamedObject<TA_Base_Bus::IAuthenticationAgentCorbaDef,
//					TA_Base_Bus::IAuthenticationAgentCorbaDef_ptr,
//					TA_Base_Bus::IAuthenticationAgentCorbaDef_var> AuthAgentType;
//
//		AuthAgentType* m_authAgent;

		TA_Base_Bus::AuthenticationLibrary m_authAgentLib;
			

		TA_Base_Core::NonReEntrantThreadLockable m_contextLock;
		TA_Base_Core::NonReEntrantThreadLockable m_policyLock;
		//TA_Base_Core::NonReEntrantThreadLockable m_profileCacheLock;

		TA_Base_Bus::SessionUpdateSubject m_sessionUpdates;
		SessionProfileCache  m_sessionCache;

		typedef std::map<std::string,TA_Base_Bus::SessionInfo> SessionInfoMap;
		SessionInfoMap m_sessionInfoMap;
		ACE_RW_Mutex m_mutex;
        /**
         * m_genericAgent
         *
         * A pointer to the Generic Agent instance for this agent.
         */
        RightsAgentUser* m_parentClass;
    };
}

#endif // !defined(RIGHTSAGENTIMPL_H_INCLUDED)
