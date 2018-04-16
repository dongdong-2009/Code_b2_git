/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobRunner.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Used by the JobQueue to execute jobs.
  *
  */


#ifndef JOB_RUNNER_H
#define JOB_RUNNER_H


#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/scheduling/scheduling_agent/src/JobThread.h"
#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
//#include "app/scheduling/scheduling_agent/src/JobMonitorThread.h"
//#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"


#include <string>
#include <list>


namespace TA_Base_App
{


class IJobExecutionListener;




class JobRunner :
    public TA_Base_Core::NonReEntrantThreadLockable, public TA_Base_Core::Thread
{

public:


    /**
     *  Gets the single instance of this class.
     */
    static JobRunner& getInstance();

    /**
     *  Destructor.
     */
	virtual ~JobRunner();


	/**
     *  Creates a new instance of the given job, and runs it.
     *
     *  @return the minute at which the job instance was started.
     *
     *  @exception AuthenticationSecurityException
     *  Thrown if the log in attempt to the Authentication Agent
     *  fails for security reasons.
     *
     *  @exception AuthenticationAgentException
     *  Thrown if there is a general error with the log in attempt to the
     *  Authentication Agent.
     *
     *  @exception ObjectResolutionException
     *  Thrown if there is an issue resolving the Authentication Agent
     *
     *  @exception JobInstanceCreationException
     *  Thrown if for any reason a job instance cannot be created.
     */
    unsigned long executeJob(IJob* job);
    
	
    /**
     *  Called by JobThread objects to indicate when a job has completed.
     */
    void jobCompleted(
        JobThread* jobThread,
        const std::string& errorMessage);


    /**
     *  Adds a listener to the list of objects that receive events when
     *  a job starts and finishes executing.
     */
	void addJobExecutionListener(IJobExecutionListener* listener);


    /**
     *  Removes a listener from the list of objects that receive events when
     *  a job starts and finishes executing.
     */
	void removeJobExecutionListener(IJobExecutionListener* listener);



private:

    JobRunner();
    JobRunner(const JobRunner&);
    const JobRunner& operator=(const JobRunner&);


    /**
     *  Called by Thread's start method.
     */
    void run();

    /**
     *  Called by Thread's terminateAndWait method.
     */
    void terminate();

    /**
     *  Adds the job to the list of JobThread pointers that will be terminated
     *  and deleted when the terminateAndDeleteFinishedJobThreads() method is
     *  called.
     */
    void addJobThreadToTerminateList(JobThread* jobThread);

    /**
     *  Terminates and deletes any threads that are in the list,
     *  and deletes the JobInstances associated with them.
     */
    void terminateAndDeleteFinishedJobThreads();


    /**
     *  Informs all listeners that a job is about to start.
     */
    void JobRunner::fireStartingJobEvent(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& sessionID);


    /**
     *  Informs all listeners that a job has just completed.
     */
    void fireJobCompletedEvent(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& errorMessage,
        const std::string& sessionID);



    typedef std::list< IJobExecutionListener* > JobExecutionListenerList;
    /**
     *  The list of objects that are listening for StartingJob and
     *  JobCompleted events.
     */
    JobExecutionListenerList m_listenerList;


    /**
     *  The list of JobThreads that have completed and need to be
     *  terminateAndWaited.
     */
    std::list< JobThread* > m_jobThreadsToTerminateList;


    /**
     *  Indicates whether the thread is currently running.
     */
    volatile bool m_running;


    /** The single instance of this class */
    static JobRunner* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};


}; // TA_Base_App

#endif // JOB_RUNNER_H
