#include "StdAfx.h"
#include "Utilities.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/threads/src/Thread.h"

using namespace TA_Base_Core;

static const char* DEFAULT_PASSWORD = "<empty>";
static const char* RPARAM_USERID = "UserId";
static const char* RPARAM_PROFILEID = "ProfileId";
static const char* RPARAM_LOCATIONID = "LocationId";
static const char* RPARAM_WORKSTATIONID = "WorkstationId";
static const char* RPARAM_PASSWORD = "Password";
static const unsigned int MAX_FAILED_TIMES = 20;

std::string Utilities::getLocationName()
{
    static const char* svrname = ACE_OS::getenv("SVRNAME");

    if (svrname)
    {
        return svrname;
    }

    if (RunParams::getInstance().isSet("LocationName"))
    {
        return RunParams::getInstance().get("LocationName");
    }

    return getLocationName(TA_Base_Core::getRunParamValue(RPARAM_LOCATIONKEY, 1), "Occ");
}

std::string Utilities::getLocationName(unsigned long locationKey, const std::string& defaultName)
{
    try
    {
        static std::vector<ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

        BOOST_FOREACH(ILocation* location, locations)
        {
            if (location->getKey() == locationKey)
            {
                std::string name = boost::to_lower_copy(location->getName());
                name[0] = ::toupper(name[0]);
                return name;
            }
        }
    }
    catch (const std::exception&)
    {
    }
    catch (...)
    {
    }

    return defaultName;
}

std::string Utilities::makeRunParameterName(const std::string& name)
{
    std::vector<std::string> vs;
    std::stringstream strm;

    if (boost::starts_with(name, "--"))
    {
        std::string temp = name.substr(2);
        boost::split(vs, temp, boost::is_any_of("-"), boost::token_compress_on);
    }
    else
    {
        boost::split(vs, name, boost::is_any_of("-"));
    }

    BOOST_FOREACH(std::string& s, vs)
    {
        if (s.size())
        {
            s[0] = ::toupper(s[0]);
            strm << s;
        }
    }

    return strm.str();
}

std::string Utilities::getDebugLogDir()
{
    if (RunParams::getInstance().isSet("DebugLogDir"))
    {
        return RunParams::getInstance().get("DebugLogDir");
    }

    static const char* serverlogdir = ACE_OS::getenv("SERVERLOGDIR");

    if (serverlogdir)
    {
        return serverlogdir;
    }

    static const char* mftlogdir = ACE_OS::getenv("MFTLOGDIR");

    if (mftlogdir)
    {
        return mftlogdir;
    }

    static const char* root = ACE_OS::getenv("ROOT");

    if (root)
    {
        return (boost::filesystem::path(root) / "log").string();
    }

    if (RunParams::getInstance().isSet(RPARAM_DEBUGFILE))
    {
        return boost::filesystem::path(RunParams::getInstance().get(RPARAM_DEBUGFILE)).parent_path().string();
    }

    return "";
}

void Utilities::checkConsoleManagerInstance(int port)
{
    std::string name = str(boost::format("Console Manager (%d)") % port);
    ::CreateMutex(NULL, FALSE, name.c_str());

    if (::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        std::cout << boost::format("\n\n\t***** %s Already Running *****\n\n") % name << std::endl;
        std::string line;
        std::getline(std::cin, line);
        ::exit(0);
    }
}

void Utilities::changeProfile(const std::string& profileIdParam, const std::string& locationIdParam, const std::string& passwordParam)
{
    if (!RunParams::getInstance().isSet(RPARAM_SESSIONID))
    {
        return;
    }

    try
    {
        TA_Base_Bus::AuthenticationLibrary authLib;
        std::string sessionId = authLib.changeProfile(RunParams::getInstance().get(RPARAM_SESSIONID),
                                                      strtoul(profileIdParam.c_str(), NULL, 10),
                                                      strtoul(locationIdParam.c_str(), NULL, 10),
                                                      passwordParam);

        RunParams::getInstance().set(RPARAM_PROFILEID, profileIdParam.c_str());
        RunParams::getInstance().set(RPARAM_LOCATIONID, locationIdParam.c_str());
        RunParams::getInstance().set(RPARAM_PASSWORD, passwordParam.c_str());
        RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
    }
    catch (const TA_Base_Core::AuthenticationSecurityException& ex)
    {
        std::cerr << "Couldn't change profile: " << ex.what() << std::endl;
    }
    catch (const TA_Base_Core::AuthenticationAgentException& ex)
    {
        std::cerr << "Caught TA_Base_Core::AuthenticationAgentException: " << ex.what() << std::endl;
    }
    catch (const TA_Base_Core::ObjectResolutionException& e)
    {
        std::cerr << "Caught TA_Base_Core::ObjectResolutionException: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Caught std::exception: " << typeid(e).name() << ", " << e.what() << std::endl;
    }
    catch (const CORBA::Exception& e)
    {
        std::cerr << "Caught CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception." << std::endl;
    }
}

bool Utilities::retrieveSessionId()
{
    if (RunParams::getInstance().isSet(RPARAM_USERID) ||
            RunParams::getInstance().isSet(RPARAM_PROFILEID) ||
            RunParams::getInstance().isSet(RPARAM_LOCATIONID) ||
            RunParams::getInstance().isSet(RPARAM_WORKSTATIONID) ||
            RunParams::getInstance().isSet(RPARAM_PASSWORD))
    {
        unsigned long userId = TA_Base_Core::getRunParamValue(RPARAM_USERID, 0);

        if (0 == userId)
        {
            std::cout << "Couldn't retrieve SessionId: Missing UserId parameter." << std::endl;
            return false;
        }

        unsigned long profileId = TA_Base_Core::getRunParamValue(RPARAM_PROFILEID, 0);

        if (0 == profileId)
        {
            std::cout << "Couldn't retrieve SessionId: Missing ProfileId parameter." << std::endl;
            return false;
        }

        unsigned long locationId = TA_Base_Core::getRunParamValue(RPARAM_LOCATIONID, 0);

        if (0 == locationId)
        {
            std::cout << "Couldn't retrieve SessionId: Missing LocationId parameter." << std::endl;
            return false;
        }

        unsigned long workstationId = TA_Base_Core::getRunParamValue(RPARAM_WORKSTATIONID, 0);

        if (0 == workstationId)
        {
            std::cout << "Couldn't retrieve SessionId: Missing WorkstationId parameter." << std::endl;
            return false;
        }

        if (RunParams::getInstance().get(RPARAM_LOCATIONKEY).empty())
        {
            std::cout << "Couldn't retrieve SessionId: Missing LocationKey parameter." << std::endl;
            return false;
        }

        std::string password = TA_Base_Core::getRunParamValue(RPARAM_WORKSTATIONID, DEFAULT_PASSWORD);
        const unsigned long sessionRetryPeriod = 5000; // 5 seconds
        bool resolvedAuthenticationAgent = false;
        std::string sessionId;
        TA_Base_Bus::AuthenticationLibrary authLib;
        unsigned int failedCount = 0;

        while (!resolvedAuthenticationAgent)
        {
            try
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Requesting session from Authentication Agent");
                sessionId = authLib.requestSession(userId, profileId, locationId, workstationId, password);
                resolvedAuthenticationAgent = true;
            }
            catch (TA_Base_Core::AuthenticationAgentException& ae)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent");
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationAgentException", ae.what());
                resolvedAuthenticationAgent = false;
            }
            catch (TA_Base_Core::AuthenticationSecurityException& ase)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent");
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationSecurityException", ase.what());
                resolvedAuthenticationAgent = false;
            }
            catch (...)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An unexpected error occured while requesting session from Authentication Agent");
                resolvedAuthenticationAgent = false;
            }

            if (!resolvedAuthenticationAgent)
            {
                failedCount++;

                if (MAX_FAILED_TIMES < failedCount)
                {
                    std::cout << "could not request session, retry " << failedCount << " times" << std::endl;
                }

                TA_Base_Core::Thread::sleep(sessionRetryPeriod);
            }
        }

        RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
        return true;
    }

    return false;
}
