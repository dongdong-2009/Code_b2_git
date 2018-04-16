/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_server_simulator/src/CommandException.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  */

#ifndef CommandException_H
#define CommandException_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
        class CommandException : public TA_Base_Core::TransactiveException  
    {
    public:
        /**
          * Default constructor.
          */
        CommandException() throw();

        /**
          * Constructor which takes a message string as an argument.
          *
          * @param message  An exception message describing a problem.
          */
        CommandException( const std::string & message ) throw();

        /**
          * Destructor.
          */
        virtual ~CommandException() throw();
    };    
}

#endif // CommandException_H
