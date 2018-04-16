// TransActiveMessage.h: interface for the TransActiveMessage class.
//
//////////////////////////////////////////////////////////////////////
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/src/TransActiveMessage.h $
  * @author:  Brett Zimmerman
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/09/26 09:05:01 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Standard message box format, display and logging functions
  *    Important Note : Class TransActiveMessage is not thread safe.
  */

#if !defined(AFX_TRANSACTIVEMESSAGE_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_)
#define AFX_TRANSACTIVEMESSAGE_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/TA_MessageResource/Resource.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <sstream>
#include <vector>
#include <set>

namespace TA_Base_Bus
{
    // basic argument interface
    class ITypedArgument
    {
    public:
        virtual CString getFormattedValue(void) const = 0;
        virtual CString getFormattedValue(LPCTSTR format) const = 0;
        virtual ~ITypedArgument() {};
    };

    template <class T> class TypedArgument : public ITypedArgument
    {
    public:

        // constructor
        TypedArgument(T value) : m_value(value) {};

        // destructor
        virtual ~TypedArgument() {};

        // get the value as a formatted string using system defaults
        virtual CString getFormattedValue(void) const
        {
            std::stringstream s;
            s << m_value;
            return CString(s.str().c_str());
        };

        // Return the value as a string using the input format. See MSDN sprintf for options.
        virtual CString getFormattedValue(LPCTSTR format) const
        {
            CString sReturn;
            sReturn.Format(format, m_value);
            return sReturn;
        }

        // get the value in its native type
        T getValue(void) const
        {
            return m_value;
        };

    private:
        T m_value;

        TypedArgument();
        TypedArgument(TypedArgument& arg);
        TypedArgument& operator=(const TypedArgument& arg);
    };

    class StdStringArgument : public TypedArgument<std::string>
    {
    public:
        StdStringArgument(const std::string& s) : TypedArgument<std::string>(s) {}

        virtual CString getFormattedValue(LPCTSTR format) const
        {
            CString sReturn;
            sReturn.Format(format, getValue().c_str());
            return sReturn;
        }
    };

    // some basic types...
    typedef TypedArgument<bool>             BoolArg;
    typedef TypedArgument<short>            ShortArg;
    typedef TypedArgument<int>              IntArg;
    typedef TypedArgument<LPCTSTR>          StrArg;
    typedef TypedArgument<float>            FloatArg;
    typedef TypedArgument<unsigned long>    ULongArg;
    typedef StdStringArgument               StdStringArg;

    // a collection of arguments
    // ... with auto delete
    class TypedArguments : public std::vector<ITypedArgument*>
    {
    public:
        virtual ~TypedArguments()
        {
            clearAll();
        };

        inline void clearAll()
        {
            // free the memory
            for (TypedArguments::iterator i = begin(); i != end(); i++)
            {
                delete *i;
            }

            // empty the vector
            clear();
        }

        // Generic input operators
        template<typename _T> inline TypedArguments& operator << (const _T& Arg)
        { push_back(new TypedArgument<_T>(Arg)); return *this; }

        // Specialised input operator for std::string
        inline TypedArguments& operator << (const std::string& szArg)
        { push_back(new StdStringArg(szArg)); return *this; }

        // Specialised input operator for strings (prevents interpreting the input as a char array)
        inline TypedArguments& operator << (LPCTSTR szArg)
        { push_back(new StrArg(szArg)); return *this; }
    };

    class TransActiveMessage
    {
    public:
        /**
          * ConstructMessage
          *
          * @param uiMessageID - Resource identifier of the string to display
          *
          * Constructs a formatted string using the loaded string resource identifier
          * and the pre-input arguments.
          * Format of the string table entry must match - MessageBoxStyle|HelpID|AuditInteraction|Message
          */
        CString constructMessage(UINT uiMessageID);

        /**
          * ShowMsgBox
          *
          * @param uiMessageID - Resource identifier of the string to display
          *
          * Displays a message box with text and appearance loaded from the input string resource
          * identifier. Returns the result of ::MessageBox, 0 upon failure.
          */
        UINT showMsgBox(UINT uiMessageID, const CString& caption = ""); //TD19032 Coreteam

        /**
          * Constructor
          *
          * Creates a TransActiveMessage object loading TA_MessageResource.dll from the current directory
          * with a language matching the users default language.
          */
        TransActiveMessage();

        /**
          * Constructor
          *
          * @param hResourceDll - Handle of the resource instance.
          * @param lcidLang - LCID of the language to load from the resource library.
          *
          * Creates a TransActiveMessage object with its resource library set to hResourceDll and the
          * language set to lcidLang
          */
        /*TransActiveMessage(HINSTANCE hResourceDll, LCID lcidLang);*/

        /**
          * Destructor
          *
          * Free resource library if loaded and destroy TransActiveMessage object.
          */
        virtual ~TransActiveMessage();

        /**
          * Generic parameter vector input operators
          *
          * @param Arg - Parameter to be inserted into the message arguments member vector.
          *
          * Adds the input paramater to arguments for the message.
          */
        template<typename T> inline TransActiveMessage& operator << (const T& Arg)
        { m_args.push_back(new TypedArgument<T>(Arg)); return *this; }

        /**
          * Specialised parameter vector input operator for std::string
          *
          * @param Arg - std::string parameter to be inserted into the message arguments member vector.
          *
          * Adds the input paramater to arguments for the message.
          */
        inline TransActiveMessage& operator << (const std::string& Arg)
        { m_args.push_back(new StdStringArg(Arg)); return *this; }
        inline TransActiveMessage& operator << (const char* Arg)
        { m_args.push_back(new StdStringArg(Arg)); return *this; }

        /**
          * Specialised parameter vector input operator for char array
          *
          * @param Arg - TCHAR* parameter to be inserted into the message arguments member vector.
          *
          * Adds the input paramater to arguments for the message.
          * Specialised to prevent compiler interpreting strings as char arrays.
          */
        inline TransActiveMessage& operator << (const CString& Arg)
        { m_args.push_back(new StrArg(Arg)); return *this; }

        /**
          * ClearArguments
          *
          * Empties the member vector of message arguments
          */
        inline void clearArguments() { m_args.clearAll(); }

        /**
          * closeAllMessages
          *
          * Close all popup windows
          *
          */
        static void closeAllMessages();

    private:

        /**
          * GetRawMessage
          *
          * @param uiMessageID - Resource identifier of the string to load.
          * @param sMessage - [OUT] Address of the string to load the message into.
          *
          * @return - true if successful, false otherwise.
          *
          * Loads the string resource from m_hResourceDll into sMessage.
          */
        bool getRawMessage(UINT uiMessageID, CString& sMessage);

        /**
          * ParseRawMessage
          *
          * @param sRawMessage - [IN] Message in its raw format. Must have format - MessageBoxStyle|HelpID|Message
          * @param uiMsgBoxType - [OUT] UINT to hold the MessageBox style extracted from the raw message string.
          * @param uiHelpID - [OUT] UINT to hold the help topic identifier extracted from the raw message string.
          * @param bAudit - [OUT] bool that indicates whether the message needs to be logged or not
          * @param sMessage - [OUT] CString to take the message component of the raw message string.
          *
          * @return - true if successful, false otherwise.
          *
          * This functions breaks the raw message into its three attributes.
          */
        bool parseRawMessage(CString& sRawMessage, UINT& uiMsgBoxType, UINT& uiHelpID, bool& bAudit, CString& sMessage);

        /**
          * ConstructMessage
          *
          * @param sRawMessage - [IN/OUT] String with place markers for args. Contains formated string on exit.
          *
          * @return - true if successful, false otherwise.
          *
          * Constructs a formated message using the pre input arguments.
          */
        bool constructMessage(CString& sRawMessage);

        /**
          * LogMessage
          *
          * @param pszCaption - Caption from the message box.
          * @param pszMessage - Message from the message box.
          * @param uiResult - Exit result returned by ::MessageBox.
          * @param bAuditEvent - Raise an audit event
          *
          * @return - true if successful, false otherwise.
          *
          * Adds the information passed in to the log when the logging level is set to info.
          */
        bool logMessage(LPCTSTR pszCaption, LPCTSTR pszMessage, UINT uiResult, BOOL bAuditEvent);

        /**
          * SetHelpContext
          *
          * @param uiHelp - Context sensitive help identifer to set.
          * @param pdwContext - [OUT] Context pointer used to restore the help context for the App.
          * @param dwPrevContext - [OUT] Previous help identifier used to restore the help.
          *
          * @return - Returns true if the help context could be set, fale otherwise.
          *
          * Sets the help context to the input topic identifier. ResetHelpContext must be called
          * to restore the help context to its previous state.
          */
        bool setHelpContext(UINT uiHelp, DWORD* pdwContext, DWORD& dwPrevContext);

        /**
          * ResetHelpContext
          *
          * @param pdwContext - Context pointer for the App.
          * @param dwPrevContext - Previous help identifier.
          *
          * Restores the help context to the state prior to the call to SetHelpContext
          *
          */
        void resetHelpContext(DWORD* pdwContext, DWORD dwPrevContext);

        /**
          * ReformatCurlyBraces
          *
          * @param sMessage - [IN/OUT] String to alter.
          *
          * Replaces '{{' with '{', and '}}' with '}'.
          */
        void reformatCurlyBraces(CString& sMessage);

        /**
          * WindowsError
          *
          * @param dwError - Error code to retrieve.
          *
          * Retrieves the error string for the input code and adds it to the log.
          */
        void windowsError(DWORD dwError);

        /**
          * FormatCaption
          *
          * @param sCaption - [OUT] Formated caption.
          * @param uiMessageID - Message identifer added to the caption
          *
          * Creates a message box caption with a format ApplicationName - [MessageIdentifier].
          */
        void formatCaption(CString& sCaption, UINT uiMessageID);

        /**
          * SendAuditMessage
          *
          * @param pszMessage - Formatted text displayed in the message box body
          * @param pszCaption - Text displayed in the message box caption
          * @param pszResult - Button selected to dismiss the message box.
          *
          * Raises an Audit Event with the input information.
          */
        bool sendAuditMessage(LPCTSTR pszMessage, LPCTSTR pszCaption, LPCTSTR pszResult);

    private:

        TransActiveMessage(const TransActiveMessage& rhs);
        TransActiveMessage& operator=(const TransActiveMessage&);

        /**
          * DoMsgBox
          *
          * @param pszCaption - String to display in the title bar of the message box
          * @param pszMessage - String to display in the body of the message box.
          * @param uiMsgBoxType - Message box style options e.g MB_YESNO- see MSDN ::MessageBox for details
          * @param uiHelpID - Help topic identifier.
          * @param bAuditEvent - Raise an Audit Event
          *
          * Displays a message box with text and appearance loaded from the input string resource
          * identifier. Returns the result of ::MessageBox, 0 upon failure.
          *
          */
        UINT doMsgBox(LPCTSTR pszCaption, LPCTSTR pszMessage, UINT uiMsgBoxType, UINT uiHelpID = 0,
                      BOOL bAuditEvent = FALSE);

        /**
          * determineParentWindow
          *
          * Determines the parent window for the message box.
          */
        static HWND determineParentWindow();

        /**
          * CBTProc
          *
          * The CBTProc hook procedure is an application-defined or library-defined callback
          * function used with the SetWindowsHookEx function. The system calls this function
          * before activating, creating, destroying, minimizing, maximizing, moving, or sizing a window;
          *
          * This is a fix for the MessageBox to get it to centre over the application
          * itself rather than centred over the desktop.  (TD392)
          *
          * @param nCode
          * @param wParam
          * @param lParam
          *
          * @return
          *
          */
    public:
        static LRESULT CALLBACK CBTProc(INT nCode, WPARAM wParam, LPARAM lParam);
    private:

        // Type safe message arguments vector
        TypedArguments m_args;

        HINSTANCE m_hResourceDll;
        // Handle of the resource library.

        LCID m_lcidCurrentLang;
        // Language of the strings to load.

        CString m_sResourceFilePath;
        // File name and path of the resource dll to load

        bool m_bFreeLibrary;
        // Flag to indicate the resource library has been loaded internally, and
        // needs to be released upon destruction

        // A CBT hook that hooks into the window procedure that is called before the message box is activated
        static HHOOK s_hhk;

        // protects the s_hhk from being modified from another thread
        static TA_Base_Core::ReEntrantThreadLockable s_hhkLock;

    public:
        // This stores the window handle for all message boxes that are ever displayed
        static std::set<HWND> s_popupHWnd;
    };
} // End namespace TA_Base_Bus

inline UINT showMsgBox(int uiMessageID)
{
    return TA_Base_Bus::TransActiveMessage().showMsgBox(uiMessageID);
}

template<typename T1>
UINT showMsgBox(int uiMessageID, const T1& p1)
{
    TA_Base_Bus::TransActiveMessage msg;
    msg << p1;
    return msg.showMsgBox(uiMessageID);
}

template<typename T1, typename T2>
UINT showMsgBox(int uiMessageID, const T1& p1, const T2& p2)
{
    TA_Base_Bus::TransActiveMessage msg;
    msg << p1 << p2;
    return msg.showMsgBox(uiMessageID);
}

template<typename T1, typename T2, typename T3>
UINT showMsgBox(int uiMessageID, const T1& p1, const T2& p2, const T3& p3)
{
    TA_Base_Bus::TransActiveMessage msg;
    msg << p1 << p2 << p3;
    return msg.showMsgBox(uiMessageID);
}

template<typename T1, typename T2, typename T3, typename T4>
UINT showMsgBox(int uiMessageID, const T1& p1, const T2& p2, const T3& p3, const T4& p4)
{
    TA_Base_Bus::TransActiveMessage msg;
    msg << p1 << p2 << p3 << p4;
    return msg.showMsgBox(uiMessageID);
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
UINT showMsgBox(int uiMessageID, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5)
{
    TA_Base_Bus::TransActiveMessage msg;
    msg << p1 << p2 << p3 << p4 << p5;
    return msg.showMsgBox(uiMessageID);
}

#endif // !defined(AFX_TRANSACTIVEMESSAGE_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_)
