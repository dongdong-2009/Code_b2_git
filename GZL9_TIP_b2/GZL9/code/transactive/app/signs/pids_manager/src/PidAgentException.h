/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/PidAgentException.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Exception class that signifies error while communication with PID Agent
  */

#ifndef PIDAGENTEXCEPTION_H
#define PIDAGENTEXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"


class PidAgentException : public TA_Base_Core::TransactiveException  
{
public:

    /**
	 * PidAgentException
     *
     * Initialization constructor.
	 * 
	 * * @param : const std::string& description
     *            a description appropriate to the failure
	 */
    PidAgentException(const std::string& description) 
    :   TransactiveException(description) 
    {
    };

    /**
	  * ~PidAgentException
	  * 
	  * Destructor
	  */
    virtual ~PidAgentException() {};

private:

	/**
	  * PidAgentException
	  * 
	  * Default constructor (disabled)
	  */
    PidAgentException();
};

#endif
