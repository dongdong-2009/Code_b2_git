/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_controller/src/PeerSCInfo.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2018/03/23 17:24:52 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class maintains information about a peer
  * System Controller (or System Controller that is
  * observing a group we are in). It contains a pointer to
  * an IHeartbeatReceiverCorbaDef that it uses to send
  * heartbeats.
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786 4996 4819 4244)
#endif

#include "PeerSCInfo.h"
#include "Utilities.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/timers/src/MonotonicTimer.h"
#include <string>
#include <sstream>
#include <boost/format.hpp>
#include <sys/timeb.h>

#ifdef WIN32
    #include <sys/types.h>
    #define ftime _ftime   // #defines to make Windows behave like Unix.
    #define timeb _timeb
#endif

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // Constructor
    //
    PeerSCInfo::PeerSCInfo(const std::string& myHostname, const std::string& peerHostname, int heartbeatSendPeriod, int heartbeatSendNumber)
        : m_hostname(myHostname),
          m_peerHostname(peerHostname),
          m_isPeerResolved(false),
          m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber),
          m_systemControllerMonitor(TA_Base_Core::IHeartbeatReceiverCorbaDef::_nil()),
          m_isRunning(false),
          m_port(0),
          m_isPeerResolvedLastTime(boost::indeterminate)
    {
    }

    //
    // Destructor
    //
    PeerSCInfo::~PeerSCInfo()
    {
    }

    //
    // resolvePeer
    //
    void PeerSCInfo::resolvePeer()
    {
        FUNCTION_ENTRY("resolvePeer");

        m_isPeerResolved = false;
        m_systemControllerMonitor = TA_Base_Core::IHeartbeatReceiverCorbaDef::_nil();

        // Create a string in the form:
        std::string systemControllerMonitorString = str(boost::format("corbaloc::%s:%d/SystemControllerMonitor") % m_peerHostname % m_port);

        // Attempt to resolve an IHeartbeatReceiverCorbaDef object
        try
        {
            if (m_isPeerResolvedLastTime != m_isPeerResolved)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolving object: %s", systemControllerMonitorString.c_str());
            }

            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject(systemControllerMonitorString);

            if (!CORBA::is_nil(obj))
            {
                omniORB::setClientCallTimeout(obj, m_heartbeatSendPeriod);
                m_systemControllerMonitor = TA_Base_Core::IHeartbeatReceiverCorbaDef::_narrow(obj);
                m_isPeerResolved = true;
                omniORB::setClientCallTimeout(m_systemControllerMonitor, m_heartbeatSendPeriod);

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolved System Controller %s", m_peerHostname.c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error resolving peer SystemControllerMonitor: %s", m_peerHostname.c_str());
            }
        }
        catch (CORBA::SystemException& e)
        {
            if (m_isPeerResolvedLastTime != m_isPeerResolved)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException",
                                    str(boost::format("Error resolving peer SystemControllerMonitor: %s - %s") % m_peerHostname % TA_Base_Core::CorbaUtil::exceptionToString(e)));
            }
        }
        catch (CORBA::Exception& e)
        {
            if (m_isPeerResolvedLastTime != m_isPeerResolved)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                                    str(boost::format("Error resolving peer SystemControllerMonitor: %s - %s") % m_peerHostname % TA_Base_Core::CorbaUtil::exceptionToString(e)));
            }
        }
        catch (...)
        {
            if (m_isPeerResolvedLastTime != m_isPeerResolved)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", str(boost::format("Error resolving peer SystemControllerMonitor: %s") % m_peerHostname));
            }
        }

        m_isPeerResolvedLastTime = m_isPeerResolved;

        FUNCTION_EXIT;
    }

    //
    // beatHeart
    //
    void PeerSCInfo::beatHeart()
    {
        if (!m_isPeerResolved)
        {
            resolvePeer();

            if (m_isPeerResolved)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "begin sending heartbeat to peer %s", m_peerHostname.c_str());
            }
        }

        if (m_isPeerResolved)
        {
            try
            {
                m_systemControllerMonitor->receiveHeartbeat(m_hostname.c_str());
            }
            catch (const std::exception& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());

                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An error occurred while seding heartbeat");

                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
        }
    }

    //
    // run
    //
    void PeerSCInfo::run()
    {
        m_isRunning = true;
        m_isPeerResolved = false;
        TA_Base_Core::MonotonicTimer timer(true);
        m_port = Utilities::getMgrPort(m_peerHostname);

        while (m_isRunning)
        {
            timer.start();

            beatHeart();

            unsigned long long elapsedTime = timer.elapsed();
            unsigned long long sleepTime = (elapsedTime < m_heartbeatSendPeriod) ? m_heartbeatSendPeriod - elapsedTime : 0;

            sleep(sleepTime);
        }
    }

    //
    // terminate
    //
    void PeerSCInfo::terminate()
    {
        m_isRunning = false;
    }
}
