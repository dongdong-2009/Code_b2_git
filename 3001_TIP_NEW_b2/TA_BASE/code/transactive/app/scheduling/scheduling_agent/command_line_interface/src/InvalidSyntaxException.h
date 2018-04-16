/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/InvalidSyntaxException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Thrown if there is a syntax error with a command line string.
  */


#ifndef INVALID_SYNTAX_EXCEPTION_H
#define INVALID_SYNTAX_EXCEPTION_H

#include <exception>
#include <string>

namespace TA_Base_App
{


class InvalidSyntaxException: public std::exception
{

public:

    InvalidSyntaxException( const std::string & message )
    : m_message( message )
    {
    }

    InvalidSyntaxException( const InvalidSyntaxException & rhs )
    {
        if(this != & rhs)
        {
            m_message = rhs.m_message;
        }
    }

    InvalidSyntaxException()
    {
        m_message = "";
    }

    virtual ~InvalidSyntaxException() throw () {}

    virtual const char *what() const throw ()
    {
        return m_message.c_str();
    }

private:

    std::string m_message;
};


} // TA_Base_App

#endif INVALID_SYNTAX_EXCEPTION_H
