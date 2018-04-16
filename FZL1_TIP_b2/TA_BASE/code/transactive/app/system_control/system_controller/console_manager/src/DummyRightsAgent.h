#pragma once
#include "Singleton.h"
#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"
#include "core/corba/src/ServantBase.h"

struct DummyRightsAgent
    : public virtual POA_TA_Base_Bus::IRightsAgentCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public Singleton<DummyRightsAgent>
{
public:

    DummyRightsAgent();

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

    void sessionUpdate(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, bool bSessionEnd);

protected:

    void checkOperationMode();
};
