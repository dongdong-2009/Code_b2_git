#include "StdAfx.h"
#include "DummyRightsAgent.h"
#include "Utilities.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

DummyRightsAgent::DummyRightsAgent()
{
    std::string agentName = Utilities::getLocationName() + "RightsAgent";
    CorbaUtil::getInstance().setServantKey(this, agentName);
    activateServantWithName(agentName);
}

void DummyRightsAgent::sessionUpdate(const IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, bool bSessionEnd)
{
}

TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse DummyRightsAgent::isActionPermittedOnResource(const char* sessionId,
                                                                                                 CORBA::ULong resourceKey,
                                                                                                 CORBA::ULong actionKey,
                                                                                                 CORBA::String_out reasonString,
                                                                                                 TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{
    checkOperationMode();
    reasonString = CORBA::string_dup("");
    return TA_Base_Bus::IRightsAgentCorbaDef::Permit;
}

TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse DummyRightsAgent::isActionPermittedOnLocSub(const char* sessionId,
                                                                                               CORBA::ULong actionKey,
                                                                                               CORBA::ULong locationKey,
                                                                                               CORBA::ULong subsystemKey,
                                                                                               CORBA::String_out reasonString,
                                                                                               TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{
    checkOperationMode();
    reasonString = CORBA::string_dup("");
    return TA_Base_Bus::IRightsAgentCorbaDef::Permit;
}

TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq* DummyRightsAgent::isActionPermittedOnResources(const char* sessionId,
                                                                                               const TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq& resourceKeys,
                                                                                               CORBA::ULong actionKey,
                                                                                               CORBA::String_out reasonString,
                                                                                               TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{
    checkOperationMode();
    reasonString = CORBA::string_dup("");
    return new TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq;
}

TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse DummyRightsAgent::isActionPermittedOnResourceWithoutDuty(const char* sessionId,
                                                                                                            CORBA::ULong resourceKey,
                                                                                                            CORBA::ULong actionKey,
                                                                                                            CORBA::String_out reasonString,
                                                                                                            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{
    checkOperationMode();
    reasonString = CORBA::string_dup("");
    return TA_Base_Bus::IRightsAgentCorbaDef::Permit;
}

TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq* DummyRightsAgent::areActionsPermittedOnResource(const char* sessionId,
                                                                                              CORBA::ULong resourceKey,
                                                                                              const ::TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq& actionKeys,
                                                                                              CORBA::String_out reasonString,
                                                                                              TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{
    checkOperationMode();
    reasonString = CORBA::string_dup("");
    return new TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq;
}

void DummyRightsAgent::checkOperationMode()
{
}
