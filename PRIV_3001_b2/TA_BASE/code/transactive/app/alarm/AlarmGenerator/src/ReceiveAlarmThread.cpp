/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/ReceiveAlarmThread.cpp,v $
 * @author:  Cameron Rochester
 * @version: $Revision: 1.1.2.1 $
 *
 * Last modification: $Date: 2003/06/23 05:00:14 $
 * Last modified by:  $Author: andrewdc $
 *
 * A thread that will recieve alarms sent across the wire while other threads are submitting.
 * It allows real load to be placed on the Alarm Agent
 */

#include <iostream>

#include "tools/integration/AlarmGenerator/src/ReceiveAlarmThread.h"

#include "core/utilities/src/DebugUtil.h"	
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/TESTALARM_MessageTypes.h"

using TA_Core::TA_Utilities::DebugUtil;
using TA_Core::TA_Alarm::AlarmSubscriptionManager;

ReceiveAlarmThread::ReceiveAlarmThread(const int entityTypeKey,const int totalAlarmCount) 
    : m_running(false), m_targetValue(totalAlarmCount), m_key(entityTypeKey),
    m_receivedAlarmCount(0)
{
	AlarmSubscriptionManager::getInstance().subscribeToAlarms(TA_Core::TA_Message::Alarm::AllAlarms,
		this);
}

ReceiveAlarmThread::~ReceiveAlarmThread()
{
	AlarmSubscriptionManager::getInstance().unsubscribeToMessages(this);
}


void ReceiveAlarmThread::run()
{
    LOG ( SourceInfo, DebugUtil::FunctionEntry, "ReceiveAlarmThread::run");
	m_running = true;


	// Resolve names
	while (m_running || m_receivedAlarmCount < m_targetValue)
	{
		TA_Core::TA_Thread::Thread::sleep(5);
	}
    LOG ( SourceInfo, DebugUtil::FunctionExit, "ReceiveAlarmThread::run");
}

void ReceiveAlarmThread::receiveSpecialisedMessage(const TA_Core::TA_Alarm::AlarmMessageCorbaDef& message)
{
	m_receivedAlarmCount++;
}

void ReceiveAlarmThread::terminate()
{
    LOG ( SourceInfo, DebugUtil::FunctionEntry, "ReceiveAlarmThread::terminate");
	m_running = false;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "     Receiving Alarms for Type: " << m_key <<  std::endl;
    std::cout << std::endl;
    std::cout << "-- Count" <<  std::endl;
    std::cout << "Total Alarms Received:\t\t" << m_receivedAlarmCount <<  std::endl;    
    std::cout << "----------------------------------------" <<  std::endl;
    LOG ( SourceInfo, DebugUtil::FunctionExit, "ReceiveAlarmThread::terminate");
}