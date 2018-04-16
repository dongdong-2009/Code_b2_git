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
  * This class keeps track of the instances of jobs that are created by
  * the JobRunner. It is notified by the JobQueue when a job is created
  * or removed, and handles the logic of deleting the pointer to the job.
  *
  */



#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

typedef unsigned long minuteT;

namespace TA_Base_App
{

JobInstanceManager* JobInstanceManager::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobInstanceManager::m_singletonLock;

JobInstanceManager::JobInstanceManager()
    :
    m_runningJobsMultiMap(),  // Initialise the data structures
    m_jobPointersMap()        // to be empty.
{
    JobRunner::getInstance().addJobExecutionListener(this);
}

JobInstanceManager::~JobInstanceManager()
{
}



JobInstanceManager& JobInstanceManager::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobInstanceManager();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");

    }
    
    return *m_instance;
}


void JobInstanceManager::startingJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& sessionID)
{
    FUNCTION_ENTRY("startingJob");
    addJobInstance(jobID, minuteStarted);
    FUNCTION_EXIT;
}

void JobInstanceManager::addJobInstance(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted)
{
    FUNCTION_ENTRY("addJobInstance");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job instance added with ID '%s' and minute started '%d'",
                jobID, minuteStarted);

    
    //
    // Lock the object.
    //
    ThreadGuard lock(*this);


    //
    // Check that the job exists
    //
    JobPointersMap::iterator it = m_jobPointersMap.find(std::string(jobID));

    if( it == m_jobPointersMap.end() )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "Job with ID '%s' and minute started '%d' not "
                    "found in job pointers multimap. Ignoring it.",
                    jobID, minuteStarted);
    }
    else
    {
        //
        // Add the job instance to the running jobs map.
        //
        RunningJobsMultiMap::value_type entry(jobID, minuteStarted);

        m_runningJobsMultiMap.insert(entry);
    }

    FUNCTION_EXIT;
}


void JobInstanceManager::jobCompleted(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& errorMessage,
    const std::string& sessionID)
{
    FUNCTION_ENTRY("removeJobInstance");

    removeJobInstance(jobID, minuteStarted);

    FUNCTION_EXIT;
}


void JobInstanceManager::removeJobInstance(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted)
{
    FUNCTION_ENTRY("removeJobInstance");
    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

    removeJobInstanceUnlocked(jobID, minuteStarted);

    FUNCTION_EXIT;
}

//TD16352++
bool JobInstanceManager::updateEditLock(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID, 
										std::string strSessionId, 
										bool bLock)
{
	FUNCTION_ENTRY("updateEditLock");
    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

	JobPointersMap::iterator jobIt = m_jobPointersMap.find(jobID);

	if( jobIt == m_jobPointersMap.end() )
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "[TD16352]Call to lock for edit but no job exists with jobid %s",
                    jobID);
		FUNCTION_EXIT;
		return false;
    }

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "[TD16352] jobid %s locked by %s", jobID,strSessionId.c_str());

	(*jobIt).second.isLockedForEdit = bLock;
	if(bLock == false)
	{
		(*jobIt).second.strSessionId = "";
	}
	else
	{
		(*jobIt).second.strSessionId = strSessionId;
	}
	
	FUNCTION_EXIT;
	return true;
}

bool JobInstanceManager::isJobLocked(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
									 std::string strSessionId)
{
	FUNCTION_ENTRY("isJobLocked");

	bool bReturn=false;
    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

	JobPointersMap::iterator jobIt = m_jobPointersMap.find(jobID);

	if( jobIt == m_jobPointersMap.end() )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "[TD16352]Job id %s not found in m_jobPointersMap",
                    jobID);
        FUNCTION_EXIT;
        return bReturn;
    }

	if(((*jobIt).second.strSessionId != strSessionId) 
		&& ((*jobIt).second.isLockedForEdit == true) &&
		((*jobIt).second.hasBeenRemoved == false))
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "[TD16352] jobid %s locked by %s",
                    jobID,strSessionId.c_str());
		bReturn = true;
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "[TD16352] jobid %s not locked",
                    jobID);

	FUNCTION_EXIT;
	return bReturn;
}
//++TD16352

//CL-21225
bool JobInstanceManager::isJobRunning(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID)
{
    FUNCTION_ENTRY("isJobRunning");

    ThreadGuard lock(*this);

    std::string strjobID(jobID);

    for(RunningJobsMultiMap::iterator it = m_runningJobsMultiMap.begin(); it != m_runningJobsMultiMap.end(); it++)
    {
        if (0 == it->first.compare(strjobID.c_str()))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "JobID[%s] is Running", strjobID.c_str());
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}

void JobInstanceManager::removeJobInstanceUnlocked(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted)
{
    FUNCTION_ENTRY("removeJobInstanceUnlocked");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job instance removed with ID '%s' and minute started '%d'",
                jobID, minuteStarted);

    //
    // Remove the job instance from the running jobs map.
    //
    RunningJobsMultiMap::iterator it =
        findRunningJobInstance(jobID, minuteStarted);

    if( it == m_runningJobsMultiMap.end() )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "Removed job with ID '%s' and minute started '%d' not "
                    "found in running jobs multimap",
                    jobID, minuteStarted);

        FUNCTION_EXIT;
        return;
    }

    m_runningJobsMultiMap.erase(it);


    //
    // If there are no other instances of the job running,
    // and if the job has been marked as removed,
    // delete it.
    //
    if( m_runningJobsMultiMap.find(jobID) == m_runningJobsMultiMap.end() )
    {
        JobPointersMap::iterator jobIt = m_jobPointersMap.find(jobID);

        if( jobIt == m_jobPointersMap.end() )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                        "Removed job with ID '%s' and minute started '%d' "
                        "but no job with that ID exists in job pointers "
                        "map.",
                        jobID, minuteStarted);

            FUNCTION_EXIT;
            return;
        }


        if( (*jobIt).second.hasBeenRemoved )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Deleting pointer to job with ID '%s'", jobID);

            IJob* job = (*jobIt).second.job;

            TA_ASSERT(job != NULL, "Job should never be NULL");

            delete job;
            m_jobPointersMap.erase(jobIt);
        }
    }

    FUNCTION_EXIT;
}


void JobInstanceManager::removeAllJobInstances()
{
    FUNCTION_ENTRY("removeAllJobInstances");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Removing all job instances");

    
    //
    // Lock the object.
    //
    ThreadGuard lock(*this);


    //
    // Get all job instances from the running jobs map, and remove them.
    //
    RunningJobsMultiMap runningJobsCopy(m_runningJobsMultiMap);

    for(RunningJobsMultiMap::iterator it = runningJobsCopy.begin();
        it != runningJobsCopy.end();
        it ++)
    {
			TA_Base_Bus::SchedulingAgentCorbaDef::JobID jobID = CORBA::string_dup((*it).first.c_str());
        minuteT minuteStarted                = (*it).second;

        removeJobInstanceUnlocked(jobID, minuteStarted);
    }    

    FUNCTION_EXIT;
}



//
// NOTE: THE OBJECT MUST BE LOCKED BEFORE CALLING THIS METHOD
//
JobInstanceManager::RunningJobsMultiMap::iterator
    JobInstanceManager::findRunningJobInstance(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID, unsigned long minuteStarted)
{
    //
    // Search through the jobs in the map until one is found whose ID
    // and minute started match the parameter.
    //
    for(RunningJobsMultiMap::iterator it = m_runningJobsMultiMap.begin();
        it != m_runningJobsMultiMap.end();
        it ++)
    {
		TA_Base_Bus::SchedulingAgentCorbaDef::JobID jobIDInMap = CORBA::string_dup((*it).first.c_str());
        minuteT minuteStartedInMap                = (*it).second;

        if( strcmp( jobIDInMap, jobID ) == 0	&&
            minuteStartedInMap == minuteStarted    )
        {
            return it;
        }
    }

    return m_runningJobsMultiMap.end();
}



void JobInstanceManager::jobCreated(IJob* job)
{
    FUNCTION_ENTRY("jobCreated");

    TA_ASSERT(job != NULL, "Job must not be NULL");


    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job created with ID '%s'", job->getID());


    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

    
    JobInfo jobInfo;

    jobInfo.job            = job;
    jobInfo.hasBeenRemoved = false;
	//TD16352++
	jobInfo.isLockedForEdit = false;
	//++TD16352

    // m_jobPointersMap[ job->getID() ] = jobInfo;
	m_jobPointersMap.insert(std::pair<std::string, JobInfo>( std::string(job->getID()), jobInfo));

    FUNCTION_EXIT;
}


void JobInstanceManager::jobRemoved(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID)
{
    FUNCTION_ENTRY("jobRemoved");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job removed with ID '%s'", jobID);

    //
    // Lock the object.
    //
    ThreadGuard lock(*this);


    JobPointersMap::iterator it = m_jobPointersMap.find(std::string(jobID));

    //
    // Check that the job exists in the job pointers map.
    //
    if( it == m_jobPointersMap.end() )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "Call to jobRemoved for job with ID '%s' but job not "
                    "found in job pointers map. This may occur if the job "
                    "has already completed running",
                    jobID);
    }
    else
    {
        //
        // If there are no instances of the job running, erase it.
        //
        if( m_runningJobsMultiMap.find(std::string(jobID)) == m_runningJobsMultiMap.end() )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Deleting pointer to job with ID %s", jobID);

            IJob* job = (*it).second.job;

            TA_ASSERT(job != NULL, "Job should not be NULL");

            delete job;
            m_jobPointersMap.erase(it);
        }
        //
        // If there are still instances of the job running,
        // mark it as removed.
        //
        else
        {
            (*it).second.hasBeenRemoved = true;
        }
    }

    FUNCTION_EXIT;
}



IJob* JobInstanceManager::getJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID) const
{
    FUNCTION_ENTRY("getJob");


	JobPointersMap::const_iterator it;

	it = m_jobPointersMap.find(std::string(jobID));

    if( it != m_jobPointersMap.end() )
    {
        return (*it).second.job;
    }

	FUNCTION_EXIT;
    return NULL;
}



TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef
    JobInstanceManager::getBatchUpdate()
{
    FUNCTION_ENTRY("getBatchUpdate");

    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

    
    TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef batchUpdate;

    batchUpdate.jobs         = getAllJobsUnlocked();
    batchUpdate.jobInstances = getJobInstancesUnlocked();

    FUNCTION_EXIT;
    return batchUpdate;
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence JobInstanceManager::getAllJobs()
{
    FUNCTION_ENTRY("getAllJobs");

    //
    // Lock the object.
    //
    ThreadGuard lock(*this);
    
    return getAllJobsUnlocked();

    FUNCTION_EXIT;
}


//
// NOTE: THE OBJECT MUST BE LOCKED BEFORE CALLING THIS METHOD
//
TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence
    JobInstanceManager::getAllJobsUnlocked() const
{
    FUNCTION_ENTRY("getAllJobsUnlocked");

    //
    // Go through the map and convert all the IJob pointers into
    // ScheduledJobCorbaDefs, and put them into a sequence.
    //
    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Got %d jobs in total", m_jobPointersMap.size());

    TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence jobSequence;

    unsigned int i = 0;
    for(JobPointersMap::const_iterator it = m_jobPointersMap.begin();
        it != m_jobPointersMap.end();
        it ++)
    {
        IJob* job = (*it).second.job;
        
        TA_ASSERT(job != NULL, "Job should never be NULL");
#if defined(HIDE_ALARM_GENERATOR_JOB)
        // if we're supposed to be hiding the alarm generator jobs, then don't add it to the
        // list it it is one
        if (job->getJobType() == AlarmGeneratorJob::JOB_TYPE)
        {
            continue;
        }
#endif
        jobSequence.length(i+1);
        jobSequence[i] = job->toCorbaObject();

        i ++;
    }


    FUNCTION_EXIT;
    return jobSequence;
}



TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence
    JobInstanceManager::getJobInstances()
{
    FUNCTION_ENTRY("getJobInstances");

    //
    // Lock the object.
    //
    ThreadGuard lock(*this);

    FUNCTION_EXIT;
    return getJobInstancesUnlocked();
}


//
// NOTE: THE OBJECT MUST BE LOCKED BEFORE CALLING THIS METHOD
//
TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence
    JobInstanceManager::getJobInstancesUnlocked() const
{
    FUNCTION_ENTRY("getJobInstancesUnlocked");

    //
    // Create the sequence and copy in the job ID and minute started
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence instances;
    instances.length( m_runningJobsMultiMap.size() );


    unsigned int i = 0;
    for(RunningJobsMultiMap::const_iterator it = m_runningJobsMultiMap.begin();
        it != m_runningJobsMultiMap.end();
        it ++)
    {
		instances[i].jobIdentifier = CORBA::string_dup((*it).first.c_str());
        instances[i].minuteStarted = (*it).second;

        i ++;
    }

    FUNCTION_EXIT;
    return instances;
}




};// TA_Base_App

