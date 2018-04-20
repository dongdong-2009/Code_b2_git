#ifndef PlanStepException_H_INCLUDED
#define PlanStepException_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/PlanStepException.h $
  *  @author:  San Teo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
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
        PlanStepException(const std::string& msg, const TA_Base_Core::EStep step) throw();

        
        /** 
          * getStepType
          *
          * Return the type of step.
          *
          *
          * @return the type of step
          */
        const TA_Base_Core::EStep getStepType() const
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
		
        const TA_Base_Core::EStep m_stepType;
	};
}; // TA_Base_Core

#endif // PlanStepException_H_INCLUDED



