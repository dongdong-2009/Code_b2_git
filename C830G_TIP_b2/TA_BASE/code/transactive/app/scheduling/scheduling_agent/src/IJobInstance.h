/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/IJobInstance.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This interface represents an instance of an executable job.
  *
  */



#ifndef IJOB_INSTANCE_H
#define IJOB_INSTANCE_H


#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
//#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"



namespace TA_Base_App
{


// Forward declaration
class IJob;


class IJobInstance
{

public:

    typedef enum JobInstanceState
    {
        NotStarted,
        Started,
        Completed
    };


    /**
     *  Gets the parent job of this instance.
     */
	virtual IJob* getJob() = 0;

    /**
     *  Returns the minute at which the job instance started.
     */
    virtual unsigned long getMinuteStarted() const = 0;

    /**
     *  Executes the job.
     *
     *  This method must execute the job, and return after the job has
     *  finished.
     *
     *  @exception JobInstanceExecutionException
     *             thrown if there is a problem executing the job
     */
    virtual void execute() = 0;


    /**
     *  Destructor.
     */
    virtual ~IJobInstance() {}
};


}; // TA_Base_App

#endif // IJOB_INSTANCE_H
