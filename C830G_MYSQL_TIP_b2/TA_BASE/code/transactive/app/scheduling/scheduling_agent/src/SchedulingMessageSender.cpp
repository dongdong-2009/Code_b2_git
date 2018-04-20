/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingMessageSender.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * This class is used to send messages on the Comms, StateUpdate and Audit
  * channels.
  *
  */



#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/SchedulingAudit_MessageTypes.h"
#include "core/message/types/SchedulingComms_MessageTypes.h"
#include "core/message/types/SchedulingStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/scheduling_agent/src/IConfigJobData.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ILocation.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/Schedule.h"

#include <iomanip>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::MessagePublicationManager;
using TA_Base_Core::DescriptionParameters;
using TA_Base_Core::NameValuePair;
using TA_Base_Core::RunParams;


namespace TA_Base_App
{

SchedulingMessageSender* SchedulingMessageSender::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingMessageSender::m_singletonLock;

    
SchedulingMessageSender::SchedulingMessageSender()
    :
    m_auditSender(NULL),
    m_commsSender(NULL),
    m_stateSender(NULL),
    m_entityKey(0),
	m_parentEntityKey(0),
    m_subsystemKey(0),
    m_locationKey(0)
{
    FUNCTION_ENTRY("SchedulingMessageSender");

    m_auditSender =
        MessagePublicationManager::getInstance().getAuditMessageSender(
            TA_Base_Core::SchedulingAudit::Context);

    m_commsSender =
        MessagePublicationManager::getInstance().getCommsMessageSender(
            TA_Base_Core::SchedulingComms::Context);

    m_stateSender =
        MessagePublicationManager::getInstance().getStateUpdateMessageSender(
            TA_Base_Core::SchedulingStateUpdate::Context);
}


SchedulingMessageSender::~SchedulingMessageSender()
{
    FUNCTION_ENTRY("~SchedulingMessageSender");

    delete m_auditSender;
    m_auditSender = NULL;

    delete m_commsSender;
    m_commsSender = NULL;

    delete m_stateSender;
    m_stateSender = NULL;
}


void SchedulingMessageSender::initialise(
    unsigned long entityKey,
	unsigned long parentEntityKey,
    unsigned long subsystemKey,
    unsigned long locationKey)
{
    FUNCTION_ENTRY("initialise");

    TA_ASSERT(entityKey != 0, "Entity key must not be 0");

    m_entityKey    = entityKey;
	m_parentEntityKey = parentEntityKey;
    m_subsystemKey = subsystemKey;
    m_locationKey  = locationKey;
}


SchedulingMessageSender& SchedulingMessageSender::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    //
    // If this method has been called before and initialise was not called,
    // assert.
    //
    TA_ASSERT(m_instance == NULL || m_instance->m_entityKey != 0,
              "SchedulingMessageSender::initialise() must be called the "
              "first time getInstance() is invoked");

    if(m_instance == NULL)
    {
        m_instance = new SchedulingMessageSender();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }

    return *m_instance;
}





void SchedulingMessageSender::jobAdded(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
    const std::string& sessionKey)
{
    FUNCTION_ENTRY("jobAdded");


    CORBA::Any jobAsAny;
    jobAsAny <<= job;

    
    std::string incidentKey   = "";
    std::string alarmID       = "";
    std::string parentEventID = "";

    std::string details = "Job: ";
    details            += getJobCorbaDefAsString(job);


    std::stringstream jobID;
    jobID << job.identifier;

    DescriptionParameters params;
    NameValuePair jobIDPair("JobId", std::string(job.identifier));
    NameValuePair jobDescriptionPair("JobDescription", std::string(job.description));
    
	// TD #12041
	// Add the profile, location, type and frequency summary info to the event message
    std::stringstream profileID;
    profileID << TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job.profileIdentifier)->getName();

	NameValuePair profilePair("ProfileName", profileID.str());

    std::stringstream locationID;
    locationID << TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job.locationIdentifier)->getName();

    NameValuePair locationPair("LocationName", locationID.str());
    NameValuePair typePair("TypeName", std::string(job.type));
    NameValuePair frequencySummaryPair("FrequencySummary", getFrequencySummary(job));

    
    params.push_back(&jobIDPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);
    params.push_back(&frequencySummaryPair);

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending Comms and Audit messages: Job Added");


    //
    // Send a Comms message
    //
    m_commsSender->sendCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobAdded,
        m_entityKey,
        jobAsAny,
        m_subsystemKey,
        m_locationKey);

    //
    // Send an Audit message
    //
    m_auditSender->sendAuditMessage(
        TA_Base_Core::SchedulingAudit::SchedulingAddJob,
        m_parentEntityKey,
        params,
        details,
        sessionKey,
        alarmID,
        incidentKey,
        parentEventID);
}


void SchedulingMessageSender::jobScheduled(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    const TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef& runTime )
{
    FUNCTION_ENTRY("jobScheduled");

    TA_Base_Bus::SchedulingAgentCorbaDef::JobTimeCorbaDef jobTime;
    jobTime.jobIdentifer = CORBA::string_dup(jobID);
    jobTime.runTime = runTime;

    CORBA::Any jobTimeAsAny;
    jobTimeAsAny <<= jobTime;
    

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending Comms message: Job Scheduled");

    //
    // Send a Comms message
    //
    m_commsSender->sendCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobScheduled,
        m_entityKey,
        jobTimeAsAny,
        m_subsystemKey,
        m_locationKey);

}

void SchedulingMessageSender::jobUpdated(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
    const std::string& sessionKey, 
	 bool  changeEnabled)
{
    FUNCTION_ENTRY("jobUpdated");

    CORBA::Any jobAsAny;
    jobAsAny <<= job;

    
    std::string incidentKey   = "";
    std::string alarmID       = "";
    std::string parentEventID = "";

    std::string details = "New job details: ";
    details            += getJobCorbaDefAsString(job);


    NameValuePair jobIDPair("JobId", std::string(job.identifier));
    NameValuePair jobDescriptionPair("JobDescription", std::string(job.description));
    
	// TD #12041
	// Add the profile, location, type and frequency summary info to the event message
    std::stringstream profileID;
    profileID << TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job.profileIdentifier)->getName();

	NameValuePair profilePair("ProfileName", profileID.str());

    std::stringstream locationID;
    locationID << TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job.locationIdentifier)->getName();

    NameValuePair locationPair("LocationName", locationID.str());
    NameValuePair typePair("TypeName", std::string(job.type));
    NameValuePair frequencySummaryPair("FrequencySummary", getFrequencySummary(job));

    
    DescriptionParameters params;
    params.push_back(&jobIDPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);
    params.push_back(&frequencySummaryPair);


    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending Comms and Audit messages: Job Updated");




    //
    // Send a Comms message
    //
    m_commsSender->sendCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobUpdated,
        m_entityKey,
        jobAsAny,
        m_subsystemKey,
        m_locationKey);

    //
    // Send an Audit message
    //
	 if (changeEnabled)
	 {
		 if (job.isInhibited)
		 {
			 m_auditSender->sendAuditMessage(
				 TA_Base_Core::SchedulingAudit::SchedulingDisableJob,
				 m_parentEntityKey,
				 params,
				 details,
				 sessionKey,
				 alarmID,
				 incidentKey,
				 parentEventID);
		 }
		 else
		 {
			 m_auditSender->sendAuditMessage(
				 TA_Base_Core::SchedulingAudit::SchedulingEnableJob,
				 m_parentEntityKey,
				 params,
				 details,
				 sessionKey,
				 alarmID,
				 incidentKey,
				 parentEventID);
		 }
	 }
	 else
	 {
		 m_auditSender->sendAuditMessage(
			 TA_Base_Core::SchedulingAudit::SchedulingModifyJob,
			 m_parentEntityKey,
			 params,
			 details,
			 sessionKey,
			 alarmID,
			 incidentKey,
			 parentEventID);
	 }
}


void SchedulingMessageSender::jobDeleted(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    const std::string& sessionKey)
{
    FUNCTION_ENTRY("jobDeleted");

    CORBA::Any jobIDAsAny;
    jobIDAsAny <<= jobID;


    std::string incidentKey   = "";
    std::string alarmID       = "";
    std::string parentEventID = "";
    std::string details       = "";


    std::stringstream jobIDString;
    jobIDString << jobID;


    NameValuePair jobIDPair("JobId", jobIDString.str());

    TA_Base_Core::IConfigJobData* job = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID);
    std::stringstream jobDescription;
    jobDescription << job->getDescription();
    NameValuePair jobDescriptionPair("JobDescription", jobDescription.str());

	// TD #12041
	// Add the profile, location and type info to the event message
    std::stringstream profileID;
    profileID << TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID())->getName();

	NameValuePair profilePair("ProfileName", profileID.str());

    std::stringstream locationID;
    locationID << TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID())->getName();

    NameValuePair locationPair("LocationName", locationID.str());
    NameValuePair typePair("TypeName", std::string(job->getJobType()));
    
    delete job;

    DescriptionParameters params;
    params.push_back(&jobIDPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending Comms and Audit messages: Job Deleted");

    //
    // Send a Comms message
    //
    m_commsSender->sendCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobDeleted,
        m_entityKey,
        jobIDAsAny,
        m_subsystemKey,
        m_locationKey);

    //
    // Send an Audit message
    //
    m_auditSender->sendAuditMessage(
        TA_Base_Core::SchedulingAudit::SchedulingDeleteJob,
        m_parentEntityKey,
        params,
        details,
        sessionKey,
        alarmID,
        incidentKey,
        parentEventID);
}


void SchedulingMessageSender::jobStarting(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& sessionKey)
{
    FUNCTION_ENTRY("jobStarting");

    //
    // Create a JobInstanceCorbaDef from the parameters
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef instance;

    instance.jobIdentifier = CORBA::string_dup(jobID);
    instance.minuteStarted = minuteStarted;


    //
    // Create a CORBA::Any to contain the job instance
    //
    CORBA::Any jobInstanceAsAny;
    jobInstanceAsAny <<= instance;

    
    std::string incidentKey   = "";
    std::string alarmID       = "";
    std::string parentEventID = "";
    std::string details       = "";


    std::stringstream jobIDString;
    jobIDString << jobID;

    std::stringstream minuteStartedString;
    minuteStartedString << minuteStarted;

    TA_Base_Core::IConfigJobData* job = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID);
    std::stringstream jobDescription;
    jobDescription << job->getDescription();

    NameValuePair jobDescriptionPair("JobDescription", jobDescription.str());
    NameValuePair jobIDPair("JobId", jobIDString.str());
    NameValuePair minuteStartedPair("InstanceID", minuteStartedString.str());

	// TD #12041
	// Add the profile, location and type info to the event message
    std::stringstream profileID;
    profileID << TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID())->getName();

	NameValuePair profilePair("ProfileName", profileID.str());

    std::stringstream locationID;
    locationID << TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID())->getName();

    NameValuePair locationPair("LocationName", locationID.str());
    NameValuePair typePair("TypeName", std::string(job->getJobType()));

    delete job;

    DescriptionParameters params;
    params.push_back(&jobIDPair);
    params.push_back(&minuteStartedPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending State update and Audit messages: Job Started");


    //
    // Send a State Update message.
    //
    m_stateSender->sendStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobStarting,
        m_entityKey,
        RunParams::getInstance().get(RPARAM_ENTITYNAME),
        jobInstanceAsAny);

    //
    // Send an Audit message
    //
    m_auditSender->sendAuditMessage(
        TA_Base_Core::SchedulingAudit::SchedulingJobStarting,
        m_parentEntityKey,
        params,
        details,
        sessionKey,
        alarmID,
        incidentKey,
        parentEventID);
}


void SchedulingMessageSender::jobCompleted(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
    unsigned long minuteStarted,
    const std::string& sessionKey,
    const std::string& errorMessage)
{
    FUNCTION_ENTRY("jobCompleted");

    //
    // Create a JobInstanceCorbaDef from the parameters
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef instance;

    instance.jobIdentifier = CORBA::string_dup(jobID);
    instance.minuteStarted = minuteStarted;


    //
    // Create a CORBA::Any to contain the job instance
    //
    CORBA::Any jobInstanceAsAny;
    jobInstanceAsAny <<= instance;

    
    std::string incidentKey   = "";
    std::string alarmID       = "";
    std::string parentEventID = "";
    
    std::string details       = "";
    if(errorMessage.length() > 0)
    {
        details += "Error message: ";
        details += errorMessage;
    }


    std::stringstream jobIDString;
    jobIDString << jobID;

    std::stringstream minuteStartedString;
    minuteStartedString << minuteStarted;

    TA_Base_Core::IConfigJobData* job = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID);
    std::stringstream jobDescription;
    jobDescription << job->getDescription();

    NameValuePair jobDescriptionPair("JobDescription", jobDescription.str());
    NameValuePair jobIDPair("JobId", jobIDString.str());
    NameValuePair minuteStartedPair("InstanceID", minuteStartedString.str());

	// TD #12041
	// Add the profile, location and type info to the event message
    std::stringstream profileID;
    profileID << TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID())->getName();

	NameValuePair profilePair("ProfileName", profileID.str());

    std::stringstream locationID;
    locationID << TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID())->getName();

    NameValuePair locationPair("LocationName", locationID.str());
    NameValuePair typePair("TypeName", std::string(job->getJobType()));

    delete job;

    DescriptionParameters params;
    params.push_back(&jobIDPair);
    params.push_back(&minuteStartedPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC(
        SourceInfo, DebugUtil::DebugInfo,
        "Sending State Updated and Audit messages: Job Completed");


    //
    // Send a State Update message.
    //
    m_stateSender->sendStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobCompleted,
        m_entityKey,
        RunParams::getInstance().get(RPARAM_ENTITYNAME),
        jobInstanceAsAny);

    // Send an Audit message
    //
    m_auditSender->sendAuditMessage(
        TA_Base_Core::SchedulingAudit::SchedulingJobCompleted,
        m_parentEntityKey,
        params,
        details,
        sessionKey,
        alarmID,
        incidentKey,
        parentEventID);
}


std::string SchedulingMessageSender::getFrequencySummary(
     const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
	std::ostringstream frequencySummary;

	switch(job.schedule.type)
	{
		case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
			// Occurs once on <date> at <time>.z
			frequencySummary << "Occurs once on ";
			frequencySummary << getFormattedDate(job.schedule.year, job.schedule.month+1, job.schedule.dayOfMonth);
			frequencySummary << " at ";
			frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
			break;
		case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
			// Occurs daily at <time>. [Ends on <end date>.]
			frequencySummary << "Occurs daily at ";
			frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
			if(job.schedule.endDay > 0)
			{
				frequencySummary << ". Ends on ";
				frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
			}
			break;
		case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
			// Occurs weekly on <comma delimited days of week> at <time>. [Ends on <end date>.]
			frequencySummary << "Occurs weekly on ";
			frequencySummary << getDayOfWeekSequenceAsString(job.schedule.daysOfWeek);
			frequencySummary << " at ";
			frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
			if(job.schedule.endDay > 0)
			{
				frequencySummary << ". Ends on ";
				frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
			}
			break;
		case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
			// Occurs monthly on the last day at <time>. [Ends on <end date>.]
			// OR
			// Occurs monthly on the <day of month>th day at <time>. [Ends on <end date>.]
			frequencySummary << "Occurs monthly on the ";
			if(job.schedule.dayOfMonth == 0)
			{
				frequencySummary << "last";
			}
			else
			{
				frequencySummary << job.schedule.dayOfMonth;
				switch(job.schedule.dayOfMonth)
				{
					case 1:
					case 21:
					case 31:
						frequencySummary << "st";
						break;
					case 2:
					case 22:
						frequencySummary << "nd";
						break;
					case 3:
					case 23:
						frequencySummary << "rd";
						break;
					default:
						frequencySummary << "th";
						
				}
			}
			frequencySummary << " day at ";
			frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
			if(job.schedule.endDay > 0)
			{
				frequencySummary << ". Ends on ";
				frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
			}
			break;
		case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
			// Occurs yearly on the last day of February at <time>. [Ends on <end date>.]
			// OR
			// Occurs yearly on <date> day at <time>. [Ends on <end date>.]
			frequencySummary << "Occurs yearly on ";
			if(job.schedule.dayOfMonth == 0)
			{
				frequencySummary << "the last day of February";
			}
			else
			{
				frequencySummary << getFormattedDate(job.schedule.year, job.schedule.month+1, job.schedule.dayOfMonth);
			}
			frequencySummary << " at ";
			frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
			if(job.schedule.endDay > 0)
			{
				frequencySummary << ". Ends on ";
				frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
			}
			break;
	}

	frequencySummary << ".";
	return frequencySummary.str();
}


std::string SchedulingMessageSender::getFormattedDate(int year, int month, int day)
{
	// dd/mm/yyyy
	std::ostringstream formattedDate;
	formattedDate << get2DigitNumber(day);
	formattedDate << "/";
	formattedDate << get2DigitNumber(month);
	formattedDate << "/";
	formattedDate << year;
	return formattedDate.str();
}


std::string SchedulingMessageSender::getFormattedTime(int hour, int minute)
{
	// hh:mm
	std::ostringstream formattedTime;
	formattedTime << get2DigitNumber(hour);
	formattedTime << ":";
	formattedTime << get2DigitNumber(minute);
	return formattedTime.str();
}


std::string SchedulingMessageSender::get2DigitNumber(int num)
{
	std::ostringstream numAsString;

	if(num < 10)
	{
		numAsString << "0";
	}

	numAsString << num;
	return numAsString.str();
}


std::string SchedulingMessageSender::getDayOfWeekAsString(
	TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day)
{
    switch(day)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monday:
            return "Monday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Tuesday:
            return "Tuesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Wednesday:
            return "Wednesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Thursday:
            return "Thursday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Friday:
            return "Friday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Saturday:
            return "Saturday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Sunday:
            return "Sunday";
        default:
            TA_ASSERT(false, "Unrecognised day of week");
    }
}


std::string SchedulingMessageSender::getDayOfWeekSequenceAsString(
    const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek)
{
    if(daysOfWeek.length() == 0)
    {
        return "";
    }

    std::string daysOfWeekString = getDayOfWeekAsString(daysOfWeek[0]);

    for(unsigned int i = 1; i < daysOfWeek.length(); i ++)
    {
        daysOfWeekString += ", ";
        daysOfWeekString += getDayOfWeekAsString(daysOfWeek[i]);
    }

    return daysOfWeekString;
}


std::string SchedulingMessageSender::getJobCorbaDefAsString(
     const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    std::string description(job.description);

    std::string isInhibited = (job.isInhibited ? "Yes" : "No");


    //
    // Get the job as a string
    //
    std::stringstream jobAsString;
    jobAsString <<
        "ID: '"                   << job.identifier          << "', "  <<
        "Description: '"          << description             << "', " <<
        "isInhibited: "           << isInhibited             << ", "  <<
        "Schedule: '"             << Schedule::toString( job.schedule )       << "', " <<
        std::endl;
    
    return jobAsString.str();
}


void SchedulingMessageSender::jobCompletedAllExecutions(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID)
{
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef runTime;
    runTime.minute = 0;
    runTime.hour = 0;
    runTime.dayOfMonth = 0;
    runTime.month = TA_Base_Bus::SchedulingAgentCorbaDef::January;
    runTime.year = 0;

    jobScheduled(jobID, runTime);
}


}; // TA_Base_App

