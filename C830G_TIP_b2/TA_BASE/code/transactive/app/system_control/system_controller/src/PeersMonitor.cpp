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

#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <sstream>
#include <ctime>

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    //
    // Constructor
    //
    PeersMonitor::PeersMonitor(const std::vector<std::string>& peerHostnames, unsigned long heartbeatSendPeriod)
        : m_heartbeatSendPeriod(heartbeatSendPeriod)
    {
        FUNCTION_ENTRY("Constructor");

        // Initialise the time we last received a heartbeat from each peer.
        for (std::vector<std::string>::const_iterator  iter = peerHostnames.begin(); 
              iter != peerHostnames.end(); ++iter )
        {
            m_timeLastHeartBeated[ *iter ].time = 0;
            m_timeLastHeartBeated[ *iter ].millitm = 0;
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
        std::vector<std::string> failedPeers;

        timeb currentTime;
        ftime( &currentTime );
        for ( HeartBeatTimeMap::iterator iter = m_timeLastHeartBeated.begin(); iter != m_timeLastHeartBeated.end(); ++iter )
        {
			// yanrong++: CL-21195, 2010-01-18
			const unsigned long heartbeatSendNumber = 2;
            //LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
            //    "PeersMonitor::getFailedPeers(): currentTime %lu; last heartbeat: %lu; m_heartbeatSendPeriod %lu; expected last heartbeat: %lu", 
            //    currentTime.time, iter->second.time, m_heartbeatSendPeriod, (currentTime.time - m_heartbeatSendPeriod * 2));
			// ++yanrong

            if ( 0 == iter->second.time && 0 == iter->second.millitm )
            {
                // initialize to current time in order not to raise false alarms during startup.
                iter->second = currentTime;
            }            

			// yanrong++: CL-21195, 2010-01-18
            //if ( ( iter->second.time * 1000.0 + iter->second.millitm ) < ( ( currentTime.time - m_heartbeatSendPeriod * 2 ) * 1000.0 + currentTime.millitm ) )
			double lastHeartbeatTimeMs = iter->second.time;
			lastHeartbeatTimeMs *= 1000;
			lastHeartbeatTimeMs += iter->second.millitm;

			double expectedTimeMs = currentTime.time;
			expectedTimeMs *= 1000;
			expectedTimeMs += currentTime.millitm;
			expectedTimeMs -= (m_heartbeatSendPeriod * heartbeatSendNumber);

			double currentTimeMs = currentTime.time;
			currentTimeMs *= 1000;
			currentTimeMs += currentTime.millitm;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                "PeersMonitor::getFailedPeers(): currentTime [%lu:%lu]; last heartbeat: [%lu:%lu]; "
				"m_heartbeatSendPeriod [%lu]; heartbeat send number:[%lu];"
				"last heartbeat elapse [%.0f]",
                currentTime.time, currentTime.millitm, iter->second.time, iter->second.millitm, 
				m_heartbeatSendPeriod, heartbeatSendNumber,
				currentTimeMs - lastHeartbeatTimeMs);

			if(lastHeartbeatTimeMs < expectedTimeMs)
			// ++yanrong
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, 
					"PeersMonitor::getFailedPeers(): got a failed peer [%s];"
					"currentTime [%lu:%lu]; last heartbeat: [%lu:%lu]; "
					"m_heartbeatSendPeriod [%lu]; heartbeat send number:[%lu];"
					"last heartbeat elapse [%.0f]",
					iter->first.c_str(),
					currentTime.time, currentTime.millitm, iter->second.time, iter->second.millitm, 
					m_heartbeatSendPeriod, heartbeatSendNumber,
					currentTimeMs - lastHeartbeatTimeMs);

                // Timestamp not up-to-date. Add this peer to vector.
                failedPeers.push_back( iter->first );
            }
        }

        return failedPeers;
    }

    //
    // notifyHeartbeat
    //
    void PeersMonitor::notifyHeartbeat(const std::string& host)
    {
        // Check to see if the heartbeat we just received is
        // from one of our peers.
        HeartBeatTimeMap::iterator iter;
        iter = m_timeLastHeartBeated.find(host);
        
        if (iter != m_timeLastHeartBeated.end())
        {
            // Host found - heartbeat must have been from peer.
            // Record the time it was received.
            ftime( &( iter->second ) );
            
            struct tm *rectime;
/*          rectime = localtime( &( iter->second.time ) );
            
            std::ostringstream timeStr;
            timeStr << rectime->tm_mday << "/" << rectime->tm_mon << "/" << rectime->tm_year;
            timeStr << " " << rectime->tm_hour << ":" << rectime->tm_min << ":" << rectime->tm_sec << ":" << iter->second.millitm;
*///--RenQiang
//RenQiang++
#ifndef WIN32
			struct tm stm;
			localtime_r(&iter->second.time, &stm);
			rectime = &stm;
#else
            rectime = localtime(&(iter->second.time)); 
#endif		
            std::ostringstream timeStr;
            timeStr << rectime->tm_mday << "/" << rectime->tm_mon + 1 << "/" << rectime->tm_year + 1900;
            timeStr << " " << rectime->tm_hour << ":" << rectime->tm_min << ":" << rectime->tm_sec << "." << iter->second.millitm;
//++RenQiang
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
                "PeersMonitor::notifyHeartbeat(): Host %s found. Time received: %s", host.c_str(), timeStr.str().c_str());
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PeersMonitor::notifyHeartbeat(): Host %s is not in peer monitor group.  Ignoring.", host.c_str());
        }
    }

} // namespace TA_Base_App
