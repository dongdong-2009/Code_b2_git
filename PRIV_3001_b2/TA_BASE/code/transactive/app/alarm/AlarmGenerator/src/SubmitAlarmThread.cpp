/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/SubmitAlarmThread.cpp,v $
 * @author:  <Cameron Rochester>
 * @version: $Revision: 1.1.2.1 $
 *
 * Last modification: $Date: 2003/06/23 05:00:14 $
 * Last modified by:  $Author: andrewdc $
 * 
 * This class generates a specified number of alarms, and submits them to the alarm processor
 * as fast as possible. It is a thread so multiple instances of it can be fired off at once.
 */

#include <iostream>

#include "tools/integration/AlarmGenerator/src/SubmitAlarmThread.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/message/src/NameValuePair.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/TESTALARM_MessageTypes.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/utilities/src/Timer.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Core::TA_Utilities::DebugUtil;

void SubmitAlarmThread::run()
{
    LOG ( SourceInfo, DebugUtil::FunctionEntry, "SubmitAlarmThread::run");
    // Take a thread lock. This should prevent the main thread from terminating this thread
    // before the run() method finishes its job.
    TA_Core::TA_Synchronisation::ThreadGuard lock( m_instanceLock );

    TA_Core::TA_Alarm::NonUniqueAlarmHelper* nonUniqueHelper = 
		TA_Core::TA_Alarm::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper();

    TA_Core::TA_Message::DescriptionParameters dp;
	dp.push_back(new TA_Core::TA_Message::NameValuePair("AutoAlarmsName1", "ValueTest1"));
	dp.push_back(new TA_Core::TA_Message::NameValuePair("AutoAlarmsName2", "ValueTest2"));

    TA_Core::TA_Utilities::Timer timer;

    timer.restart();
    for(int i =0;i < m_alarmCount;i++)
    {
        nonUniqueHelper->submitAlarm(TA_Core::TA_Message::TESTALARM::TestAlarmType1,
            m_key,m_typeKey,dp,"blah","AlarmAutoAlarmsEntity",0,0);
	/*		m_alarmHelper->submitAlarm ( TA_Core::TA_Message::PMSAgentAlarms::DataPointTrueAlarm,
								 newAlarm->getEntityKey(),
								 newAlarm->getEntityTypeKey(),
								 descriptionParameters,
								 newAlarm->getAdditionalComments(),
								 newAlarm->getEntityName(),
								 newAlarm->getRegionKey(),
								 newAlarm->getSubsystemKey() );*/
        TA_Core::TA_Thread::Thread::sleep(m_submitDelay);
    }
    m_totalSubmitTime = timer.elapsedTime();
    m_averageSubmitTime = m_totalSubmitTime/m_alarmCount;
    LOG ( SourceInfo, DebugUtil::FunctionExit, "SubmitAlarmThread::run");
	
	while((unsigned int)0 < dp.size())
    {
		dp.erase(dp.end() - 1);
    }
}

void SubmitAlarmThread::terminate()
{
    LOG ( SourceInfo, DebugUtil::FunctionEntry, "SubmitAlarmThread::terminate");
    // Try to aquire the thread lock as taken above. This should ensure the rest of this
    // method will not execute until the run() method is finished
    TA_Core::TA_Synchronisation::ThreadGuard lock( m_instanceLock );

    TA_Core::TA_Alarm::AlarmHelper* helper = 
		TA_Core::TA_Alarm::AlarmHelperManager::getInstance().getAlarmHelper();

    TA_Core::TA_Utilities::Timer timer;

    timer.restart();
    helper->closeAlarm(TA_Core::TA_Message::TESTALARM::TestAlarmType1,m_key);
    m_totalCloseTime = timer.elapsedTime();

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "        Entity Key = " << m_key <<  std::endl;
    std::cout << std::endl;
    std::cout << "-- Submit" <<  std::endl;
    std::cout << "Total Alarms Submitted:\t\t" << m_alarmCount <<  std::endl;
    std::cout << "Total Time To Submit Alarms:\t" << m_totalSubmitTime << "s" <<  std::endl;
    std::cout << "Average Time per Alarm:\t\t" << (m_totalSubmitTime/m_alarmCount) << "s" <<  std::endl;
    std::cout << "Alarms Submitted per Second:\t" << (m_alarmCount/m_totalSubmitTime) << std::endl;
    std::cout << "-- Close" << std::endl;
    std::cout << "Total Calls to Close:\t\t1" <<  std::endl;
    std::cout << "Total Time to Close Alarms:\t" << m_totalCloseTime << "s" <<  std::endl;
    std::cout << "Average Time per Alarm:\t\t" << (m_totalCloseTime/m_alarmCount) << "s" <<  std::endl;
    std::cout << "Average Time per Close:\t\t" << (m_totalCloseTime/1) << "s" <<  std::endl;
    std::cout << "----------------------------------------" <<  std::endl;
    LOG ( SourceInfo, DebugUtil::FunctionExit, "SubmitAlarmThread::terminate");
    
}

double SubmitAlarmThread::getTotalSubmitTime()
{
    return m_totalSubmitTime;
}

double SubmitAlarmThread::getAverageSubmitTime()
{
    return m_averageSubmitTime;
}