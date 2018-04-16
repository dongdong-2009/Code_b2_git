/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is used to check if all heartbeats have been
  * received from members of a group of System Controllers.
  * When a heartbeat is received from a System Controller,
  * it is passed to this class which checks to see if it's
  * from a group member and if so timestamps it. If a
  * heartbeat is not received from ALL group members after a
  * certain time, the SystemControllerMonitor class handles it.
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786 4819 4244)
#endif

#include "GroupMonitor.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdPubUtility.h"
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <ctime>
#include <string>
#include <map>
#include <sstream>

using TA_Base_Core::DebugUtil;

const long TA_Base_App::GroupMonitor::DEFAULT_GROUP_START_FAILED_TIMEOUT  = 180;
namespace TA_Base_App
{
    //
    // Constructor
    //
    GroupMonitor::GroupMonitor(const std::string& groupName, const std::vector<std::string>& groupHostnames, unsigned long heartbeatSendPeriod, unsigned long heartbeatSendNumber)
        : m_groupName(groupName),
          m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber),
          m_atLeastOneSystemControllerIsStarted(false),
          m_isGroupStarting(false),
          m_isGroupOffline(boost::indeterminate)
    {
        FUNCTION_ENTRY("GroupMonitor::GroupMonitor"); //// INSTRUMENT_FEATURE_ENTRY("GroupMonitor", "Constructor");

        // Initialise the time we last received a heartbeat from each peer.
        BOOST_FOREACH(const std::string& hostname, groupHostnames)
        {
            // Initialise the time of the last heartbeat to the current time. This will mean the group is
            // initially marked as being online.
            m_lastHeartbeatTimeMap[hostname].first.start();
            ftime(&m_lastHeartbeatTimeMap[hostname].second);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupMonitor::GroupMonitor(): add host %s to group %s.", hostname.c_str(), m_groupName.c_str());
        }

        m_groupOfflineCheckTimeout = 0;

        if (true == TA_Base_Core::RunParams::getInstance().isSet(RPARAM_GROUPOFFLINECHECKTIMEOUT))
        {
            m_groupOfflineCheckTimeout = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPOFFLINECHECKTIMEOUT).c_str());
        }

        ftime(&m_startTime);
        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    GroupMonitor::~GroupMonitor()
    {
    }

    //
    // hasGroupFailed
    //
    bool GroupMonitor::hasGroupFailed()
    {
        TA_THREADGUARD(m_lock);

        const unsigned long TIMEOUT = m_heartbeatSendPeriod * m_heartbeatSendNumber;
        bool isGroupOffline = true;
        std::vector<std::string> logs;
        HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.begin();

        for (; it != m_lastHeartbeatTimeMap.end(); ++it)
        {
            logs.push_back(str(boost::format("GroupMonitor::hasGroupFailed(): "
                                             "host:[%s]; group:[%s]; last heartbeat:[%s]; elapsed:[%llu]; "
                                             "m_heartbeatSendPeriod:[%lu]; heartbeat send number:[%lu]")
                               % it->first % m_groupName % TA_Base_Core::CStdPubUtility::getTimeStr(&it->second.second)
                               % it->second.first.elapsed() % m_heartbeatSendPeriod % m_heartbeatSendNumber));

            if (it->second.first.elapsed() < TIMEOUT)
            {
                break;
            }
        }

        isGroupOffline = (it == m_lastHeartbeatTimeMap.end());

        if (isGroupOffline != m_isGroupOffline)
        {
            m_isGroupOffline = isGroupOffline;

            BOOST_FOREACH(const std::string& s, logs)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", s.c_str());
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupMonitor::hasGroupFailed(): return %s, group:%s", (isGroupOffline ? "true" : "false"), m_groupName.c_str());
        }

        return isGroupOffline;
    }

    //
    // notifyHeartbeat
    //
    void GroupMonitor::notifyHeartbeat(const std::string& host)
    {
        TA_THREADGUARD(m_lock);// CL-20810

        // Check to see if the heartbeat we just received is from the group.
        HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.find(host);

        if (it != m_lastHeartbeatTimeMap.end())
        {
            if ((m_heartbeatSendPeriod * 1.5) < it->second.first.elapsed())
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupMonitor::notifyHeartbeat(): Host %s found, group %s.", host.c_str(), m_groupName.c_str());
            }

            // Host found - heartbeat must have been from group.
            // Record the time it was received.
            it->second.first.start();
            ::ftime(&it->second.second);
        }
    }

    //
    // getGroupName
    //
    std::string GroupMonitor::getGroupName() const
    {
        return m_groupName;
    }

    bool GroupMonitor::contains(const std::string& host)
    {
        HeartBeatTimeMap::iterator iter;
        iter = m_lastHeartbeatTimeMap.find(host);
        return iter != m_lastHeartbeatTimeMap.end();
    }

    void GroupMonitor::setGroupStarting()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupMonitor:setGroupStarting. groupName: %s", m_groupName.c_str());
        m_isGroupStarting = true;
        ftime(&m_groupStartingTime);
    }

    bool GroupMonitor::groupStartFailed()
    {
        if (m_isGroupStarting)
        {
            timeb currentTime;
            ftime(&currentTime);
            time_t diff = currentTime.time - m_groupStartingTime.time;

            if (diff > DEFAULT_GROUP_START_FAILED_TIMEOUT)
            {
                m_isGroupStarting = false;
                return true;
            }
        }

        return false;
    }
}
