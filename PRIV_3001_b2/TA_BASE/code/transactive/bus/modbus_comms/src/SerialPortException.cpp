/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/modbus_comms/src/SerialPortException.cpp $
  * @author:  A. Ivankovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
