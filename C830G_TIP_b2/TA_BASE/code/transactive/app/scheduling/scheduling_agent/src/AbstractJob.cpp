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
  * This abstract class represents an executable job.
  *
  */


#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/IJobData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "omniORB4/CORBA.h"

#include "app/scheduling/scheduling_agent/src/AbstractJob.h"


#include <sstream>


using TA_Base_Core::DataException;


namespace TA_Base_App
{




TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef AbstractJob::toCorbaObject()
{
    FUNCTION_ENTRY("toCorbaObject");
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef corbaJob;
    
    corbaJob.identifier          = CORBA::string_dup(getID());
    corbaJob.profileIdentifier   = getProfileID();
    corbaJob.locationIdentifier  = getLocationID();
    corbaJob.isInhibited         = isInhibited();
    corbaJob.description         = CORBA::string_dup( getDescription().c_str() );
    corbaJob.type                = CORBA::string_dup( getJobType().c_str() );
    corbaJob.schedule            = getSchedule().toCorbaObject();
    corbaJob.runTime             = Schedule::getNextScheduledTime( corbaJob.schedule );

    // copy the job data
    TA_Base_Core::IJobData::ParameterMap data = getParameters();

    corbaJob.data.length(data.size());

    unsigned long i = 0;
    for (TA_Base_Core::IJobData::ParameterMap::const_iterator p = data.begin();
            p != data.end();
            ++p)
    {
        TA_Base_Bus::SchedulingAgentCorbaDef::NameValuePair pair;
        pair.name = CORBA::string_dup(p->first.c_str());
        pair.value = CORBA::string_dup(p->second.c_str());
        corbaJob.data[i] = pair;
        ++i;
    }

    return corbaJob;
}


Schedule& AbstractJob::getSchedule()
{
    return *m_schedule;
}



std::string AbstractJob::getJobType()
{
    return m_jobData->getJobType();
}

std::string AbstractJob::getDescription()
{
    return m_jobData->getDescription();
}

bool AbstractJob::isInhibited()
{
    return m_jobData->getInhibitionStatus();
}

TA_Base_Bus::SchedulingAgentCorbaDef::JobID AbstractJob::getID()
{
    return CORBA::string_dup(m_jobData->getID().c_str()) ;
}

TA_Base_Bus::SchedulingAgentCorbaDef::LocationID AbstractJob::getLocationID()
{
    return m_jobData->getLocationID();
}

TA_Base_Bus::SchedulingAgentCorbaDef::ProfileID AbstractJob::getProfileID()
{
    return m_jobData->getProfileID();
}


std::string AbstractJob::getParameter(const std::string& parameterName)
{
    return m_jobData->getParameter(parameterName);
}

TA_Base_Core::IJobData::ParameterMap AbstractJob::getParameters()
{
    return m_jobData->getParameters();
}

void AbstractJob::setParameter(const std::string& name, const std::string& value)
{
    m_jobData->setParameter(name, value);
    m_jobData->applyChanges();
}

void AbstractJob::deleteParameter(const std::string& name)
{
    m_jobData->deleteParameter(name);
    m_jobData->applyChanges();
}


void AbstractJob::reloadDataFromDB(bool forceReload/* = false*/)
{
    m_jobData->reloadDataFromDB( forceReload );
}

void AbstractJob::invalidate()
{
    m_jobData->invalidate();
}

void AbstractJob::saveJob()
{
    // save the job first as the schedule requires it in the DB
    m_jobData->saveJob();
    m_jobData->getSchedule()->saveSchedule();
}

void AbstractJob::deleteJob()
{
    m_jobData->deleteJob();
}

void AbstractJob::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("updateJob");

    updateJobDetails(job);

    m_jobData->applyChanges();
}

void AbstractJob::updateJobDetails(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("updateJobDetails");
    m_jobData->setDescription( job.description.in() );
    m_jobData->setJobType( job.type.in() );
    m_jobData->setLocationID( job.locationIdentifier );
    m_jobData->setProfileID( job.profileIdentifier );
    m_jobData->setInhibitionStatus ( job.isInhibited );

    if ( job.data.length() != 0 )
    {
        unsigned int i = job.data.length();
        while (i--)
        {
            m_jobData->setParameter( job.data[i].name.in(), job.data[i].value.in() );
        }
    }

    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();
    TA_ASSERT(scheduleData != NULL, "Schedule should never be NULL");

    if ( job.schedule.daysOfWeek.length() != 0 )
    {
        std::list<unsigned short> days;
        unsigned int i = job.schedule.daysOfWeek.length();
        while (i--)
        {
            days.push_back( job.schedule.daysOfWeek[i] );
        }
        scheduleData->setDaysOfWeek( days );
    }
    scheduleData->setDayOfMonth( job.schedule.dayOfMonth );
    scheduleData->setEndDay( job.schedule.endDay );
    scheduleData->setEndMonth( job.schedule.endMonth );
    scheduleData->setEndYear( job.schedule.endYear );
    scheduleData->setHour( job.schedule.hour );
    scheduleData->setMinute( job.schedule.minute );
    scheduleData->setMonthOfYear( job.schedule.month );
    scheduleData->setScheduleType( job.schedule.type );
    scheduleData->setYear( job.schedule.year );

}



unsigned long AbstractJob::getStringAsUnsignedLong(
    const std::string& theString)
{
    std::stringstream stringStream;
    stringStream << theString;

    unsigned long number = 0;
    if(stringStream >> number)
    {
        return number;
    }

    std::string message = "Could not convert string to number: "
                        + theString;
    TA_THROW(DataException(message.c_str(), DataException::WRONG_TYPE, ""));
}



AbstractJob::AbstractJob(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("AbstractJob(job)");
    //
    // Create a new job, filling the details
    //
    m_jobData = TA_Base_Core::JobDataAccessFactory::getInstance().createJob();

    TA_ASSERT(m_jobData != NULL, "Job data should never be NULL");
    updateJobDetails(job);

    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();
    TA_ASSERT(scheduleData != NULL, "Schedule should never be NULL");

    m_schedule = new Schedule( *scheduleData );
    TA_ASSERT(m_schedule != NULL, "m_schedule should never be NULL");
}



AbstractJob::AbstractJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID)
{
    FUNCTION_ENTRY("AbstractJob(jobID)");
    m_jobData = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID);
    TA_ASSERT(m_jobData != NULL, "Job data should never be NULL");
    
    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();
    TA_ASSERT(scheduleData != NULL, "Schedule should never be NULL");
    
    m_schedule = new Schedule(*scheduleData);
    TA_ASSERT(m_schedule != NULL, "m_schedule should never be NULL");
}


AbstractJob::~AbstractJob()
{
    FUNCTION_ENTRY("~AbstractJob");
    // The schedule should be deleted first as it contains a reference
    // to m_jobData->getSchedule(), which should be cleaned up first
    delete m_schedule;
    m_schedule = NULL;
    
    delete m_jobData;
    m_jobData = NULL;
}


}; // TA_Base_App

