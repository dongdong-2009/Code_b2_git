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
  * Implements the IJobFactory interface to provide a factory that creates
  * plan runner jobs.
  *
  */

#include "app/scheduling/scheduling_agent/src/PlanRunnerJob.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"


#include "app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::PlanAccessFactory;
using TA_Base_Core::IPlan;
using TA_Base_Core::CorbaNameList;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::GenericAgentException;
using TA_Base_Bus::PlanAgentLibrary;

namespace TA_Base_App
{


PlanAgentMap PlanRunnerJobFactory::m_planAgentMap;
PlanAgentLibrary* PlanRunnerJobFactory::m_defaultPlanAgent = NULL;

void PlanRunnerJobFactory::initialise(
    int argc,
    char* argv[],
    TA_Base_Core::ISchedulingAgentEntityData& entityData)
{
    unsigned long planAgentLocationKey  = entityData.getPlanAgentLocationKey();

    m_defaultPlanAgent = new PlanAgentLibrary(planAgentLocationKey);
    TA_ASSERT(m_defaultPlanAgent != NULL, "m_planAgent should never be NULL");
}



IJob* PlanRunnerJobFactory::createJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("createJob");

    //
	// only proceed if of the right type
    //
    std::string jobtype(job.type);

    if( jobtype != PlanRunnerJob::JOB_TYPE )
	{
        return NULL;
	}
    
    //
    // Create a new PlanRunnerJob. 
    //
    PlanRunnerJob* iJob = new PlanRunnerJob(job);
    TA_ASSERT(iJob != NULL, "iJob should never be NULL");

    //
    // and save to db if no problems
    //
    iJob->saveJob();

    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                "Plan Runner job created with "
                "id %s, "
                "locationID %d, "
                "profileID %d, "
                "isInhibited %d, "
                "description '%s', "
                "schedule '%s', "
                "plan %s.",
                iJob->getID(),
                iJob->getLocationID(),
                iJob->getProfileID(),
                iJob->isInhibited(),
                iJob->getDescription().c_str(),
                iJob->getSchedule().toString().c_str(),
                iJob->getPlanPath().c_str());

    return iJob;
}

IJob::IJobPtrVector PlanRunnerJobFactory::createJobsFromStore()
{
    FUNCTION_ENTRY("createJobsFromStore");
    //
    // Get the database objects for all jobs that are plans
    //
    TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector jobDataObjects =
        TA_Base_Core::JobDataAccessFactory::getInstance().getAllJobsOfType(
            PlanRunnerJob::JOB_TYPE);

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
        // Get the ID of the Plan Runner Job in the database and create
        // a PlanRunnerJob object from it.
        //
			TA_Base_Bus::SchedulingAgentCorbaDef::JobID planRunnerJobID = CORBA::string_dup((*it)->getID().c_str());
        jobs.push_back( new PlanRunnerJob(planRunnerJobID) );

        delete *it;
    }

    return jobs;
}


IJob* PlanRunnerJobFactory::getJobFromStore(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& id)
{
    FUNCTION_ENTRY("getJobFromStore");
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
    if(jobData != NULL && jobData->getJobType() == PlanRunnerJob::JOB_TYPE)
    {
        //
        // Create a PlanRunnerJob object using the ID of the job.
        //
        try
        {
            job = new PlanRunnerJob( CORBA::string_dup(jobData->getID().c_str()) );
            TA_ASSERT(job != NULL, "job should never be NULL");
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


/*PlanAgentLibrary* PlanRunnerJobFactory::getPlanAgent()
{
    return m_planAgent;
}*/

PlanAgentLibrary* PlanRunnerJobFactory::getPlanAgent(unsigned long locationKey)
{
    PlanAgentLibrary* pPlanAgent = NULL;
	// Location key will be 0 for non-location-specific profiles, eg. Super
	if(locationKey == 0)
	{
		pPlanAgent = m_defaultPlanAgent;
	}
	else
	{
		pPlanAgent = &(m_planAgentMap.getPlanAgent(locationKey));
	}

    if ( NULL == pPlanAgent )
    {
        std::stringstream stmError;
        stmError << "planAgent not exist for location " << locationKey;
        TA_THROW(TA_Base_Core::TransactiveException( stmError.str().c_str() ));
    }

    return pPlanAgent;
}

PlanRunnerJobFactory::~PlanRunnerJobFactory()
{
}


PlanRunnerJobFactory::PlanRunnerJobFactory()
{
}


};// TA_Base_App

