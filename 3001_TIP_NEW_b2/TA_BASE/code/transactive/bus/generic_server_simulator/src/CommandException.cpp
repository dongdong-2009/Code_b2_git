/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_server_simulator/src/CommandException.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * An exception that is thrown as a result of a problem while validating or
  * parsing Ats Simulator commands
  */


#include "bus/generic_server_simulator/src/CommandException.h"


namespace TA_Base_Bus
{
	CommandException::CommandException() throw()
    {
    }


    CommandException::CommandException( const std::string & message )  throw()
        : TransactiveException( message )
    {
    }


    CommandException::~CommandException() throw()
    {
    }
}
