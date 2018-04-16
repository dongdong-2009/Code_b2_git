#ifndef PlanStepDetailException_H_INCLUDED
#define PlanStepDetailException_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanStepDetailException.h $
  *  @author:  San Teo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  <description>
  */

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class PlanStepDetailException : public TA_Base_Core::TransactiveException
	{
	public:

        /** 
          * PlanStepDetailException
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param type The type of step detail
          * @param stepId The step Id
          *
          */
        PlanStepDetailException(const std::string& msg, const std::string& type, 
            const TA_Base_Core::EPlanStepDetailError error, const unsigned long stepPos);

		
        /** 
          * getErrorType
          *
          * Return the type of error.
          *
          *
          * @return the type of error
          */
        const TA_Base_Core::EPlanStepDetailError getErrorType() const;


        /** 
          * getDetailType
          *
          * Return the detail which has the error
          *
          *
          * @return the detail which has the error
          */
        const std::string getDetailType() const;


        /** 
          * getStepPosition
          *
          * Return the step number for the plan that the error occurred in.
          *
          *
          * @return the step number for the plan.
          */
        const unsigned long getStepPosition() const;

        virtual ~PlanStepDetailException() throw();

	private:

        /** 
          * PlanStepDetailException
          *
          * Force the exception to be constructed using the only constructor above
          *
          */
        PlanStepDetailException();
		
        const TA_Base_Core::EPlanStepDetailError m_errorCode; 
        const std::string m_type;
        const unsigned long m_stepPos;
	};
}; // TA_Base_Core

#endif // PlanStepDetailException_H_INCLUDED



