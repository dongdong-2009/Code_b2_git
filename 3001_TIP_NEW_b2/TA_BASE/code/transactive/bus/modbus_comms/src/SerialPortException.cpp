/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/modbus_comms/src/SerialPortException.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * An exception that is thrown as a result of a serial port set up or communications
  * problem.
  */

#include "SerialPortException.h"


namespace TA_Base_Bus
{
    SerialPortException::SerialPortException() throw()
    {
    }


    SerialPortException::SerialPortException( const std::string & message )  throw()
        : TransactiveException( message )
    {
    }


    SerialPortException::~SerialPortException() throw()
    {
    }
}
