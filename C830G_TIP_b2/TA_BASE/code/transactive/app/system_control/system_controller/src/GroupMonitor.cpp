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
    #pragma warning(disable:4786)
#endif

#include "GroupMonitor.h"

#include "core/utilities/src/DebugUtil.h"

#include <ctime>
#include <string>
#include <map>
#include <sstream>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // Constructor
    //
    GroupMonitor::GroupMonitor(const std::string& groupName, 
                    const std::vector<std::string>& groupHostnames, unsigned long heartbeatSendPeriod)
        : m_groupName(groupName), m_heartbeatSendPeriod(heartbeatSendPeriod)
    {
        FUNCTION_ENTRY("GroupMonitor::GroupMonitor"); //// INSTRUMENT_FEATURE_ENTRY("GroupMonitor", "Constructor");

		TA_Base_Core::ThreadGuard guard(m_mapLock);	// CL-20810

        // Initialise the time we last received a heartbeat from each peer.
        for (std::vector<std::string>::const_iterator iter = groupHostnames.begin(); 
            iter != groupHostnames.end(); iter++)
        {
            /*// Initialise the time of the last heartbeat to the current time. This will mean the group is
            // initially marked as being online.
            time_t current;
            time(&current);
            m_timeLastHeartBeated[*iter] = current;*/
            m_timeLastHeartBeated[ *iter ].time = 0;
            m_timeLastHeartBeated[ *iter ].millitm = 0;
        }

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
        timeb currentTime;
        ftime( &currentTime );

		TA_Base_Core::ThreadGuard guard(m_mapLock);	// CL-20810

        for ( HeartBeatTimeMap::iterator iter = m_timeLastHeartBeated.begin(); iter != m_timeLastHeartBeated.end(); ++iter )
        {
// 			struct tm *rectime;
// 			rectime = localtime(&(iter->second)); 
//             
// 			std::ostringstream timeStr;
// 			timeStr << rectime->tm_mday << "/" << rectime->tm_mon << "/" << rectime->tm_year;
// 			timeStr << " " << rectime->tm_hour << ":" << rectime->tm_min << ":" << rectime->tm_sec;
// 			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
// 				"[Group offline log] GroupMonitor::hasGroupFailed(): Last heartbeat recieved: %s", timeStr.str().c_str());

			// yanrong++: CL-21195, 2010-01-18
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
				"[Group offline log] GroupMonitor::hasGroupFailed(): Last heartbeat received: %lu:%lu, current: %lu:%lu", 
				iter->second.time, iter->second.millitm, currentTime.time, currentTime.millitm);
			// ++yanrong
            if ( 0 == iter->second.time && 0 == iter->second.millitm )
            {
                // initialize to current time in order not to raise false alarms during startup.
                iter->second = currentTime;
            }

            //if (iter->second >= expectedTime)
			// yanrong++: CL-21195, 2010-01-18
            //if ( ( iter->second.time * 1000.0 + iter->second.millitm ) >= ( ( currentTime.time - m_heartbeatSendPeriod * 2 ) * 1000.0 + currentTime.millitm ) )
			unsigned long heartbeatSendNumber = 2;
			double lastHeartbeatTimeMs = iter->second.time;
			lastHeartbeatTimeMs *= 1000;
			lastHeartbeatTimeMs += iter->second.millitm;

			double expectedTimeMs = currentTime.time;
			expectedTimeMs *= 1000;
			expectedTimeMs += currentTime.millitm;
			expectedTimeMs -= (m_heartbeatSendPeriod * heartbeatSendNumber);

			if(lastHeartbeatTimeMs >= expectedTimeMs)
			// ++yanrong
            {
                // Timestamp up-to-date. Group has not failed.
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
					"[Group offline log] GroupMonitor::hasGroupFailed(): return false");
                return false;
            }
        }

        // No up-to-date timestamps found.
		// Timestamp up-to-date. Group has not failed.
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
			"[Group offline log] GroupMonitor::hasGroupFailed(): return true");
        return true;
    }

    //
    // notifyHeartbeat
    //
    void GroupMonitor::notifyHeartbeat(const std::string& host)
    {
		TA_Base_Core::ThreadGuard guard(m_mapLock);	// CL-20810

        // Check to see if the heartbeat we just received is
        // from the group.
        HeartBeatTimeMap::iterator iter;
        iter = m_timeLastHeartBeated.find( host );
        
        if ( iter != m_timeLastHeartBeated.end() )
        {
            // Host found - heartbeat must have been from group.
            // Record the time it was received.
            ftime( &( iter->second ) );

             struct tm *rectime;
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

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
                "[Group offline log] GroupMonitor::notifyHeartbeat(): Host %s found. Time received: %s", host.c_str(), timeStr.str().c_str());
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "GroupMonitor::notifyHeartbeat(): Host %s is not in the monitored group.  Ignoring.", host.c_str());
        }
    }

    // 
    // getGroupName
    //
    std::string GroupMonitor::getGroupName() const
    {
        return m_groupName;
    }

}
