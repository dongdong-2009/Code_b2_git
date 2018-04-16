/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * All exceptions in the Scheduling Agent project extend from
  * SchedulingException, which itself extends from TransactiveException.
  *
  */


#ifndef SCHEDULING_EXCEPTION_H
#define SCHEDULING_EXCEPTION_H

#include <string>
#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_App
{

class SchedulingException: public TA_Base_Core::TransactiveException
{

public:

    SchedulingException( const std::string & message )
    : m_message( message )
    {
    }

    SchedulingException( const SchedulingException & rhs )
    {
        if(this != & rhs)
        {
            m_message = rhs.m_message ;
        }
    }

    SchedulingException()
    {
        m_message = "";
    }

    virtual ~SchedulingException() throw () {}

    virtual const char *what() const throw ()
    {
        return m_message.c_str();
    }

private:

    std::string m_message;
};


}; // TA_Base_App

#endif // SCHEDULING_EXCEPTION_H
