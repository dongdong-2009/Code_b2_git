/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/trains/TrainTransactionModel/src/TrainTransmissionFailureException.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 10:00:53 $
 * Last modified by:  $Author: builder $
 * 
 * Thrown when a command cannot be sent to the train via the radio interface.
 */

#if !defined(TRAIN_TRAINTRANSMISSIONFAILUREEXCEPTION_H)
#define TRAIN_TRAINTRANSMISSIONFAILUREEXCEPTION_H


#include "bus/trains/TrainTransactionModel/src/TrainAgentException.h"
namespace TA_IRS_Bus
{

	class TrainTransmissionFailureException : public TrainAgentException
	{

	public:

	    /**
	      * TrainTransmissionFailureException
	      * 
	      * Default Constructor
	      * 
	      * @return inline 
	      * 
	      * @exception <exceptions> Optional
	      */
		TrainTransmissionFailureException() throw();

	    /**
	      * ~TrainTransmissionFailureException
	      * 
	      * Destructor
	      * 
	      * @return virtual 
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual ~TrainTransmissionFailureException() throw();

	
	    /**
	      * TrainTransmissionFailureException
	      * 
	      * Constructor
	      * 
	      * @return inline 
	      * @param : const std::string& msg
	      * 
	      * @exception <exceptions> Optional
	      */
		TrainTransmissionFailureException(const std::string& msg) throw();

	    /**
	      * const char* what
	      * 
	      * Return Exception message
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual const char* what() const throw();

	};

}
#endif // !defined(TRAIN_TRAINTRANSMISSIONFAILUREEXCEPTION_H)

