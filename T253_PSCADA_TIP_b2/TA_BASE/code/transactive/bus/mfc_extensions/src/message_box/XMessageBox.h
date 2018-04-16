/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/mfc_extensions/src/message_box/XMessageBox.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/11 17:54:29 $
  * Last modified by:  $Author: Ouyang $
  *
  */
// XMessageBox.h
//
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

#define MB_DEFBUTTON5		0x00000400L
#define MB_DEFBUTTON6		0x00000500L

#define MB_CONTINUEABORT	0x00000008L     // adds two buttons, "Continue"  and "Abort"
#define MB_DONOTASKAGAIN	0x01000000L     // add checkbox "Do not ask me again"
#define MB_DONOTTELLAGAIN	0x02000000L     // add checkbox "Do not tell me again"
#define MB_YESTOALL			0x04000000L     // must be used with either MB_YESNO or MB_YESNOCANCEL
#define MB_NOTOALL			0x08000000L     // must be used with either MB_YESNO or MB_YESNOCANCEL
#define MB_NORESOURCE		0x10000000L		// do not try to load button strings from resources
#define MB_NOSOUND			0x80000000L     // do not play sound when mb is displayed

//#define IDCONTINUE		10 // defined already
#define IDYESTOALL			11
#define IDNOTOALL			12

#include "afxmt.h"

AFX_EXT_CLASS int XMessageBox(HWND hwnd, 
                              LPCTSTR lpszMessage,
                              LPCTSTR lpszCaption = NULL, 
                              UINT uStyle = MB_OK|MB_ICONEXCLAMATION,
                              UINT uHelpId = 0);

struct AFX_EXT_CLASS XMessageBoxProxy
{
    struct LockGuard
    {
        LockGuard( CMutex& mutex ) : m_mutex( mutex ) { m_mutex.Lock(); }
        ~LockGuard() { m_mutex.Unlock(); }
        CMutex& m_mutex;
    };

    XMessageBoxProxy()
        : m_hwnd( NULL )
    {
    }

    void setHwnd( const HWND hwnd )
    {
        LockGuard lock(m_mutex);
        m_hwnd = hwnd;
        m_event.SetEvent();
    }

    DWORD waitPopup( const DWORD dwMilliseconds )
    {
        DWORD wait = ::WaitForSingleObject( m_event, dwMilliseconds );

        if ( WAIT_OBJECT_0 == wait )
        {
            m_event.ResetEvent();
        }

        return wait;
    }

    bool isClosed()
    {
        LockGuard lock(m_mutex);
        return ( NULL == m_hwnd );
    }

    void resetHwnd()
    {
        LockGuard lock(m_mutex);
        m_hwnd = NULL;
        m_event.ResetEvent();
    }

    void beforeXMessageBox()
    {
        g_mutex.Lock(); // XMessageBox will unlock it internally
        g_hwnd = this;
        g_thread_id = ::GetCurrentThreadId();
    }

    void afterXMessageBox()
    {
        resetHwnd();
    }

    int XMessageBox( HWND hwnd, LPCTSTR lpszMessage, LPCTSTR lpszCaption = NULL, UINT uStyle = MB_OK|MB_ICONEXCLAMATION, UINT uHelpId = 0 )
    {
        beforeXMessageBox();
        int result = ::XMessageBox( hwnd, lpszMessage, lpszCaption, uStyle, uHelpId );
        afterXMessageBox();

        return result;
    }

    void close()
    {
        LockGuard lock(m_mutex);

        if ( m_hwnd != NULL )
        {
            ::EndDialog( m_hwnd, IDCLOSE );
            m_hwnd = NULL;
            m_event.ResetEvent();
        }
    }

    // call by XMessageBox
    static void insideXMessageBox( HWND hwnd )
    {
        DWORD thread_id = ::GetCurrentThreadId();

        if ( g_hwnd != NULL && thread_id == g_thread_id )
        {
            g_hwnd->setHwnd( hwnd );
            g_hwnd = NULL;
            g_thread_id = 0;
            g_mutex.Unlock();
        }
    }

    HWND   m_hwnd;
    CMutex m_mutex;
    CEvent m_event;
    static XMessageBoxProxy* g_hwnd;
    static CMutex            g_mutex;
    static DWORD             g_thread_id;
};


#endif //XMESSAGEBOX_H
