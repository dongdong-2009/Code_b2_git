#pragma once
#include "Singleton.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "core/corba/src/ServantBase.h"

struct DummyAuthenticationAgent
    : public virtual POA_TA_Base_Bus::IAuthenticationAgentCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public Singleton<DummyAuthenticationAgent>
{
public:

    DummyAuthenticationAgent();
    char* requestSession(CORBA::ULong userId, CORBA::ULong profileId, CORBA::ULong locationId, CORBA::ULong workstationId, const char* password, CORBA::Boolean displayOnly = false);
    void changePassword(const char* sessionId, const char* oldPassword, const char* newPassword);
    void beginOverride(const char* sessionId, CORBA::ULong userId, CORBA::ULong profileId, CORBA::ULong locationId, const char* password);
    void endOverride(const char* sessionId);
    char* changeProfile(const char* sessionId, CORBA::ULong profileId, CORBA::ULong locationId, const char* password, CORBA::Boolean displayOnly = false);
    TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence* getProfiles(const char* sessionId);
    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef* getSessionInfo(const char* sessionId, const char* requestingSessionId);
    void endSession(const char* sessionId);
    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence* getActiveSessions(const char* sessionId);
    void keepSessionAlive(time_t timestamp, const char* sessionId);
    void beginDisplayonly(bool display);

protected:

    void checkOperationMode();
};
