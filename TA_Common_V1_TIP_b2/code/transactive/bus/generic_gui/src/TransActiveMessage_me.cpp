// TransActiveMessage.cpp: implementation of the TransActiveMessage class.
//
//////////////////////////////////////////////////////////////////////
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/src/TransActiveMessage.cpp $
  * @author:  Brett Zimmerman
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Standard message box format, display and logging functions
  */
#include "bus/generic_gui/src/stdafx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/mfc_extensions/src/LoadString.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/MessageResourceException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/UserInteractionAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <boost/foreach.hpp>
#include <omnithread.h>
#include <ace/TSS_T.h>

#ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new DEBUG_NEW
#endif

// WM_HELPPROMPTADDR is used internally by MFC to get the address of
//	m_dwPromptContext from the associated frame window. This is used
//	during message boxes to setup for F1 help while that msg box is
//	displayed. lResult is the address of m_dwPromptContext.
#ifndef WM_HELPPROMPTADDR
    #define WM_HELPPROMPTADDR	0x0376
#endif

// Context sensitive help support from afxpriv.h
#ifndef HID_BASE_PROMPT
    #define HID_BASE_PROMPT     0x00030000UL
#endif

using TA_Base_Core::DebugUtil;


namespace
{
    class HookWrap
    {
    public:

        HookWrap()
            : m_hhook(NULL),
              m_depth(0)
        {
        }

        static void beforeMessageBox()
        {
            tss()->lock();
            startHook();
        }

        static void afterMessageBox()
        {
            stopHook();
            tss()->unlock();

            if (0 == tss()->depth())
            {
                removeMessageBoxHwnd();
            }
        }

        static void stopHook()
        {
            HHOOK hhook = tss()->getHhook();

            if (hhook != NULL)
            {
                UnhookWindowsHookEx(hhook);
                tss()->setHhook(NULL);
                LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::stopHook - UnhookWindowsHookEx %p", hhook);
            }
        }

        static void startHook()
        {
            tss()->setHhook(SetWindowsHookEx(WH_CBT, &TA_Base_Bus::TransActiveMessage::CBTProc, 0, GetCurrentThreadId()));
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::beforeMessageBox - SetWindowsHookEx %p", tss()->getHhook());
        }

        static void addMessageBoxHwnd(const HWND hwnd)
        {
            g_mutex().lock();
            TA_Base_Bus::TransActiveMessage::s_popupHWnd.insert(tss()->addHwnd(hwnd));
            g_mutex().unlock();
        }

        static void removeMessageBoxHwnd()
        {
            g_mutex().lock();
            BOOST_FOREACH(HWND hwnd, tss()->hwnds())
            {
                TA_Base_Bus::TransActiveMessage::s_popupHWnd.erase(hwnd);
            }
            g_mutex().unlock();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace,
                "HookWrap::removeMessageBoxHwnd - removed %d, remains %d", tss()->hwnds().size(), TA_Base_Bus::TransActiveMessage::s_popupHWnd.size());
            tss()->removeHwnds();
        }

    private:

        void setHhook(HHOOK hhook) { m_hhook = hhook; }
        HHOOK& getHhook() { return m_hhook; }
        omni_mutex* mutex() { return &m_mutex; };
        size_t depth() { return m_depth; }
        std::set<HWND>& hwnds() { return m_hwnds; }

        void lock()
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::lock - acqriring lock %p (depth=%d)", &m_mutex, m_depth);
            m_mutex.lock();
            m_depth++;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::lock - acqrired lock %p (depth=%d)", &m_mutex, m_depth);
        }

        void unlock()
        {
            m_depth--;
            m_mutex.unlock();
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::unlock - released lock %p (depth=%d)", &m_mutex, m_depth);
        }

        HWND addHwnd(HWND hwnd)
        {
            m_hwnds.insert(hwnd);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::addHwnd - add MessageBox hwnd %p, size=%lu", hwnd, m_hwnds.size());
            return hwnd;
        }

        void removeHwnds()
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "HookWrap::removeHwnd - remove all MessageBox hwnds, size=%lu", m_hwnds.size());
            m_hwnds.clear();
        }

        static ACE_TSS<HookWrap>& tss()
        {
            static ACE_TSS<HookWrap>* t = new ACE_TSS<HookWrap>;
            return *t;
        }

        static omni_mutex& g_mutex()
        {
            static omni_mutex* m = new omni_mutex;
            return *m;
        }

    private:

        HHOOK m_hhook;
        omni_mutex m_mutex;
        std::set<HWND> m_hwnds;
        size_t m_depth;
    };

    ACE_TSS<HookWrap>* g_hhookWrapTss = NULL;
}


namespace TA_Base_Bus
{
#ifdef _DEBUG
    static LPCTSTR DEFAULT_RES_FILE_NAME = _T("TA_MessageResource_d.dll"); //SL
#else
    static LPCTSTR DEFAULT_RES_FILE_NAME = _T("TA_MessageResource.dll"); //SL
#endif

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////
    HHOOK TransActiveMessage::s_hhk = NULL;
    TA_Base_Core::ReEntrantThreadLockable TransActiveMessage::s_hhkLock;

    std::set<HWND> TransActiveMessage::s_popupHWnd;

    TransActiveMessage::TransActiveMessage()
        : m_hResourceDll(NULL),
          m_lcidCurrentLang(GetUserDefaultLCID()),
          m_sResourceFilePath(DEFAULT_RES_FILE_NAME),
          m_bFreeLibrary(false)
    {
        m_hResourceDll = ::AfxLoadLibrary(m_sResourceFilePath);

        if (!m_hResourceDll)
        {
            windowsError(::GetLastError());
        }
        else
        {
            m_bFreeLibrary = true;
        }
    }

    /*TransActiveMessage::TransActiveMessage(HINSTANCE hResourceDll, LCID lcidLang)
    :	m_hResourceDll(hResourceDll),
    	m_lcidCurrentLang(lcidLang),
    	m_bFreeLibrary(false)
    {
    	m_sResourceFilePath.Empty();
    }*/

    TransActiveMessage::~TransActiveMessage()
    {
        if (m_bFreeLibrary && m_hResourceDll)
        {
            if (!::AfxFreeLibrary(m_hResourceDll))
            {
                windowsError(GetLastError());
            }
            else
            {
                m_hResourceDll = NULL;
                m_bFreeLibrary = false;
            }
        }
    }

    UINT TransActiveMessage::showMsgBox(UINT uiMessageID, const CString& caption)
    {
        ASSERT(uiMessageID > 0);

        CString sCaption, sMessage, sLoadedMessage;
        UINT uiMsgBoxType = 0, uiHelpID = 0;
        bool bAudit = false;

        if (NULL == m_hResourceDll)
        {
            // First things first, if the resource DLL is NULL, nothing is going to work.
            // Note that in this instance, there are problem a number of "Windows Error 126" messages in the log file.

            CString errorMessage;
            errorMessage.Format(_T("The Message Resource DLL has not been loaded. The DLL filename should be: %s"),
                                m_sResourceFilePath);
            TA_THROW(TA_Base_Core::MessageResourceException(_T("The Message Resource DLL has not been loaded."),
                     TA_Base_Core::MessageResourceException::DLL_NOT_FOUND, uiMessageID));
        }

        if (getRawMessage(uiMessageID, sLoadedMessage))
        {
            if (parseRawMessage(sLoadedMessage, uiMsgBoxType, uiHelpID, bAudit, sMessage))
            {
                if (m_args.size() > 0)
                {
                    constructMessage(sMessage);
                }

                reformatCurlyBraces(sMessage);

                if (caption == "")
                {
                    formatCaption(sCaption, uiMessageID);
                }
                else
                {
                    sCaption = caption;
                }

                return doMsgBox(sCaption, sMessage, uiMsgBoxType, uiHelpID, bAudit);
            }
            else
            {
                // NickJ: 06/12/04: As a fix for TD8327, the TransActiveMessage class now has its own exception type
                TA_THROW(TA_Base_Core::MessageResourceException("Could not parse message string",
                         TA_Base_Core::MessageResourceException::FORMATTING_ERROR, uiMessageID));
            }
        }
        else
        {
            // NickJ: 06/12/04: As a fix for TD8327, the TransActiveMessage class now has its own exception type.
            TA_THROW(TA_Base_Core::MessageResourceException("Could not load message string",
                     TA_Base_Core::MessageResourceException::GENERAL_ERROR, uiMessageID));
        }
    }

    UINT TransActiveMessage::doMsgBox(LPCTSTR pszCaption, LPCTSTR pszMessage, UINT uiMsgBoxType, UINT uiHelpID, BOOL bAuditEvent)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Popup MessageBox - [%s] [%s]", pszCaption, pszMessage);

        DWORD dwPrevContext = 0;
        DWORD* pdwContext = NULL;

        if (uiHelpID > 0)
        {
            setHelpContext(uiHelpID, pdwContext, dwPrevContext);
        }

        // TES333: Make sure the message box gets displayed in front of GUI's / schematics.
        uiMsgBoxType |= MB_SETFOREGROUND | MB_TOPMOST;

        // TD392 - Fix for message box so that the message box is always centred on the app rather than on the desktop
        //s_hhkLock.acquire();
        //s_hhk = SetWindowsHookEx(WH_CBT, &TransActiveMessage::CBTProc, 0, GetCurrentThreadId());
        HookWrap::beforeMessageBox();

        UINT uiResult = ::MessageBox(determineParentWindow(), pszMessage, pszCaption, uiMsgBoxType);

        HookWrap::afterMessageBox();

        if (pdwContext)
        {
            resetHelpContext(pdwContext, dwPrevContext);
        }

        logMessage(pszCaption, pszMessage, uiResult, bAuditEvent);

        return uiResult;
    }

    bool TransActiveMessage::setHelpContext(UINT uiHelp, DWORD* pdwContext, DWORD& dwPrevContext)
    {
        // Call ResetHelpContext after message box has been displayed
        ASSERT(uiHelp > 0);

        CWnd* pMainWnd = AfxGetMainWnd();
        HWND hWnd = (pMainWnd) ? pMainWnd->GetSafeHwnd() : NULL;

        // set help context if possible
        if (hWnd)
        {
            // use app-level context or frame level context
            LRESULT lResult = ::SendMessage(hWnd, WM_HELPPROMPTADDR, 0, 0);

            if (lResult)
            {
                pdwContext = reinterpret_cast<DWORD*>(lResult);
            }
        }

        if (pdwContext)
        {
            // save old prompt context for restoration later
            dwPrevContext = *pdwContext;

            if (uiHelp)
            {
                *pdwContext = HID_BASE_PROMPT + uiHelp;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    void TransActiveMessage::resetHelpContext(DWORD* pdwContext, DWORD dwPrevContext)
    {
        // restore prompt context if possible
        if (pdwContext)
        {
            *pdwContext = dwPrevContext;
        }
    }

    bool TransActiveMessage::logMessage(LPCTSTR pszCaption, LPCTSTR pszMessage, UINT uiResult, BOOL bAuditEvent)
    {
        CString sResult, sMessage;

        switch (uiResult)
        {
        case IDABORT:
            sResult = _T("Abort");
            break; //SL

        case IDCANCEL:
            sResult = _T("Cancel");
            break; //SL

        case IDIGNORE:
            sResult = _T("Ignore");
            break; //SL

        case IDNO:
            sResult = _T("No");
            break; //SL

        case IDOK:
            sResult = _T("OK");
            break; //SL

        case IDRETRY:
            sResult = _T("Retry");
            break; //SL

        case IDYES:
            sResult = _T("Yes");
            break; //SL
#if (_MSC_VER > 1200)

        case IDTRYAGAIN:
            sResult = _T("Try Again");
            break; //SL

        case IDCONTINUE:
            sResult = _T("Continue");
            break; //SL
#endif

        default:
            char buff[100];
            sResult = _itoa(uiResult, buff, 10);
        }

        if (bAuditEvent)
        {
            sendAuditMessage(pszMessage, pszCaption, sResult);
        }

        sMessage.Format(_T("Popup MessageBox - [%s] [%s] [Result: %s]"), pszCaption, pszMessage, sResult); //SL

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", sMessage);

        return true;
    }

    CString TransActiveMessage::constructMessage(UINT uiMessageID)
    {
        ASSERT(uiMessageID > 0);

        CString sLoadedMessage, sMessage;
        UINT uiMsgBoxType = 0, uiHelpID = 0;
        bool bAudit = false;

        if (getRawMessage(uiMessageID, sLoadedMessage) &&
                parseRawMessage(sLoadedMessage, uiMsgBoxType, uiHelpID, bAudit, sMessage))
        {
            constructMessage(sMessage);
        }

        return sMessage;
    }

    void TransActiveMessage::windowsError(DWORD dwError)
    {
        LPVOID lpMsgBuf = NULL;

        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL, dwError, m_lcidCurrentLang, reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, static_cast<LPCTSTR>(lpMsgBuf));
            ::LocalFree(lpMsgBuf);
        }
        else
        {
            CString sOutput;
            sOutput.Format(_T("TransActiveMessage Windows Error - %d\n"), dwError); //SL
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sOutput);
        }
    }

    bool TransActiveMessage::constructMessage(CString& sRawMessage)
    {
        int iArgCount = m_args.size();
        int iStart = 0, iFinish = 0, iIndex = 0, iParamIndex = 0, iRightBraceCount = 0;
        CString sFormat, sParamIndex, sSubString, sSubstitution, sError;
        bool bFindRightBrace = true;

        if (iArgCount > 0)
        {
            iStart = sRawMessage.Find(_T("{"), iStart); //SL

            if (iStart < 0)
            {
                sError.Format(_T("Warning - Expecting message to contain place markers for %d arguments - %s"), m_args.size(), sRawMessage); //SL
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, sError);
                return true;
            }

            while (iStart >= 0)
            {
                // If not at the end of the string check for overriding character
                if (iStart < (sRawMessage.GetLength() - 1) && sRawMessage[iStart + 1] == '{') //SL
                {
                    iStart += 2;

                    if (iStart < sRawMessage.GetLength())
                    {
                        iStart = sRawMessage.Find(_T("{"), iStart); //SL
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }

                iFinish = iStart + 1;
                iRightBraceCount = 0;
                bFindRightBrace = true;

                while (bFindRightBrace)
                {
                    iFinish = sRawMessage.Find(_T("}"), iFinish); //SL

                    if (iFinish < 0)
                    {
                        sError.Format(_T("Error - Malformed String (Missing closing bracket) - %s"), sRawMessage); //SL
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
                        return false;
                    }

                    // Check for overriding character
                    if (iFinish < (sRawMessage.GetLength() - 1) && sRawMessage[iFinish + 1] == '}') //SL
                    {
                        iRightBraceCount++;
                        iFinish++;
                    }
                    else
                    {
                        bFindRightBrace = false; // stop loop
                    }
                }

                // odd number of right braces (iRightBraceCount is one less than actual count)
                if (iRightBraceCount % 2 == 0)
                {
                    iFinish -= iRightBraceCount;
                }

                // lose the {} characters
                sSubString = sRawMessage.Mid(iStart + 1, iFinish - iStart - 1);
                sFormat.Empty();
                int iComma = 0;
                iComma = sSubString.Find(_T(","), iComma); //SL

                if (iComma > 0)
                {
                    sFormat = sSubString.Right(sSubString.GetLength() - iComma);
                    sParamIndex = sSubString.Left(iComma);
                }
                else
                {
                    sParamIndex = sSubString;
                }

                iParamIndex = _ttoi(sParamIndex);

                if (iParamIndex < 0)
                {
                    sError.Format(_T("Malformed String (Param Index Error %s) - s"), sParamIndex, sRawMessage); //SL
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
                    return false;
                }

                if (iParamIndex >= iArgCount)
                {
                    sError.Format(_T("Malformed String (Param Index not passed in for %s) - s"), sParamIndex, sRawMessage);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
                    return false;
                }

                // Perform Substitution
                sRawMessage.Delete(iStart, iFinish - iStart + 1);

                if (sFormat.IsEmpty())
                {
                    sSubstitution = m_args[iParamIndex]->getFormattedValue();
                }
                else
                {
                    sSubstitution = m_args[iParamIndex]->getFormattedValue(sFormat);
                }

                // Perform Substitution
                sRawMessage.Insert(iStart, sSubstitution);

                // adjust for change in length
                iStart = iStart + sSubstitution.GetLength();

                iStart = sRawMessage.Find(_T("{"), iStart); //SL
            }
        }

        return true;
    }

    void TransActiveMessage::reformatCurlyBraces(CString& sMessage)
    {
        sMessage.Replace(_T("{{"), _T("{")); //SL
        sMessage.Replace(_T("}}"), _T("}")); //SL
    }

    bool TransActiveMessage::getRawMessage(UINT uiMessageID, CString& sMessage)
    {
        try
        {
#if (_MSC_VER <= 1200) // MFC 6
            return (VC6LoadString(sMessage, m_hResourceDll, uiMessageID, static_cast<unsigned short>(GetUserDefaultLCID)) == TRUE);
#else // MFC 7 already has inbuilt function
            // we'd better do not use system language as load string parameter as we only have one version of MessageResource dll
            // 		return (TRUE == sMessage.LoadString(m_hResourceDll, uiMessageID, (WORD)m_lcidCurrentLang));
            return (TRUE == sMessage.LoadString(m_hResourceDll, uiMessageID));
#endif
        }
        catch (CMemoryException* e)
        {
            // Create the error message
            CString errorMessage = _T("Exception caught while attempting to load message box string for message ID %d. Exception error reason:");

            // and extract the message from the exception
            CString exceptionMessage;
            int size = 2048;
            e->GetErrorMessage(exceptionMessage.GetBuffer(size), size);
            e->Delete();

            // Throw a MessageResourceException instead
            TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
                     TA_Base_Core::MessageResourceException::GENERAL_ERROR, uiMessageID));
        }
    }

    bool TransActiveMessage::parseRawMessage(CString& sRawMessage, UINT& uiMsgBoxType, UINT& uiHelpID, bool& bAudit, CString& sMessage)
    {
        TCHAR seps[] = _T("|"); //SL
        TCHAR* token = NULL;

        token = _tcstok(sRawMessage.GetBuffer(0), seps);

        if (!token)
        {
            CString errorMessage;
            errorMessage.Format("Message string does not include any data delimiters. String is\n%s", sRawMessage);
            sRawMessage.ReleaseBuffer();
            TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
                     TA_Base_Core::MessageResourceException::FORMATTING_ERROR));
        }

        uiMsgBoxType = _ttoi(token);

        token = _tcstok(NULL, seps);

        if (!token)
        {
            CString errorMessage;
            errorMessage.Format("Message includes only one data delimiter. String is\n%s", sRawMessage);
            sRawMessage.ReleaseBuffer();
            TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
                     TA_Base_Core::MessageResourceException::FORMATTING_ERROR));
        }

        uiHelpID = _ttoi(token);

        token = _tcstok(NULL, seps);

        if (!token)
        {
            CString errorMessage;
            errorMessage.Format("Message includes only two data delimiters. String is\n%s", sRawMessage);
            sRawMessage.ReleaseBuffer();
            TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
                     TA_Base_Core::MessageResourceException::FORMATTING_ERROR));
        }

        bAudit = (1 == _ttoi(token) ? true : false);

        token = _tcstok(NULL, seps);

        if (!token)
        {
            CString errorMessage;
            errorMessage.Format("Message includes only three data delimiters. String is\n%s", sRawMessage);
            sRawMessage.ReleaseBuffer();
            TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
                     TA_Base_Core::MessageResourceException::FORMATTING_ERROR));
        }


        sMessage = token;

        sRawMessage.ReleaseBuffer();

        return true;
    }

    void TransActiveMessage::formatCaption(CString& sCaption, UINT uiMessageID)
    {
        CWinApp* pApp = AfxGetApp();

        if (pApp)
        {
            //TD19144 remove uiMessageID from MessageBox caption
            // sCaption.Format(_T("%s  -  [%d]"), pApp->m_pszAppName, uiMessageID); //SL
            sCaption.Format(_T("%s"), pApp->m_pszAppName, uiMessageID); //SL
        }
        else
        {
            //TD19144
            //sCaption.Format(_T("[%d]"), uiMessageID); //SL
        }
    }

    bool TransActiveMessage::sendAuditMessage(LPCTSTR pszMessage, LPCTSTR pszCaption, LPCTSTR pszResult)
    {
        FUNCTION_ENTRY("SendAuditMessage"); //SL

        // Get our session - needed for rights check
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        if (sessionId.empty())
        {
            return false;
        }

        TA_Base_Core::AuditMessageSender* auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
                    TA_Base_Core::UserInteractionAudit::Context);

        // May have been called before the application has initialised the necessary objects
        if (auditMessageSender == NULL)
        {
            return false;
        }

        TA_Base_Core::DescriptionParameters desc;
        TA_Base_Core::NameValuePair param1("MsgBoxCaption", pszMessage);
        TA_Base_Core::NameValuePair param2("MsgBoxContents", pszCaption);
        TA_Base_Core::NameValuePair param3("MsgBoxResult", pszResult);
        desc.push_back(&param1);
        desc.push_back(&param2);
        desc.push_back(&param3);

        unsigned long ulEntityKey = 0;

        try
        {
            std::auto_ptr< TA_Base_Core::IEntityData > data(
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME)));

            if (data.get() == 0)
            {
                return false;
            }

            ulEntityKey = data->getKey();
        }
        catch (TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            return false;
        }
        catch (TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            return false;
        }

        try
        {
            auditMessageSender->sendAuditMessage(TA_Base_Core::UserInteractionAudit::MsgBoxInteractionAudit,
                                                 ulEntityKey, desc, "", sessionId, "", "", "", NULL);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
            return false;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught an unknown exception sending audit message.");
            return false;
        }

        FUNCTION_EXIT;

        return true;
    }


    LRESULT CALLBACK TransActiveMessage::CBTProc(INT nCode, WPARAM wParam, LPARAM lParam)
    {
        // copied from http://www.codeguru.com/Cpp/W-P/win32/messagebox/article.php/c4541/

        HWND  hParentWnd, hChildWnd;    // msgbox is "child"
        RECT  rParent, rChild, rDesktop;
        POINT pCenter, pStart;
        INT   nWidth, nHeight;

        // notification that a window is about to be activated
        // window handle is wParam
        if (nCode == HCBT_ACTIVATE)
        {
            // set window handles
            hParentWnd = determineParentWindow();

            hChildWnd  = (HWND)wParam;

            if ((hParentWnd != 0) &&
                    (hChildWnd != 0) &&
                    (GetWindowRect(GetDesktopWindow(), &rDesktop) != 0) &&
                    (GetWindowRect(hParentWnd, &rParent) != 0) &&
                    (GetWindowRect(hChildWnd, &rChild) != 0))
            {
                int monitorCount = GetSystemMetrics(SM_CMONITORS);

                // calculate message box dimensions
                nWidth  = (rChild.right - rChild.left);
                nHeight = (rChild.bottom - rChild.top);

                // calculate parent window center point
                pCenter.x = rParent.left + ((rParent.right - rParent.left) / 2);
                pCenter.y = rParent.top + ((rParent.bottom - rParent.top) / 2);

                // calculate message box starting point
                pStart.x = (pCenter.x - (nWidth / 2));
                pStart.y = (pCenter.y - (nHeight / 2));

                // adjust if message box is off desktop
                if (pStart.x < 0)
                {
                    pStart.x = 0;
                }

                if (pStart.y < 0)
                {
                    pStart.y = 0;
                }

                if (pStart.x + nWidth > rDesktop.right * monitorCount)
                {
                    pStart.x = rDesktop.right * monitorCount - nWidth;
                }

                if (pStart.y + nHeight > rDesktop.bottom)
                {
                    pStart.y = rDesktop.bottom - nHeight;
                }

                if (1 < monitorCount)
                {
                    int leftInScreen = pStart.x / rDesktop.right;
                    int rightInScreen = (pStart.x + nWidth) / rDesktop.right;

                    if (leftInScreen < rightInScreen)
                    {
                        int centerInScreen = (pStart.x + nWidth / 2) / rDesktop.right;

                        if (centerInScreen == leftInScreen)
                        {
                            pStart.x = (rightInScreen * rDesktop.right) - nWidth;
                        }
                        else
                        {
                            pStart.x = (rightInScreen * rDesktop.right);
                        }
                    }
                }

                // move message box
                MoveWindow(hChildWnd, pStart.x, pStart.y, nWidth, nHeight, FALSE);
            }

            //s_popupHWnd.push_back(hChildWnd);

            // exit CBT hook
            //UnhookWindowsHookEx(s_hhk);
            //s_hhkLock.release();
            HookWrap::addMessageBoxHwnd(hChildWnd);
            HookWrap::stopHook();
        }
        // otherwise, continue with any possible chained hooks
        else
        {
            CallNextHookEx(s_hhk, nCode, wParam, lParam);
        }

        return 0;
    }


    void TransActiveMessage::closeAllMessages()
    {
        TA_Base_Core::ThreadGuard guard(s_hhkLock);

        BOOST_FOREACH(HWND hwnd, s_popupHWnd)
        {
            if (::IsWindow(hwnd))
            {
                ::PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }

        s_popupHWnd.clear();
    }


    HWND TransActiveMessage::determineParentWindow()
    {
        HWND hParentWnd = NULL;

        // Get the active window first.
        hParentWnd = ::GetActiveWindow();

        if (NULL == hParentWnd)
        {
            // Couldn't get forground window so get main window.
            CWnd* pMainWnd = AfxGetMainWnd();

            if (NULL != pMainWnd)
            {
                hParentWnd = pMainWnd->m_hWnd;
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Using main window as parent.");
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Using active window as parent.");
        }

        // Couldn't get main window or window is minimised so get desktop window.
        if ((hParentWnd == NULL) || ::IsIconic(hParentWnd))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Can't get the parent window or window is minimised, so using the desktop window");
            hParentWnd = ::GetDesktopWindow();
        }

        return hParentWnd;
    }
} // End namespace TA_Base_Bus
