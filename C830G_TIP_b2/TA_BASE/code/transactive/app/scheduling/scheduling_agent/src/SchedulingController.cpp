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
  *
  * This class encapsulates the logic for the Scheduling Agent in control
  * mode.
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4700) // local variable used without having been initialised
#endif // _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAccessManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/JobQueue.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/JobFactory.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"

#include "app/scheduling/scheduling_agent/src/SchedulingController.h"
#include "core/exceptions/src/CorbaException.h" //like++. for "TA_THROW_CORBA"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseErrorException;

typedef TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef JobCorbaDef;
typedef TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef ScheduleCorbaDef;
typedef TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence JobSequence;
typedef TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException JobDoesNotExistException;

//TD16352++
typedef TA_Base_Bus::SchedulingAgentCorbaDef::EditLockException EditLockException;
//++TD16352

namespace TA_Base_App
{

SchedulingController* SchedulingController::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingController::m_singletonLock;


SchedulingController& SchedulingController::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new SchedulingController();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


SchedulingController::SchedulingController()
{
    activateServantWithName(RunParams::getInstance().get(RPARAM_ENTITYNAME));
}


SchedulingController::~SchedulingController()
{
    deactivateAndDeleteServant();
}



void SchedulingController::enable()
{
    FUNCTION_ENTRY("enable");

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo, "Enabling Scheduling Controller");

    JobRunner::getInstance().addJobExecutionListener(this);

    FUNCTION_EXIT;
}


void SchedulingController::disable()
{
    FUNCTION_ENTRY("disable");

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo, "Disabling Scheduling Controller");


    JobRunner::getInstance().removeJobExecutionListener(this);

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////
//                        IJobExecutionListener                        //
/////////////////////////////////////////////////////////////////////////


void SchedulingController::startingJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& sessionID)
{
    FUNCTION_ENTRY("startingJob");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job starting with ID %s at %d minutes after the epoch.",
                jobID, minuteStarted);

    //
    // Note that this object listens for the job starting and job completed
    // events instead of the SchedulingMessageSender as only the controlling
    // agent should send the corresponding messages (not the monitoring agent).
    //
    SchedulingMessageSender::getInstance().jobStarting(
        jobID, minuteStarted, sessionID);
}


void SchedulingController::jobCompleted(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& errorMessage,
    const std::string& sessionID)
{
    FUNCTION_ENTRY("jobCompleted");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Job with ID %s completed. Job was started at %d minutes "
                "after the epoch.",
                jobID, minuteStarted);
    
    //
    // Note that this object listens for the job starting and job completed
    // events instead of the SchedulingMessageSender as only the controlling
    // agent should send the corresponding messages (not the monitoring agent).
    //
    SchedulingMessageSender::getInstance().jobCompleted(
        jobID, minuteStarted, sessionID, errorMessage);
}




/////////////////////////////////////////////////////////////////////////
//                       SchedulingAgentCorbaDef                       //
/////////////////////////////////////////////////////////////////////////

TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* SchedulingController::getSupportedJobTypes(
    const char* /*sessionIdentifier*/)
{
    FUNCTION_ENTRY("getSupportedJobTypes");
    
    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();

    // for now we only support 1 job type
    TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* ret = 
        new TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq;
    TA_ASSERT(ret != NULL, "TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* ret should never be NULL");
    
    // TD 8692: Inserted Alarm Raising back into Scheduler so alarms can be raised at 
    // predefined intervals for archiving.
    ret->length(1);
    (*ret)[0] = CORBA::string_dup("Plan");
#if !defined(HIDE_ALARM_GENERATOR_JOB)
    ret->length(2);
    (*ret)[1] = CORBA::string_dup("AlarmGeneratorJob");
#endif        
    return ret;
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobID SchedulingController::scheduleJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& scheduledJob,
    const char* sessionIdentifier)
{
    FUNCTION_ENTRY("scheduleJob");

    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();

    //
    // Check if the sessionID is valid for this action.
    //
    
    TA_Base_Bus::AccessControlledAction actionID = TA_Base_Bus::aca_SCHEDULE_MODIFY;
    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(
        sessionIdentifier, actionID);


    TA_Base_Bus::SchedulingAgentCorbaDef::JobID jobID =  CORBA::string_dup("");

    try
    {
        //
        // Schedule the job.
        //
        jobID = CORBA::string_dup(JobQueue::getInstance().scheduleJob(scheduledJob));
    }
    //
    // Convert DataExceptions and DatabaseExceptions to the corba equivalent
    //
    catch(const DataException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    catch(const DatabaseException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    

    //
    // Inform the SchedulingMessageSender
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef newJob(scheduledJob);
    newJob.identifier = CORBA::string_dup(jobID);

    SchedulingMessageSender::getInstance().jobAdded(
        newJob, std::string(sessionIdentifier));


    return jobID;
}

void SchedulingController::removeJob(
    const char* jobID,
    const char* sessionID)
{
    FUNCTION_ENTRY("removeJob");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Removing job with id '%s'.", jobID);

	//TD16352++
	bool isJobLocked = false;

	isJobLocked = JobInstanceManager::getInstance().isJobLocked(
		CORBA::string_dup(jobID), 
		sessionID);

	if(isJobLocked == true)
	{
		TA_THROW_CORBA_EXCEPTION(EditLockException());
	}
	//TD16352++

    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();

    //
    // Check if the sessionID is valid for this action.
    //
    TA_Base_Bus::AccessControlledAction actionID = TA_Base_Bus::aca_SCHEDULE_MODIFY;
    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(
        sessionID, actionID);

	//[[ CL-21225 Scheduling Agent crash the operator delete a plan schedule job which is executing
	if (JobInstanceManager::getInstance().isJobRunning(CORBA::string_dup(jobID)))
	{
		std::stringstream ss;
		ss << "can not remove this job[" ;
		ss << jobID;
		ss << "], because it is running now";
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, ss.str().c_str() );
		TA_THROW_CORBA_EXCEPTION(EditLockException(ss.str().c_str()));
	}
	//]] CL-21225

    try
    {
        //
        // Remove the job with that ID from the JobQueue.
        //
        JobQueue::getInstance().removeJob(CORBA::string_dup(jobID), sessionID);
    }
    //
    // Convert DataExceptions and DatabaseExceptions to the corba equivalent
    //
    catch(const DataException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    catch(const DatabaseException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }

}


void SchedulingController::updateJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
    const char* sessionID,
	 CORBA::Boolean changeEnabled,
	 CORBA::Boolean  checkIfInPast/*=true*/)
{
    FUNCTION_ENTRY("updateJob");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
		"Updating job with id '%s'.", std::string(job.identifier).c_str());


	//TD16352++
	bool isJobLocked = false;

	isJobLocked = JobInstanceManager::getInstance().isJobLocked(
		CORBA::string_dup(job.identifier), 
		sessionID);

	if(isJobLocked == true)
	{
		TA_THROW_CORBA_EXCEPTION(EditLockException());
	}
	//TD16352++

    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();

    //
    // Check if the sessionID is valid for this action.
    //
    TA_Base_Bus::AccessControlledAction actionID = TA_Base_Bus::aca_SCHEDULE_MODIFY;
    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(
        sessionID, actionID);



    try
    {
        //
        // Update the job in the job queue
        //
        JobQueue::getInstance().updateJob(job, checkIfInPast);
    }
    //
    // Convert DataExceptions and DatabaseExceptions to the corba equivalent
    //
    catch(const DataException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    catch(const DatabaseException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }


    //
    // Inform the SchedulingMessageSender.
    //
    SchedulingMessageSender::getInstance().jobUpdated(
        job, std::string(sessionID), changeEnabled);

}

//TD16352++
void SchedulingController::updateEditLock(
    const char* jobID,
    const char* sessionID,
	CORBA::Boolean  bLock)
{
	FUNCTION_ENTRY("getJob");

	if(JobInstanceManager::getInstance().updateEditLock(
		CORBA::string_dup(jobID), 
		sessionID, 
		bLock)==false)
	{
		TA_THROW_CORBA_EXCEPTION(JobDoesNotExistException());
	}

	FUNCTION_EXIT;
}

bool SchedulingController::isJobLockedForEdit(
		const char* jobID,
		const char* sessionID)
{
	return JobInstanceManager::getInstance().isJobLocked(
		CORBA::string_dup(jobID), 
		sessionID);
}
//++TD16352


JobCorbaDef* SchedulingController::getJob(
    const char* jobID,
    const char* sessionID)
{
    FUNCTION_ENTRY("getJob");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Get job called for job with id '%s'", jobID);

    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();

    IJob* job = NULL;


    try
    {
        //
        // Get the job
        //
        job = JobInstanceManager::getInstance().getJob(CORBA::string_dup(jobID));
    }
    //
    // Convert DataExceptions and DatabaseExceptions to the corba equivalent
    //
    catch(const DataException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    catch(const DatabaseException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }

    //
    // If the job is NULL, the job does not exist in the job queue.
    //
    if(job == NULL)
    {
        TA_THROW_CORBA_EXCEPTION(JobDoesNotExistException());
    }

    return new JobCorbaDef( job->toCorbaObject() );
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence* SchedulingController::getAllJobs(
    const char* sessionID)
{
    FUNCTION_ENTRY("getAllJobs");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Get scheduled jobs called");
    
    //
    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    //
    checkOperationMode();


    try
    {
        //
        // Get all the jobs that have been scheduled and return them.
        //
        return new JobSequence( JobInstanceManager::getInstance().getAllJobs() );
    }
    //
    // Convert DataExceptions and DatabaseExceptions to the corba equivalent
    //
    catch(const DataException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
    catch(const DatabaseException& ex)
    {
        TA_THROW_CORBA(DatabaseErrorException(ex.what()));
    }
}


void SchedulingController::checkOperationMode()
{
    if( RunParams::getInstance().get(RPARAM_OPERATIONMODE) == RPARAM_MONITOR )
    {
        TA_THROW_CORBA(TA_Base_Core::OperationModeException());
    }
}


}; // TA_Base_App
