/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implements the IJobInstance interface for AlarmGeneratorJobs.
  *
  */



#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/naming/src/NamedObject.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/SchedulingAlarms_MessageTypes.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"

#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::NameValuePair;
using TA_Base_Core::DescriptionParameters;

namespace TA_Base_App
{


void AlarmGeneratorJobInstance::execute()
{
    TA_ASSERT(getJob() != NULL, "Job must not be NULL");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Executing alarm generator job instance at minute %d "
                "since the epoch that is a child of job with ID %d.",
                getMinuteStarted(), getJob()->getID());

    //
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // [Description]
    //
    NameValuePair descriptionPair("Description", m_job->getAlarmDescription());

    DescriptionParameters params;
    params.push_back(&descriptionPair);


    //
    // Submit the alarm
    //
    SchedulingAlarmSender::getInstance().submitAlarmWithSeverity(
        TA_Base_Core::SchedulingAlarms::ScheduledAlarm,
        params,
        m_job->getAlarmSeverityKey());
}




AlarmGeneratorJobInstance::~AlarmGeneratorJobInstance()
{
}

	
AlarmGeneratorJobInstance::AlarmGeneratorJobInstance(
    AlarmGeneratorJob* job,
    const unsigned long minuteStarted)
    :
    AbstractJobInstance(job, minuteStarted),
    m_job(job)
{
}



}; // TA_Base_App

