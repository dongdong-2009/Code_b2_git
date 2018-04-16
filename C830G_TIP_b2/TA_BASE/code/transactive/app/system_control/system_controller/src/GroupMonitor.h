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
  
#if !defined(GroupMonitor_7F03A06B_776D_4f74_A432_E38D5A1214D8__INCLUDED_)
#define GroupMonitor_7F03A06B_776D_4f74_A432_E38D5A1214D8__INCLUDED_

#include <string>
#include <map>
#include <vector>
#include <sys/timeb.h>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"	//CL-20810

namespace TA_Base_App
{

    class GroupMonitor
    {

    public:

        /**
          * Constructor
          *
          * @param groupName       The name of the group we are monitoring.
          * @param groupHostnames  A vector of hostnames of the System Controllers
          *                        belonging to the group.
          * 
          */
        GroupMonitor(const std::string& groupName, 
                    const std::vector<std::string>& groupHostnames, unsigned long heartbeatSendPeriod);

        /**
          * Destructor
          */
	    virtual ~GroupMonitor();

        /** 
          * hasGroupFailed
          *
          * Checks the timestamps to see if at least one group 
          * member has sent us a heartbeat within the last time frame.
          *
          * @return True if the group has failed, and false if the
          *         group hasn't (ie at least one heartbeat was received).
          *
          *
          */
        bool hasGroupFailed();

        /** 
          * notifyHeartbeat
          *
          * Called to inform this class that a heartbeat has been received
          * from another System Controller.
          *
          * @param host The hostname of the System Controller that
          *             sent the heartbeat.
          */
	    void notifyHeartbeat(const std::string& host);

        /** 
          * getGroupName
          *
          * Returns the name of the group that this object is monitoring.
          *
          * @return The name of the group.
          *
          */
        std::string getGroupName() const;

    private:
   	    GroupMonitor(const GroupMonitor& theGroupMonitor);
   		GroupMonitor& operator=(const GroupMonitor&);

        /**
          * The name of this group.
          */
        std::string m_groupName;

   	    /**
          * A map of hostnames to timestamps.
          */
	    //std::map<std::string, time_t> m_timeLastHeartBeated;//limin--
        typedef std::map< std::string, timeb > HeartBeatTimeMap;
        HeartBeatTimeMap m_timeLastHeartBeated;//limin++

		//zn++ CL-20810
		TA_Base_Core::NonReEntrantThreadLockable m_mapLock;
		//++zn

        /**
          * The interval, in milliseconds, between heartbeats.
          */
        unsigned long m_heartbeatSendPeriod;
    };

} // namespace TA_Base_App
#endif // !defined(GroupMonitor_7F03A06B_776D_4f74_A432_E38D5A1214D8__INCLUDED_)


