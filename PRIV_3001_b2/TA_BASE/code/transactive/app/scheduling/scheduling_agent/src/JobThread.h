/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobThread.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

    void markAsDeleted( const bool bDeleted = true );

    bool isMarkDeleted();

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
    bool m_bDeleted;
};


}; // TA_Base_App

#endif // JOB_THREAD_H
