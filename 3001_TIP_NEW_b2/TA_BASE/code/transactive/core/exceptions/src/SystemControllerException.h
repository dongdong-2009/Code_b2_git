/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/SystemControllerException.h $
  * @author:  <Rob Ashcroft>
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Exception class used by the System Controller and
  * System Manager.
  */


#if !defined(AFX_SYSTEMCONTROLLEREXCEPTION_H__BCECD7DE_CBB1_47E8_BA92_5D48199EC772__INCLUDED_)
#define AFX_SYSTEMCONTROLLEREXCEPTION_H__BCECD7DE_CBB1_47E8_BA92_5D48199EC772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{

    class SystemControllerException : public TA_Base_Core::TransactiveException  
    {
    public:

		/**
		  * Default Constructor
		  */
        SystemControllerException() throw();

		/**
		  * Constructor
		  *
		  * @param msg  The reason why the exception was thrown.
		  */
        SystemControllerException(const std::string& msg) throw();

		/**
		  * what
		  *
		  * Returns the message associated with the exception.
		  */
        virtual const char* what() const throw();

		/**
		  * Destructor
		  */
	    virtual ~SystemControllerException() throw();

    protected:

        // This is where any exception messages are stored
        std::string m_message;
    };
 
};

#endif // !defined(AFX_SYSTEMCONTROLLEREXCEPTION_H__BCECD7DE_CBB1_47E8_BA92_5D48199EC772__INCLUDED_)
