/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingMessageSender.h $
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


#ifndef SCHEDULING_MESSAGE_SENDER_H
#define SCHEDULING_MESSAGE_SENDER_H

#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
//#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"


#include <string>

namespace TA_Base_App
{



class SchedulingMessageSender
{

public:


	/**
	 *  Gets the single instance of this class.
     *  Note that the first time this method is called, initialise must be
     *  called.
	 */
	static SchedulingMessageSender& getInstance();


    /**
     *  Initialises the object.
     *
     *  This method is used instead of a constructor.
     *  It must be called the first time getInstance is invoked, or
     *  subsequent invocations will cause an assertion.
     *
     *  @param entityKey     the entity key of the Scheduling Agent.
     *  @param subsystemKey  the subsystem key of the Scheduling Agent.
     *  @param locationKey   the location key of the Scheduling Agent.
     */
    void SchedulingMessageSender::initialise(
        unsigned long entityKey,
		unsigned long parentEntityKey,
        unsigned long subsystemKey,
        unsigned long locationKey);

    
    /**
     *  Called when a job is created.
     */
    void SchedulingMessageSender::jobAdded(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
        const std::string& sessionKey);


    /**
     *  Called when a job is scheduled.
     */
    void SchedulingMessageSender::jobScheduled(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        const TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef& runTime );

    /**
     *  Called when a job is updated.
     */
    void SchedulingMessageSender::jobUpdated(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
        const std::string& sessionKey,
		  bool changeEnabled);


    /**
     *  Called when a job is removed.
     */
    void SchedulingMessageSender::jobDeleted(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        const std::string& sessionKey);


    /**
     *  Called when a job is about to start.
     */
    void SchedulingMessageSender::jobStarting(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& sessionKey);

    /**
     *  Called when a job is completed.
     */
    void SchedulingMessageSender::jobCompleted(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& sessionKey,
        const std::string& errorMessage);


    /** 
      * jobCompletedAllExecutions
      *
      * Called when a job has completed all its executions
      *
      * @param jobID
      */
    void jobCompletedAllExecutions(const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID);

    /**
     *  Destructor.
     */
    ~SchedulingMessageSender();



private:

	SchedulingMessageSender();
    SchedulingMessageSender(const SchedulingMessageSender&);
    const SchedulingMessageSender& operator=(const SchedulingMessageSender&);


    /**
     *  Gets a JobCorbaDef as a string.
     */
    std::string SchedulingMessageSender::getJobCorbaDefAsString(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);

    /**
     * Returns a string describing the occurrence frequency of this job,
	 * as well as info pertaining to the start and end dates of the job
     */
	std::string SchedulingMessageSender::getFrequencySummary(
		const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);

    /**
     * Returns a date in the format dd/mm/yyyy
     */
	std::string SchedulingMessageSender::getFormattedDate(int year, int month, int day);

    /**
     * Returns a time in the format hh:mm
     */
	std::string SchedulingMessageSender::getFormattedTime(int hour, int minute);

    /**
     * Returns the given number as at least a 2-digit string.
	 *    eg.   4 ==> "04"
	 *         12 ==> "12"
	 *        255 ==> "255"
     */
	std::string SchedulingMessageSender::get2DigitNumber(int num);

    /**
     * Returns a string representation of the SchedulingAgentCorbaDef day
     */
	std::string SchedulingMessageSender::getDayOfWeekAsString(
		TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day);

    /**
     * Returns a comma delimited list of days in the SchedulingAgentCorbaDef day sequence
     */
	std::string SchedulingMessageSender::getDayOfWeekSequenceAsString(
		const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek);

    // Audit message sender
    TA_Base_Core::AuditMessageSender* m_auditSender;

    // Comms message sender
    TA_Base_Core::CommsMessageSender* m_commsSender;

    // State update message sender
    TA_Base_Core::StateUpdateMessageSender* m_stateSender;

    // The Scheduling Agent's entity key
    unsigned long m_entityKey;

    // The Scheduling Agent's parent entity key
    unsigned long m_parentEntityKey;

    // The Scheduling Agent's subsystem key
    unsigned long m_subsystemKey;

    // The Scheduling Agent's location key
    unsigned long m_locationKey;


    /** The single instance of this class */
    static SchedulingMessageSender* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};



}; // TA_Base_App


#endif // SCHEDULING_MESSAGE_SENDER_H
