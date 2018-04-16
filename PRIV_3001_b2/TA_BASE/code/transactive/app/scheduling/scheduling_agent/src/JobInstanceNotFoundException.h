/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Thrown when a job instance cannot be found.
  *
  */


#ifndef JOB_INSTANCE_NOT_FOUND_EXCEPTION_H
#define JOB_INSTANCE_NOT_FOUND_EXCEPTION_H

#include "SchedulingException.h"

namespace TA_Base_App
{

class JobInstanceNotFoundException: public SchedulingException
{

public:

    JobInstanceNotFoundException( const std::string & message )
        :
        SchedulingException(message)
    {
    }


    virtual ~JobInstanceNotFoundException() throw () {};

    virtual const char *what() const throw ()
    {
        return SchedulingException::what();
    }

private:
	JobInstanceNotFoundException();

};


}; // TA_Base_App

#endif // JOB_INSTANCE_NOT_FOUND_EXCEPTION_H
