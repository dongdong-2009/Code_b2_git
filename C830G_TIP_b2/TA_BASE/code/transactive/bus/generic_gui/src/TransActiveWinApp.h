/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This inherits from CWinApp and implements all common functionality for
  * TransActive applications.
  */

#if !defined(AFX_TRANSACTIVEWINAPP_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
#define AFX_TRANSACTIVEWINAPP_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <string>

namespace TA_Base_Core 
{
	class MessageResourceException;
	class GenericGUIException;
};

namespace TA_Base_Bus
{
    class GenericGUI;
    class IGUIApplication;

    class TransActiveWinApp : public CWinApp
    {
    public:
        /**
         * Constructor
         *
         * @param IGUIApplication* - The pointer to the implementation of an IGUIApplication.  This must not be NULL.
         * @param CString - The display name of the application
         */
        TransActiveWinApp(IGUIApplication* gui, const CString& applicationName);


		virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

		virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);

        /**
         * Destructor
         */
        virtual ~TransActiveWinApp();


        /**
         * InitInstance
         *
         * This method must be implemented to ensure that when the application is
         * started it is started correctly and the same for each TransActive application.
         */
        virtual BOOL InitInstance();

        /**
         * ExitInstance
         *
         * This method must be implemented to ensure that when the application is
         * closed it is shutdown correctly and the same for each TransActive application.
         */
        virtual int ExitInstance();


        DECLARE_MESSAGE_MAP()

    protected:
        
        /**
         * getGuiApplication
         *
         * This method is provided so classes that inherit from this class can
         * use the IGUIApplication object.
         *
         * @return IGUIApplication& - A reference to the GUI object so callbacks can
         *                            be performed.
         */
        IGUIApplication& getGuiApplication()
        {
            return *m_gui;
        }


        /** 
          * getConsoleName
          *
          * Returns the console name.  If it managed to connect to the database to get the
          *         name associated with the console, it will return that, otherwise, it
          *         wil just return the name of the console as it is.
          *
          * @return the console name.  
          */
        CString getConsoleName()
        {
            return m_consoleName;
        }

    private:

        TransActiveWinApp( const TransActiveWinApp& theTransActiveWinApp){ };
        TransActiveWinApp& operator=(const TransActiveWinApp &) { }

        /** 
          * outOfMemoryHandler
          *
          * Provides the standard way of handling out of memory problem
          * by throwing a bad_alloc exception
          *
          * @param size_t size 
          *
          * @return Nothing as the bad_alloc exception will be thrown
          * 
          * @throw bad_alloc
          */
        static int outOfMemoryHandler( size_t size );
		
		//TD18095,jianghp
		void sendAuditMessage();
		/**
		return false for the app can't launch otherwise can
		*/
		bool handleGenericGUIException(const TA_Base_Core::GenericGUIException& ex); //td13694
		void handleMessageResourceException(const TA_Base_Core::MessageResourceException& ex);
		//TD18095,jianghp

	private:
        GenericGUI*       m_genericGUI;
        IGUIApplication*  m_gui;
        CString           m_applicationName;
        CString           m_consoleName;

	public:
        static const std::string APPLICATION_NAME;
        static const std::string ERROR_MESSAGE;
        static const std::string CONSOLE_NAME;

    };
} // End namespace TA_Base_Bus

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIVEWINAPP_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
