/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef SCHEDULEBROADCASTWORKER__INCLUDED_
#define SCHEDULEBROADCASTWORKER__INCLUDED_

#include <map>

#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

struct TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig;

namespace TA_IRS_App
{
// According ace 5.6 source code, the invalid timer id is -1
const long INVALID_TIMER_ID = -1;

const time_t MAX_DELAY_BROADCAST_TIME = 2;

class ScheduleBroadcast;

class ScheduleBroadcastWorker : public ACE_Event_Handler
{
public:
	ScheduleBroadcastWorker();
	virtual ~ScheduleBroadcastWorker();

    // ACE_Event_Handler interface
    virtual int handle_timeout( const ACE_Time_Value& current_time, const void* act = 0 );

    void updateScheduleBroadcastsFromDatabase();
    void clearScheduleBroadcastInLocalMap();
    
    void addNewScheduleBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refConfig );
    void updateScheduleBroadcastStatus( unsigned long ulScheduleBroadcastKey, const std::string& strStatus );
    void delScheduleBroadcast( unsigned long ulScheduleBroadcastKey, bool bJobDone, bool bForceDel = false );

    void setToControl();
    void setToMonitor();

private:
    typedef std::pair<long, ScheduleBroadcast*> TimerIdAndSchedulePair;
    typedef std::map<unsigned long, TimerIdAndSchedulePair> ScheduleKeyToTimerScheduleMap;
    ScheduleKeyToTimerScheduleMap m_mapScheduleBroadcasts;
    ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> m_thdScheduleTimer;
    bool m_bIsControl;
};

}

#endif // ifndef (SCHEDULEBROADCASTWORKER__INCLUDED_)
