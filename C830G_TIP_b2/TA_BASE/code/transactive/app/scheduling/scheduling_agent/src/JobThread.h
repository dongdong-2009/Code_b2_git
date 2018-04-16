/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobThread.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Runs an instance of a job.
  *
  */


#ifndef JOB_THREAD_H
#define JOB_THREAD_H


#include "core/threads/src/Thread.h"


namespace TA_Base_App
{

// Forward declaration
class IJobInstance;



class JobThread:  public TA_Base_Core::Thread
{

public:


    /**
     *  Constructor
     */
    JobThread(const IJobInstance* jobInstance);

    /**
     *  Destructor.
     */
	virtual ~JobThread();


    /**
     *  Gets the job instance that this object runs.
     */
    IJobInstance* getJobInstance();
    


private:

    /**
     *  Called by the start method.
     */
    void run();

    /**
     *  This method does nothing, as the run method will only return
     *  when IJobInstance's execute method returns.
     */
    void terminate();



    JobThread();
    JobThread(const JobThread&);
    const JobThread& operator=(const JobThread&);


    IJobInstance* m_jobInstance;

};


}; // TA_Base_App

#endif // JOB_THREAD_H
