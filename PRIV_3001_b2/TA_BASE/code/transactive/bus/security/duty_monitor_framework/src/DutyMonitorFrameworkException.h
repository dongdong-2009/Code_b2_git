#if !defined(_DUTYMONITORFRAMEWORKEXCEPTION_H__5C0A4434_39D4_4B10_B61F_E0BC123999AD__INCLUDED_)
#define _DUTYMONITORFRAMEWORKEXCEPTION_H__5C0A4434_39D4_4B10_B61F_E0BC123999AD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_monitor_framework/src/DutyMonitorFrameworkException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Thrown when an internal Duty Monitor Framework error occurs.
  *
  */

namespace TA_Base_Bus
{
    class DutyMonitorFrameworkException : public TA_Base_Core::TransactiveException  
    {
    public:
        DutyMonitorFrameworkException() throw();
        DutyMonitorFrameworkException(const std::string& msg) throw();
        DutyMonitorFrameworkException(const std::string& msg, const std::string& details) throw();
        virtual ~DutyMonitorFrameworkException() throw();

		virtual const char* getDetails() const throw();


    private:
        std::string m_details;  // additional error details to complement the message
    };
}

#endif // !defined(_DUTYMONITORFRAMEWORKEXCEPTION_H__5C0A4434_39D4_4B10_B61F_E0BC123999AD__INCLUDED_)
