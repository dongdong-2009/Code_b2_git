/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Thrown when a call entry does not exist in the CallEntryManager for
  * get and delete methods.
  */


#ifndef CALL_ENTRY_DOES_NOT_EXIST_EXCEPTION_H
#define CALL_ENTRY_DOES_NOT_EXIST_EXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{

    class CallEntryDoesNotExistException: public TA_Base_Core::TransactiveException
    {

    public:

        CallEntryDoesNotExistException( const std::string & message )
            :
            m_message( message )
        {
        }

        CallEntryDoesNotExistException( const CallEntryDoesNotExistException & rhs )
        {
            if(this != & rhs)
            {
                m_message = rhs.m_message;
            }
        }

        CallEntryDoesNotExistException()
        {
            m_message = "";
        }

        virtual ~CallEntryDoesNotExistException() throw () {}

        virtual const char *what() const throw ()
        {
            return m_message.c_str();
        }

    private:

        std::string m_message;
    };


}; // TA_IRS_Bus

#endif // CALL_ENTRY_DOES_NOT_EXIST_EXCEPTION_H
