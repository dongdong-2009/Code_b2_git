/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/src/GroupMonitor.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2015/09/29 18:25:34 $
  * Last modified by:  $Author: limin.zhu $
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
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

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
          m_isGroupStarting(false)
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

		m_groupOfflineCheckTimeout = 0;
		if( true == TA_Base_Core::RunParams::getInstance().isSet(RPARAM_GROUPOFFLINECHECKTIMEOUT) )
		{
			m_groupOfflineCheckTimeout = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPOFFLINECHECKTIMEOUT).c_str());
		}
		ftime( &m_startTime );
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

		time_t diff = currentTime.time - m_startTime.time;
		if(diff < m_groupOfflineCheckTimeout && false == m_atLeastOneSystemControllerIsStarted)
		{
			return false;
		}
		TA_Base_Core::ThreadGuard guard(m_mapLock);	// CL-20810

        for ( HeartBeatTimeMap::iterator iter = m_timeLastHeartBeated.begin(); iter != m_timeLastHeartBeated.end(); ++iter )
        {
 			//struct tm *rectime;
 			//rectime = localtime(&(iter->second)); 

 			//std::ostringstream timeStr;
 			//timeStr << rectime->tm_mday << "/" << rectime->tm_mon << "/" << rectime->tm_year;
 			//timeStr << " " << rectime->tm_hour << ":" << rectime->tm_min << ":" << rectime->tm_sec;
 			//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
 			//	"[Group offline log] GroupMonitor::hasGroupFailed(): Last heartbeat recieved: %s", timeStr.str().c_str());

			// yanrong++: CL-21195, 2010-01-18
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
				"[Group offline log] GroupMonitor::hasGroupFailed(): Last heartbeat received for group %s: %lu:%lu, current: %lu:%lu", m_groupName.c_str(),
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
			double lastHeartbeatTimeMs = iter->second.time;
			lastHeartbeatTimeMs *= 1000;
			lastHeartbeatTimeMs += iter->second.millitm;

			double expectedTimeMs = currentTime.time;
			expectedTimeMs *= 1000;
			expectedTimeMs += currentTime.millitm;
			expectedTimeMs -= (m_heartbeatSendPeriod * m_heartbeatSendNumber);

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
			m_isGroupStarting = false;
			m_atLeastOneSystemControllerIsStarted = true;
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

	bool GroupMonitor::contains( std::string host )
	{
		HeartBeatTimeMap::iterator iter;
		iter = m_timeLastHeartBeated.find( host );

		return iter != m_timeLastHeartBeated.end();
	}

	void GroupMonitor::setGroupStarting()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupMonitor:setGroupStarting. groupName: %s", m_groupName.c_str());

		m_isGroupStarting = true;
		ftime( &m_groupStartingTime ); 
	}

	bool GroupMonitor::groupStartFailed()
	{
		if(m_isGroupStarting )
		{
			timeb currentTime;
			ftime( &currentTime );
			time_t diff = currentTime.time - m_groupStartingTime.time;
			if(diff > DEFAULT_GROUP_START_FAILED_TIMEOUT)
			{
				m_isGroupStarting = false;
				return true;
			}
		}

		return false;
	}
}
