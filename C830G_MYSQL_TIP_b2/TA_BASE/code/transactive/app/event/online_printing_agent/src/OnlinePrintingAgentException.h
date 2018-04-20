#if !defined(ONLINEPRINTINGAGENTEXCEPTION_H_INCLUDED)
#define ONLINEPRINTINGAGENTEXCEPTION_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Thrown when an internal Online Printing Agent error occurs.
  *
  */

namespace TA_Base_App
{
    class OnlinePrintingAgentException : public TA_Base_Core::TransactiveException  
    {
    // Operations
    public:
        OnlinePrintingAgentException() throw();
        OnlinePrintingAgentException(const std::string& msg) throw();
        OnlinePrintingAgentException(const std::string& msg, const std::string& details) throw();
        virtual ~OnlinePrintingAgentException() throw();

		virtual const char* getDetails() const throw();


    // Attributes
    private:
        std::string m_details;  // additional error details to complement the message
    };
}

#endif // !defined(ONLINEPRINTINGAGENTEXCEPTION_H_INCLUDED)
