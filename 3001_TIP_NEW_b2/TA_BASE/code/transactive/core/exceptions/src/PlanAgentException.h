#ifndef PLANAGENTEXCEPTION_H_INCLUDED
#define PLANAGENTEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/PlanAgentException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  <description>
  */
#include "TransactiveException.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
    class PlanAgentException : public TA_Base_Core::TransactiveException  
	{
	public:
        /** 
          * PlanAgentException
          *
          * description
          *
          * @param msg The message to be logged.
          * @param error The type of error
          * @param itemId This can be any id, e.g. operator id, plan id etc.
          *
          */
        PlanAgentException(const std::string& msg, const TA_Base_Core::EPlanAgentError error, const unsigned long itemId = 0) throw();

		PlanAgentException() throw();


		virtual const char* what() const throw();
		

        /** 
          * getErrorCode
          *
          * Returns the error code
          *
          *
          * @return the error code
          */
        TA_Base_Core::EPlanAgentError getErrorType() const
        {
            return m_errorCode;
        }


        /** 
          * getItemId
          *
          * Returns the additional item id associated with the error
          *
          *
          * @return the item Id associated with the error
          */
        unsigned long getItemId() const
        {
            return m_itemId;
        }

        
        virtual ~PlanAgentException() throw();
	private:


		const std::string m_msg;
        const TA_Base_Core::EPlanAgentError m_errorCode;
        const unsigned long m_itemId;
	};

}; // TA_Base_Core



#endif // PLANAGENTEXCEPTION_H_INCLUDED





