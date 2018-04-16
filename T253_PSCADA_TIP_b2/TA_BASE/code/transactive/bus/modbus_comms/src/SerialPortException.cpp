/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/modbus_comms/src/SerialPortException.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
