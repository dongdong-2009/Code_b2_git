/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/IJobExecutionListener.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * This interface should be implemented by any class that needs to receive
  * events about job instances starting and completing.
  *
  */


#ifndef JOB_EXECUTION_LISTENER_H
#define JOB_EXECUTION_LISTENER_H


#include "app/scheduling/scheduling_agent/src/JobRunner.h"

#include <string>

namespace TA_Base_App
{


class IJobExecutionListener
{

public:

    /**
     *  Called when a job is about to start.
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started,
     *                        relative to the epoch.
     *  @param sessionID      the ID of the session used to execute the job
     */
    virtual void startingJob(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& sessionID)   = 0;


    /**
     *  Called immediately after a job completes.
     *  If there was an error, the errorMessage parameter is not "".
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started,
     *                        relative to the epoch.
     *  @param errorMessage   if there was an error, this contains the
     *                        message.
     *  @param sessionID      the ID of the session used to execute the job
     */
    virtual void jobCompleted(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobID& jobID,
        unsigned long minuteStarted,
        const std::string& errorMessage,
        const std::string& sessionID)  = 0;


    /**
     *  Destructor.
     */
    virtual ~IJobExecutionListener() {}
};


}; // TA_Base_App


#endif // JOB_EXECUTION_LISTENER_H
