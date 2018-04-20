#if !defined(AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_)
#define AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/DutyManager/src/MessageBox.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Utility class used to display exception messages using the 
  * standard Windows message box.
  *
  */

namespace TA_Base_App
{
    class MessageBox
    {
    // Operations
    public:
        static void error(const std::string& message, const TA_Base_Core::TransactiveException& exception);
        static void error(const std::string& message, const TA_Base_Bus::DutyMonitorFrameworkException& exception);
        static void error(const std::string& message);
        static void warning(const std::string& message);
        static void info(const std::string& message);

    private:
        static std::string formatMessage(const std::string& message,
                                         const std::string& exceptionMessage,
                                         const std::string& exceptionDetail);

        static void display(const std::string& message, int iconId);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_MESSAGEBOX_H__7DD408A3_F30E_48DF_8B0A_3399C6E64E52__INCLUDED_)
