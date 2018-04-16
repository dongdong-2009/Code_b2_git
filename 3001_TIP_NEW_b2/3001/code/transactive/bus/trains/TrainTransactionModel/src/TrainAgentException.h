/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/trains/TrainTransactionModel/src/TrainAgentException.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 10:00:53 $
 * Last modified by:  $Author: builder $
 * 
 * The generic train agent exception.
 */

#if !defined(TRAIN_TRAINAGENTEXCEPTION_H)
#define TRAIN_TRAINAGENTEXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
	
	class TrainAgentException : public TA_Base_Core::TransactiveException
	{

	public:

	    /**
	      * TrainAgentException
	      * 
	      * Constructor
	      * 
	      * @return inline 
	      * 
	      * @exception <exceptions> Optional
	      */
 		TrainAgentException() throw();

	    /**
	      * ~TrainAgentException
	      * 
	      * Destructor
	      * 
	      * @return inline virtual 
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual ~TrainAgentException() throw();


	    /**
	      * TrainAgentException
	      * 
	      * Constructor
	      * 
	      * @return inline 
	      * @param : const std::string& msg
	      * 
	      * @exception <exceptions> Optional
	      */
		TrainAgentException(const std::string& msg) throw();

	    /**
	      * const char* what
	      * 
	      * Return Exception Message
	      * 
	      * @return const char* 
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual const char* what() const throw();

	};
}

#endif // !defined(TRAIN_TRAINAGENTEXCEPTION_H)
