/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceStateRetrievalException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Thrown when the state of a job instance cannot be retrieved
  *
  */


#ifndef JOB_INSTANCE_STATE_RETRIEVAL_EXCEPTION_H
#define JOB_INSTANCE_STATE_RETRIEVAL_EXCEPTION_H

#include "SchedulingException.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"

namespace TA_Base_App
{

class JobInstanceStateRetrievalException: public SchedulingException
{

public:

    JobInstanceStateRetrievalException(
        const std::string & message)
        :
        SchedulingException(message)
    {
    }

    JobInstanceStateRetrievalException(const JobInstanceStateRetrievalException & rhs)
        :
        SchedulingException(rhs)
    {
    }

    JobInstanceStateRetrievalException()
        :
        SchedulingException()
    {
    }

    virtual ~JobInstanceStateRetrievalException() throw () {};

    virtual const char *what() const throw ()
    {
        return SchedulingException::what();
    }

};


}; // TA_Base_App

#endif // JOB_INSTANCE_STATE_RETRIEVAL_EXCEPTION_H
