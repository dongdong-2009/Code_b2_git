#pragma once
#include "StdAfx.h"
#include "Utilities.h"
#include "DummyAuthenticationAgent.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

static const std::string TransActiveSuperSessionID = "TransActive Super Session ID";

DummyAuthenticationAgent::DummyAuthenticationAgent()
{
    std::string agentName = Utilities::getLocationName() + "AuthenticationAgent";
    CorbaUtil::getInstance().setServantKey(this, agentName);
    activateServantWithName(agentName);
}

char* DummyAuthenticationAgent::requestSession(CORBA::ULong userId, CORBA::ULong profileId, CORBA::ULong locationId, CORBA::ULong workstationId, const char* password, CORBA::Boolean displayOnly)
{
    checkOperationMode();
    return CORBA::string_dup(TransActiveSuperSessionID.c_str());
}

void DummyAuthenticationAgent::changePassword(const char* sessionId, const char* oldPassword, const char* newPassword)
{
    checkOperationMode();
}

void DummyAuthenticationAgent::beginOverride(const char* sessionId, CORBA::ULong userId, CORBA::ULong profileId, CORBA::ULong locationId, const char* password)
{
    checkOperationMode();
}

void DummyAuthenticationAgent::endOverride(const char* sessionId)
{
    checkOperationMode();
}

char* DummyAuthenticationAgent::changeProfile(const char* sessionId,  CORBA::ULong profileId,  CORBA::ULong locationId,  const char* password, CORBA::Boolean displayOnly)
{
    checkOperationMode();
    return CORBA::string_dup(TransActiveSuperSessionID.c_str());
}

TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence*  DummyAuthenticationAgent::getProfiles(const char* sessionId)
{
    checkOperationMode();
    TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence_var profiles = new TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence;
    return profiles._retn();
}

TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef* DummyAuthenticationAgent::getSessionInfo(const char* sessionId, const char* requestingSessionId)
{
    checkOperationMode();
    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef_var session = new TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef;
    return session._retn();
}

void DummyAuthenticationAgent::endSession(const char* sessionId)
{
    checkOperationMode();
}

TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence* DummyAuthenticationAgent::getActiveSessions(const char* sessionId)
{
    checkOperationMode();
    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence_var sessions = new TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence;
    return sessions._retn();
}

void DummyAuthenticationAgent::keepSessionAlive(time_t timestamp, const char* sessionId)
{
    checkOperationMode();
}

void DummyAuthenticationAgent::checkOperationMode()
{
}

void DummyAuthenticationAgent::beginDisplayonly(bool display)
{
}
