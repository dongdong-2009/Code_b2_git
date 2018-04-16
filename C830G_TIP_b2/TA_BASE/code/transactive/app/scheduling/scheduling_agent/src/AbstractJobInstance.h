/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJobInstance.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This abstract class represents an instance of an executable job.
  * It implements the getJob and getMinuteStarted methods of the
  * IJobInstance interface.
  *
  */



#ifndef ABSTRACT_JOB_INSTANCE_H
#define ABSTRACT_JOB_INSTANCE_H


#include "app/scheduling/scheduling_agent/src/IJobInstance.h"



namespace TA_Base_App
{


// Forward declaration
class IJob;


class AbstractJobInstance : public IJobInstance
{

public:

    /**
     *  Gets the parent job of this instance.
     */
	IJob* getJob();

    /**
     *  Returns the minute at which the job instance started.
     */
    unsigned long getMinuteStarted() const;

    /**
     *  Destructor.
     */
	virtual ~AbstractJobInstance();


    /**
     *  Executes the job.
     *
     *  This method must execute the job, and return after the job has
     *  finished. It is not implemented by this class.
     *
     *  @exception JobInstanceExecutionException
     *             thrown if there is a problem executing the job
     */
    virtual void execute() = 0;

	
protected:

	/**
	 *  Implementers of the IJobInstance interface should call the
     *  AbstractJobInstance constructor from within the body of their
     *  constructor.
     *
	 *  @param job            the parent job.
	 *  @param minuteStarted  the minute at which the instance was started.
	 */
	AbstractJobInstance(
        const IJob* job,
        const unsigned long minuteStarted);



private:

    AbstractJobInstance();
    AbstractJobInstance(const AbstractJobInstance&);
    const AbstractJobInstance& operator=(const AbstractJobInstance&);


    IJob*         m_job;
    unsigned long m_minuteStarted;
};


}; // TA_Base_App

#endif // ABSTRACT_JOB_INSTANCE_H
