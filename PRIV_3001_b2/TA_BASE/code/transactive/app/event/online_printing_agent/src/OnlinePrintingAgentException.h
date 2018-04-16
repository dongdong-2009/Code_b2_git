#if !defined(ONLINEPRINTINGAGENTEXCEPTION_H_INCLUDED)
#define ONLINEPRINTINGAGENTEXCEPTION_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
