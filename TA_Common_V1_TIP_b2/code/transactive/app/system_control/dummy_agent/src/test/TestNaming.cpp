#include "StdAfx.h"
#include "TestNaming.h"
#include "core/naming/src/NamedObject.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

TestNaming::TestNaming()
{
    testRunParam_NamingRepository();
}

void TestNaming::testRunParam_NamingRepository()
{
    RunParams::getInstance().set("NamingRepository", "corbaloc::occa:6666/OccConsoleManagerRepo");
    typedef TA_Base_Core::NamedObject<TA_Base_Bus::IAuthenticationAgentCorbaDef> AuthenticationAgentNamedObject;
    AuthenticationAgentNamedObject autheticationAgent("OccAuthenticationAgent", "OccAuthenticationAgent");

    try
    {
        std::string password;
        CORBA::String_var sessionId = autheticationAgent->requestSession(0, 0, 0, 0, password.c_str(), true);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TestNaming::testRunParam_NamingRepository - sessionId=%s", sessionId.in());
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", str(boost::format("TestNaming::testRunParam_NamingRepository - %s") % ex.what()));
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "unknown exception", "TestNaming::testRunParam_NamingRepository");
    }
}
