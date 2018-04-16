/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/IAnswerBackErrorNotification.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#ifndef IANSWER_BACK_ERROR_NOTIFICATION_H
#define IANSWER_BACK_ERROR_NOTIFICATION_H


namespace TA_Base_Bus
{

	class IAnswerBackErrorNotification
	{
	public:

		/**
		 * processPostEvaluationStep
		 *
		 * Call back for processing the answer back error checking result
		 *
		 * @param 	answerBackErrorCheckingResult	The result of the AnswerBackError checking mechanism
		 *
		 */
		virtual void processPostEvaluationStep ( bool answerBackErrorCheckingResult ) = 0;
	};

}

#endif
