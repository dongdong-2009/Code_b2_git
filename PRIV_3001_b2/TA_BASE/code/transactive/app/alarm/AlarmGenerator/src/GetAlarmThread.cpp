/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/GetAlarmThread.cpp,v $
 * @author:  Cameron Rochester
 * @version: $Revision: 1.1.2.1 $
 *
 * Last modification: $Date: 2003/06/23 05:00:14 $
 * Last modified by:  $Author: andrewdc $
 *
 * A thread to call getAlarms() on the Alarm Agent while other threads are attempting to 
 * submit, receive, close, etc.
 */

#include "tools/integration/AlarmGenerator/src/GetAlarmThread.h"

#include "core/utilities/src/Timer.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Core::TA_Utilities::DebugUtil;

//#include 
//#include "AlarmMessageCorbaDef.idl"

GetAlarmThread::GetAlarmThread(const int sessionId,const bool ackAlarms) : 
m_alarms(NULL), m_ackAlarms(ackAlarms), m_sessionId(sessionId)
{
    // Set the name on the AlarmProcessor NamedObjRef
    m_alarmProcessor.setName(TA_Core::TA_Alarm::ALARM_AGENT_NAME);
}

void GetAlarmThread::run()
{
    // Take a thread lock. This should prevent the main thread from terminating this thread
    // before the run() method finishes its job.
    TA_Core::TA_Synchronisation::ThreadGuard lock( m_instanceLock );

    LOG ( SourceInfo, DebugUtil::FunctionEntry, "GetAlarmThread::run");

    TA_Core::TA_Utilities::Timer timer;
    // Get all active alarms
    try
    {
        timer.restart();
        // Get all the active alarms currently in the system.
	    m_alarms = m_alarmProcessor->getAlarms();
        m_getTime = timer.elapsedTime();
    }
    catch( const TA_Core::TA_Exceptions::ObjectResolutionException& ex )
    {
	    LOG(SourceInfo, 
		    TA_Core::TA_Utilities::DebugUtil::ExceptionCatch,
		    "ObjectResolutionException",
		    ex.what());
        std::cout << "getAlarms() did not succeed" << std::endl;
    }
    catch( ... )
    {
	    LOG(SourceInfo, 
		    TA_Core::TA_Utilities::DebugUtil::ExceptionCatch,
		    "UnknownException",
		    "Thrown while trying to get an alarm");
        std::cout << "getAlarms() did not succeed" << std::endl;
    }

    if (m_ackAlarms)
    {
        // need to create a sequence of alarm ids
        TA_Core::TA_Alarm::StringSequence alarmIds;
        alarmIds.length(m_alarms->length());
        for (unsigned int i = 0; i < m_alarms->length();i++)
        {
            alarmIds[i] = CORBA::string_dup((*m_alarms)[i].alarmID);
        }

        // Now submit the sequence to be closed
        try
        {
            timer.restart();
            // Get all the active alarms currently in the system.
	        m_alarmProcessor->acknowledgeSeq(alarmIds,m_sessionId);
            m_ackTime = timer.elapsedTime();
        }
        catch( const TA_Core::TA_Exceptions::ObjectResolutionException& ex )
        {
	        LOG(SourceInfo, 
		        TA_Core::TA_Utilities::DebugUtil::ExceptionCatch,
		        "ObjectResolutionException",
		        ex.what());
            std::cout << "getAlarms() did not succeed" << std::endl;
        }
        catch( ... )
        {
	        LOG(SourceInfo, 
		        TA_Core::TA_Utilities::DebugUtil::ExceptionCatch,
		        "UnknownException",
		        "Thrown while trying to get an alarm");
            std::cout << "getAlarms() did not succeed" << std::endl;
        }
    }
    LOG ( SourceInfo, DebugUtil::FunctionExit, "GetAlarmThread::run");
}

void GetAlarmThread::terminate()
{
    // Try to aquire the thread lock as taken above. This should ensure the rest of this
    // method will not execute until the run() method is finished
    TA_Core::TA_Synchronisation::ThreadGuard lock( m_instanceLock );

    LOG ( SourceInfo, DebugUtil::FunctionEntry, "GetAlarmThread::terminate");
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "     Getting all active alarms"<<  std::endl;
    std::cout << std::endl;
    std::cout << "-- Count" <<  std::endl;
    std::cout << "Total Alarms Retrieved:\t\t" << m_alarms->length() <<  std::endl; 
    std::cout << "-- Times" <<  std::endl;   
    std::cout << "Total Time to Get Alarms:\t\t" << m_getTime << std::endl;
    if (m_ackAlarms)
    {
        std::cout << "Total Time to Ack Alarms:\t" << m_ackTime << std::endl;
    }
    else
    {
        std::cout << "Total Time to Ack Alarms:\tN/A" << std::endl;
    }
    std::cout << "----------------------------------------" <<  std::endl;
    LOG ( SourceInfo, DebugUtil::FunctionExit, "GetAlarmThread::terminate");
}