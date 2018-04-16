/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_controller/src/PeersMonitor.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2018/03/23 17:24:52 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is used to check if all heartbeats have been
  * received from peer System Controllers. When a heartbeat is
  * received from a System Controller, it is passed to this
  * class which checks to see if it's from a peer and if so
  * timestamps it. If a heartbeat is not received from ANY peer
  * after a certain time, the SystemControllerMonitor class
  * handles it.
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "PeersMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdPubUtility.h"
#include <boost/foreach.hpp>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace TA_Base_Core;

namespace TA_Base_App
{
    //
    // Constructor
    //
    PeersMonitor::PeersMonitor(const std::vector<std::string>& peerHostnames, unsigned long heartbeatSendPeriod, unsigned long heartbeatSendNumber)
        : m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber)
    {
        FUNCTION_ENTRY("PeersMonitor::PeersMonitor");

        // Initialise the time we last received a heartbeat from each peer.
        BOOST_FOREACH(const std::string& hostname, peerHostnames)
        {
            m_lastHeartbeatTimeMap[hostname].first.start();
            ftime(&m_lastHeartbeatTimeMap[hostname].second);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PeersMonitor::PeersMonitor(): add peer host %s.", hostname.c_str());
        }

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    PeersMonitor::~PeersMonitor()
    {
    }

    //
    // getFailedPeers
    //
    std::vector<std::string> PeersMonitor::getFailedPeers()
    {
        TA_THREADGUARD(m_lock);
        std::vector<std::string> failedPeers;
        const unsigned long TIMEOUT = m_heartbeatSendPeriod * m_heartbeatSendNumber;

        BOOST_FOREACH(HeartBeatTimeMap::value_type& v, m_lastHeartbeatTimeMap)

        for (HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.begin(); it != m_lastHeartbeatTimeMap.end(); ++it)
        {
            if (TIMEOUT < v.second.first.elapsed())
            {
                if (std::find(m_failedPeers.begin(), m_failedPeers.end(), v.first) == m_failedPeers.end())
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PeersMonitor::getFailedPeers(): got a failed peer [%s]; "
                                "last heartbeat:[%s]; elapsed:[%llu]; m_heartbeatSendPeriod:[%lu]; m_heartbeatSendNumber:[%lu]",
                                v.first.c_str(), CStdPubUtility::getTimeStr(&v.second.second).c_str(), v.second.first.elapsed(), m_heartbeatSendPeriod, m_heartbeatSendNumber);
                }

                // Timestamp not up-to-date. Add this peer to vector.
                failedPeers.push_back(v.first);
            }
        }

        m_failedPeers = failedPeers;
        return failedPeers;
    }

    //
    // notifyHeartbeat
    //
    void PeersMonitor::notifyHeartbeat(const std::string& host)
    {
        TA_THREADGUARD(m_lock);

        // Check to see if the heartbeat we just received is
        // from one of our peers.
        HeartBeatTimeMap::iterator it = m_lastHeartbeatTimeMap.find(host);

        if (it != m_lastHeartbeatTimeMap.end())
        {
            // Host found - heartbeat must have been from peer.
            // Record the time it was received.

            if ((m_heartbeatSendPeriod * 1.5) < it->second.first.elapsed())
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PeersMonitor::notifyHeartbeat(): Host %s found.", host.c_str());
            }

            it->second.first.start();
            ftime(&it->second.second);
        }
    }
} // namespace TA_Base_App
