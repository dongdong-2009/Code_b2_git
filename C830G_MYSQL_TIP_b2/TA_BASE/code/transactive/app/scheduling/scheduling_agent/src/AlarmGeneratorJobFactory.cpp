/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/25 12:06:55 $
  * Last modified by:  $Author: builder $
  *
  * Implements the IJobFactory interface to provide a factory that creates
  * alarm generator jobs.
  *
  */



#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"


#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaNameList;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_App
{


void AlarmGeneratorJobFactory::initialise(
    int argc,
    char* argv[],
    TA_Base_Core::ISchedulingAgentEntityData& entityData)
{
    //
    // This job type has no initialisation
    //
}



IJob* AlarmGeneratorJobFactory::createJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugDebug,
        "Create Job request for Alarm Generator");

        
	// Make sure the object is of the correct type
	// TD20192++ Yanrong
    //if (job.type != AlarmGeneratorJob::JOB_TYPE.c_str())
	if(AlarmGeneratorJob::JOB_TYPE.compare(job.type.in()) != 0)
	// ++TD20192
    {
        return NULL;
    }

    // TODO: verify that the expected parameters are present.

    //
    // Create a new AlarmGeneratorJob. This will also create an entry in
    // the database.
    //
    
    AlarmGeneratorJob* iJob =
        new AlarmGeneratorJob(job);
    TA_ASSERT(iJob != NULL, "iJob should never be NULL");




    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                "Alarm Generator job created with "
                "id %s, "
                "locationID %d, "
                "profileID %d, "
                "isInhibited %d, "
                "job description '%s', "
                "schedule '%s', "
                "severity '%d'.",
                "alarm description '%s'",
                iJob->getID(),
                iJob->getLocationID(),
                iJob->getProfileID(),
                iJob->isInhibited(),
                iJob->getDescription().c_str(),
                iJob->getSchedule().toString().c_str(),
                iJob->getAlarmSeverityKey(),
                iJob->getAlarmDescription().c_str());

    return iJob;
}

IJob::IJobPtrVector AlarmGeneratorJobFactory::createJobsFromStore()
{
    //
    // Get the database objects for all jobs that are alarm generators
    //
    TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector jobDataObjects =
        TA_Base_Core::JobDataAccessFactory::getInstance().getAllJobsOfType(
            AlarmGeneratorJob::JOB_TYPE);

    //
    // Create a vector that will hold the IJob pointers
    //
    IJob::IJobPtrVector jobs;
    jobs.reserve(jobDataObjects.size());

    for(TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector::iterator it =
            jobDataObjects.begin();
        it != jobDataObjects.end();
        it ++)
    {
        //
        // Get the ID of the Alarm Generator Job in the database and create
        // an AlarmGeneratorJob object from it.
        //
			TA_Base_Bus::SchedulingAgentCorbaDef::JobID alarmGeneratorJobID = CORBA::string_dup((*it)->getID().c_str());
        jobs.push_back( new AlarmGeneratorJob(alarmGeneratorJobID) );

        delete *it;
    }

    return jobs;
}


IJob* AlarmGeneratorJobFactory::getJobFromStore(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& id)
{
    //
    // Get the database object for the job
    //
    TA_Base_Core::IConfigJobData* jobData = NULL;
    try
    {
        jobData = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(id);
    }
    //
    // If the job does not exist in the database, let the other JobFactories
    // try to retrieve it from whatever store they use.
    //
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException&)
    {
    }

    IJob* job = NULL;

    //
    // If the job exists, and it's of the correct type
    //
    if(jobData != NULL && jobData->getJobType() == AlarmGeneratorJob::JOB_TYPE)
    {
        //
        // Create an AlarmGeneratorJob object using the ID of the job.
        //
        try
        {
            job = new AlarmGeneratorJob( CORBA::string_dup(jobData->getID().c_str()) );
            TA_ASSERT(job != NULL, "job should never be NULL");

        }
		catch(const TA_Base_Core::TransactiveException& ex)// Wuzhongyi CL-21357
		{
			delete jobData;
			jobData = NULL;
			TA_THROW(TA_Base_Core::TransactiveException(ex.what()));
		}
        catch(...)
        {
            delete jobData;
			LOG_GENERIC(
				SourceInfo, DebugUtil::DebugError,
				"Unknown Exception"); // Wuzhongyi CL-21357
            throw;
        }
    }

    delete jobData;

    return job;
}



AlarmGeneratorJobFactory::~AlarmGeneratorJobFactory()
{
}


AlarmGeneratorJobFactory::AlarmGeneratorJobFactory()
{
}


};// TA_Base_App

