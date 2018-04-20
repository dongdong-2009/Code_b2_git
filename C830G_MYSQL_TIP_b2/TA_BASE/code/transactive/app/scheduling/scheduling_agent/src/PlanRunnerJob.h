/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanRunnerJob.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class implements the IJob interface by extending the Job class
  * to create a Job that can be used to run plans.
  *
  */



#ifndef PLAN_RUNNER_JOB_H
#define PLAN_RUNNER_JOB_H

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


#include "app/scheduling/scheduling_agent/src/AbstractJob.h"


namespace TA_Base_App
{


class PlanRunnerJob : public AbstractJob
{


public:


    typedef TA_Base_Core::NamedObject
    <
        TA_Base_Bus::IPlanAgentCorbaDef,
	    TA_Base_Bus::IPlanAgentCorbaDef_ptr,
	    TA_Base_Bus::IPlanAgentCorbaDef_var
    >
    IPlanAgentNamedObjRef;



    /**
     *  Creates a new IJobInstance.
     *
     *  Implementers of the IJob interface should create an IJobInstance
     *  object using new, and return a pointer to it.
     *
     *  @param minuteStarted  the minute at which the instance is started,
     *                        relative to the epoch.
     *                        This may be used as a unique identifier for the
     *                        job instance.
     */
    virtual IJobInstance* createJobInstance(
        unsigned long minuteStarted);


    /**
     *  Gets the state of an instance of this IJob.
     *  Note that this method must get the state from an external source
     *  at the time this method is called, rather than periodically updating
     *  an internally-stored state.
     *  Otherwise failover will not work properly.
     *
     *  @param minuteStarted  [in]  the minute at which the instance started.
     *  @param errorMessage   [out] the error message, if there was an error.
     *
     *  @exception JobInstanceNotFoundException
     *  Thrown if a job instance that started at the specified minute could
     *  not be found.
     *
     *  @exception JobInstanceStateRetrievalException
     *  Thrown if the job instance cannot be retrieved.
     *
     */
    virtual IJobInstance::JobInstanceState getJobInstanceState(
        unsigned long minuteStarted,
        std::string& errorMessage);


    /**
     *  Returns a JobCorbaDef containing the information in this object.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef toCorbaObject();


    /**
     *  Gets the full path of the plan this job runs.
     */
    virtual std::string getPlanPath();



    /** 
      * Updates the job in the database.
      *
      * @param job  the JobCorbaDef with the details of the job to be updated.
      *             Note that it must be possible to convert the job.data
      *             field to a plan runner job.
      *
      * @exception  UnrecognisedJobTypeException if the job.data cannot be
      *             converted to a plan runner job.
      */
    void updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);



    /**
     *  Destructor.
     */
	virtual ~PlanRunnerJob();
	

	/**
     *  Constructs a plan runner job from the given parameters.
     *
     *  @param job     a JobCorbaDef object containing the fields of this job
     *                 that are common to all jobs.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem writing
     *  values to the database.
	 */
    PlanRunnerJob(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);


    /** 
      * Constructs a plan runner job from the database.
      *
      *  @param jobID  the ID of the job in the database.
      *  @exception DatabaseException
      *
      *  A DatabaseException will be thrown if there is a problem retrieving
      *  values from the database.
      *
      *  @exception DataException
      *  A DataException will be thrown if the ID doesn't exist in the
      *  database or if any database fields are badly formatted.
      */
    PlanRunnerJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID);


    /**
      * Used in the database to indicate what type of job this is
      * (ie Plan Runner Job).
      */
    static const std::string JOB_TYPE;

    static const std::string PLAN_PATH_PARAMETER_NAME;


private:

    PlanRunnerJob();
    PlanRunnerJob(const PlanRunnerJob&);
    const PlanRunnerJob& operator=(const PlanRunnerJob&);


};



}; // TA_Base_App

#endif // PLAN_RUNNER_JOB_H
