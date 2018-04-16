/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobFactoryInitialisationException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Thrown when there is a problem initialising an IJobFactory subclass.
  *
  */


#ifndef JOB_FACTORY_INITIALISATION_EXCEPTION_H
#define JOB_FACTORY_INITIALISATION_EXCEPTION_H

#include "SchedulingException.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"

namespace TA_Base_App
{

class JobFactoryInitialisationException: public SchedulingException
{

public:

    JobFactoryInitialisationException(
        const std::string & message)
        :
        SchedulingException(message)
    {
    }

    JobFactoryInitialisationException(const JobFactoryInitialisationException & rhs)
        :
        SchedulingException(rhs)
    {
    }

    JobFactoryInitialisationException()
        :
        SchedulingException()
    {
    }

    virtual ~JobFactoryInitialisationException() {};

    virtual const char *what() const
    {
        return SchedulingException::what();
    }

};


}; // TA_Base_App

#endif // JOB_FACTORY_INITIALISATION_EXCEPTION_H
