/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/modbus_comms/src/SerialPortException.h $
  * @author:  A. Ivankovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * An exception that is thrown as a result of a serial port set up or communications
  * problem.
  */

#ifndef SerialPortException_h
#define SerialPortException_h

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
        class SerialPortException : public TA_Base_Core::TransactiveException  
    {
    public:
        /**
          * SerialPortException
          *
          * Constructor.
          */
        SerialPortException() throw();

        /**
          * SerialPortException
          *
          * Constructor which takes a message string as an argument.
          *
          * @param message  An exception message describing a problem.
          */
        SerialPortException( const std::string & message ) throw();

        /**
          * ~SerialPortException
          *
          * Destructor.
          */
        virtual ~SerialPortException() throw();
    };    
}

#endif // SerialPortException_h
