/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Dirk McCormick
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Used by the JobQueue to execute jobs.
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
#include "app/scheduling/scheduling_agent/src/IJobExecutionListener.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"

#include "app/scheduling/scheduling_agent/src/JobRunner.h"


using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;


namespace TA_Base_App
{

JobRunner* JobRunner::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobRunner::m_singletonLock;


JobRunner& JobRunner::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobRunner();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


JobRunner::~JobRunner() {}

JobRunner::JobRunner()
    :
    m_listenerList(),
    m_jobThreadsToTerminateList(),
    m_running(false)
{
}


void JobRunner::run()
{
    m_running = true;

    int i = 1;
    while(m_running)
    {
        //
        // Every ten seconds, terminate and delete and JobThreads that have
        // completed.
        //
        Thread::sleep(1000);

        if(i >= 10)
        {
            terminateAndDeleteFinishedJobThreads();
            i = 1;
        }
        else
        {
            i ++;
        }
    }

    //
    // Make sure any left over threads are cleaned up before the thread exits.
    //
    terminateAndDeleteFinishedJobThreads();
}


void JobRunner::terminate()
{
    m_running = false;
}


unsigned long JobRunner::executeJob(IJob* job)
{
    TA_ASSERT(job != NULL, "Job must not be NULL");


    SchedulingTimer now;
    unsigned long minuteStarted = now.getTimeInMinutesSinceEpoch();

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Executing job with id %d at minute %d",
        job->getID(), minuteStarted);


    IJobInstance* instance = job->createJobInstance(minuteStarted);

    //
    // Get the session ID
    //
    std::string sessionID = "";
	try
	{
		sessionID = SchedulingSessionManager::getInstance().getSessionKey(job->getProfileID(), job->getLocationID());
	}
	catch (const TA_Base_Core::TransactiveException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
	}
	catch (const std::exception& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when get session key");
	}
        


    //
    // Create a thread in which to run the job,
    // inform the listeners that the job is about to start,
    // and start the job.
    //
    JobThread* thread = new JobThread(instance);
    TA_ASSERT(thread != NULL, "thread should never be NULL");

    fireStartingJobEvent(
        job->getID(), instance->getMinuteStarted(), sessionID);

    thread->start();

    return instance->getMinuteStarted();
}



void JobRunner::jobCompleted(
    JobThread* jobThread,
    const std::string& errorMessage)
{
    IJobInstance* jobInstance = jobThread->getJobInstance();
    TA_ASSERT(jobInstance != NULL, "Job instance must not be NULL");

    IJob* job = jobInstance->getJob();
    TA_ASSERT(job != NULL, "Job must not be NULL");


    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Job with id %d that was started at minute %d has completed",
        job->getID(), jobInstance->getMinuteStarted());


    //
    // If the error message is not blank, raise an alarm
    //
    if( ! errorMessage.empty() )
    {
        SchedulingAlarmSender::getInstance().jobExecutionFailed(
            job->getID(),
            jobInstance->getMinuteStarted(),
            job->getDescription(),
            errorMessage);
    }


    //
    // Get the session ID
    //
    std::string sessionID = "";
    try
    {
        sessionID =
            SchedulingSessionManager::getInstance().getSessionKey(
                job->getProfileID(), job->getLocationID());
    }
    catch(const CORBA::Exception& ex)
    {
        std::string errorMessage =
            "Exception while trying to retrieve session key for completed job "
            "with id '%d' and instance id '%d': '%s'";
        LOG_GENERIC(
            SourceInfo, DebugUtil::DebugError,
            errorMessage.c_str(),
            job->getID(),
            jobInstance->getMinuteStarted(),
            CorbaUtil::getInstance().exceptionToString(ex).c_str());
    }
    catch(...)
    {
        std::string errorMessage =
            "Unknown exception while trying to retrieve session key for "
            "completed job with id '%d' and instance id '%d'.";
        LOG_GENERIC(
            SourceInfo, DebugUtil::DebugError,
            errorMessage.c_str(),
            job->getID(),
            jobInstance->getMinuteStarted());
    }

    //
    // Inform the listeners that the job has completed,
    // and clean up.
    //

	//marvin++
	//TD13527
// 	if( errorMessage.empty() )
//     {
    // No matter there is error message or not,
    // Always remove the JobInstance from JobInstanceManager once the job is completed,
    // otherwise user can be able to delete the job in Scheduling Mangager
	fireJobCompletedEvent(
		job->getID(),
		jobInstance->getMinuteStarted(),
		errorMessage,
		sessionID);
/*	}*/
	//++marvin
	//TD13527

    addJobThreadToTerminateList(jobThread);
}


void JobRunner::addJobThreadToTerminateList(JobThread* jobThread)
{
    //
    // Lock the object
    //
    ThreadGuard lock(*this);

    m_jobThreadsToTerminateList.push_back(jobThread);
}

void JobRunner::terminateAndDeleteFinishedJobThreads()
{
    //
    // Lock the object
    //
    ThreadGuard lock(*this);

    while( ! m_jobThreadsToTerminateList.empty() )
    {
        //
        // Get the job thread, the job and the job instance.
        //
        JobThread* jobThread = *(m_jobThreadsToTerminateList.begin());

        IJobInstance* jobInstance = jobThread->getJobInstance();
        TA_ASSERT(jobInstance != NULL, "Job instance must not be NULL");


        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Terminating and deleting job thread and instance for "
                    "job instance was started at %d minutes after the epoch",
                    jobInstance->getMinuteStarted() );

        //
        // Delete the job instance.
        //
        delete jobInstance;
        jobInstance = NULL;
        
        //
        // Terminate the thread and delete it.
        //
        jobThread->terminateAndWait();
        delete jobThread;
        jobThread = NULL;


        //
        // Remove the entry from the list.
        //
        m_jobThreadsToTerminateList.pop_front();
    }
}



void JobRunner::fireStartingJobEvent(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& sessionID)
{
    for(JobExecutionListenerList::iterator it = m_listenerList.begin();
        it != m_listenerList.end();
        it ++)
    {
        TA_ASSERT( (*it) != NULL, "Listener must not be NULL" );

        (*it)->startingJob(jobID, minuteStarted, sessionID);
    }
}


void JobRunner::fireJobCompletedEvent(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& errorMessage,
    const std::string& sessionID)
{
    for(JobExecutionListenerList::iterator it = m_listenerList.begin();
        it != m_listenerList.end();
        it ++)
    {
        TA_ASSERT( (*it) != NULL, "Listener must not be NULL" );

        (*it)->jobCompleted(jobID, minuteStarted, errorMessage, sessionID);
    }
}


void JobRunner::addJobExecutionListener(IJobExecutionListener* listener)
{
    //
    // Lock the method
    //
    ThreadGuard lock(*this);

    m_listenerList.push_back(listener);
}


void JobRunner::removeJobExecutionListener(IJobExecutionListener* listener)
{
    //
    // Lock the method
    //
    ThreadGuard lock(*this);

    m_listenerList.remove(listener);
}



}; // TA_Base_App

