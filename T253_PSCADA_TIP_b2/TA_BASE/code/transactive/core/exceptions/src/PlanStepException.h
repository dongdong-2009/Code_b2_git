#ifndef PlanStepException_H_INCLUDED
#define PlanStepException_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanStepException.h $
  *  @author:  San Teo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class PlanStepException : public TA_Base_Core::TransactiveException
	{
	public:

        /** 
          * PlanStepException
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param type The type of step detail
          * @param stepId The step Id
          *
          */
        PlanStepException(const std::string& msg, const int step) throw();

        
        /** 
          * getStepType
          *
          * Return the type of step.
          *
          *
          * @return the type of step
          */
        const int getStepType() const
        {
            return m_stepType;
        }


        virtual ~PlanStepException() throw();
	private:

        /** 
          * PlanStepException
          *
          * Force the exception to be constructed using the only constructor above
          *
          */
        PlanStepException() throw();
		
        const int m_stepType;
	};
}; // TA_Base_Core

#endif // PlanStepException_H_INCLUDED



