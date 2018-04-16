/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceCreationException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Thrown when a job instance cannot be created.
  *
  */


#ifndef JOB_INSTANCE_CREATION_EXCEPTION_H
#define JOB_INSTANCE_CREATION_EXCEPTION_H

#include "SchedulingException.h"

namespace TA_Base_App
{

class JobInstanceCreationException: public SchedulingException
{

public:

    JobInstanceCreationException(
        const std::string & message)
        :
        SchedulingException(message)
    {
    }

    JobInstanceCreationException(const JobInstanceCreationException & rhs)
        :
        SchedulingException(rhs)
    {
    }

    JobInstanceCreationException()
        :
        SchedulingException()
    {
    }

    virtual ~JobInstanceCreationException() throw () {}

    virtual const char *what() const throw ()
    {
        return SchedulingException::what();
    }

};


}; // TA_Base_App

#endif // JOB_INSTANCE_CREATION_EXCEPTION_H
