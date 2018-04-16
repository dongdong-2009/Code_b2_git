/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_server_simulator/src/CommandException.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
