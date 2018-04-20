#ifndef PLANNOTFOUNDEXCEPTION_H_INCLUDED
#define PLANNOTFOUNDEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanNotFoundException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/TransactiveException.h"
namespace TA_Base_App
{
    class PlanNotFoundException : public TA_Base_Core::TransactiveException
	{
	public:
        /** 
          * PlanNotFoundException
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param planId The node id of the plan
          * @param instanceId The instance number
          *
          */
        PlanNotFoundException(  const std::string& msg, 
                                const TA_Base_Core::EPlanNotFoundError error, 
                                const TA_Base_Core::NodeId planId = 0, 
                                const unsigned long instanceId = 0) throw();

		
        /** 
          * PlanNotFoundException
          *
          *
          */
        PlanNotFoundException() throw();

        
        virtual const char* what() const throw();
		
		
        /** 
          * getErrorType
          *
          * Returns the error type
          *
          *
          * @return the error type
          */
        const TA_Base_Core::EPlanNotFoundError getErrorType() const
        {
            return m_errorCode;
        }


        /** 
          * getPlanId
          *
          * Returns the Plan id associated with the error
          *
          *
          * @return the Plan Id associated with the error
          */
        const TA_Base_Core::NodeId getPlanId() const
        {
            return m_planId;
        }


        /** 
          * getInstanceId
          *
          * Returns the Instance id associated with the error
          *
          *
          * @return the Instance Id associated with the error
          */
        unsigned long getInstanceId() const
        {
            return m_instanceId;
        }
        
        
		virtual ~PlanNotFoundException() throw();

	private:

		const std::string m_msg;
        TA_Base_Core::EPlanNotFoundError m_errorCode;
        TA_Base_Core::NodeId m_planId;
        unsigned long m_instanceId;
	};

}; // TA_Base_App



#endif // PLANNOTFOUNDEXCEPTION_H_INCLUDED





