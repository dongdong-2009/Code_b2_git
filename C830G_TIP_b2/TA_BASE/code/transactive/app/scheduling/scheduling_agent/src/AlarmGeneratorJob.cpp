/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJob.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class implements the IJob interface by extending the Job class
  * to create a Job that can be used to generate alarms
  *
  */



#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/corba/src/CorbaUtil.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceStateRetrievalException.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"

using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;


namespace TA_Base_App
{


const std::string AlarmGeneratorJob::JOB_TYPE = "AlarmGeneratorJob";
const std::string AlarmGeneratorJob::ALARM_SEVERITY_KEY_PARAMETER_NAME = "AlarmSeverityKey";
const std::string AlarmGeneratorJob::ALARM_DESCRIPTION_PARAMETER_NAME  = "AlarmDescription";


IJobInstance* AlarmGeneratorJob::createJobInstance(
    unsigned long minuteStarted)
{
    FUNCTION_ENTRY("createJobInstance");

    return new AlarmGeneratorJobInstance(this, minuteStarted);
}

IJobInstance::JobInstanceState AlarmGeneratorJob::getJobInstanceState(
    unsigned long minuteStarted,
    std::string& errorMessage)
{
    FUNCTION_ENTRY("getJobInstanceState");

    //
    // This method is called after a failover to determine the state of
    // a job that should have been run.
    // It is assumed that an alarm takes such an insignificant time to be
    // raised that it will always have been raised when the job was started.
    //
    return IJobInstance::Completed;
}



TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef AlarmGeneratorJob::toCorbaObject()
{
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef job = AbstractJob::toCorbaObject();

    return job;
}


unsigned long AlarmGeneratorJob::getAlarmSeverityKey()
{
    std::string alarmSeverityKeyParameter =
        getParameter(ALARM_SEVERITY_KEY_PARAMETER_NAME);
    
    return getStringAsUnsignedLong(alarmSeverityKeyParameter);
}

std::string AlarmGeneratorJob::getAlarmDescription()
{
    return getParameter(ALARM_DESCRIPTION_PARAMETER_NAME);
}


void AlarmGeneratorJob::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    AbstractJob::updateJob(job);
}



AlarmGeneratorJob::~AlarmGeneratorJob()
{
}



AlarmGeneratorJob::AlarmGeneratorJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
    : AbstractJob(job)
{
    FUNCTION_ENTRY("AlarmGeneratorJob");

    
    FUNCTION_EXIT;
}


AlarmGeneratorJob::AlarmGeneratorJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID)
    :
    AbstractJob(jobID)
{
}




}; // TA_Base_App

