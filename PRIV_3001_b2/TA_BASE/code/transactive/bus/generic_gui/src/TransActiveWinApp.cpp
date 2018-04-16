/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/TransActiveWinApp.cpp $
* @author:  Karen Graham
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2018/03/23 17:40:42 $
* Last modified by:  $Author: Ouyang $
* 
* This inherits from CWinApp and implements all common functionality for
* TransActive applications.
*/

#include "bus/generic_gui/src/StdAfx.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/IGUIApplication.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include <new.h>
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/MessageResourceException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/ApplicationStatusAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/message/src/ConnectionChecker.h"
#include "core\naming\src\Naming.h"
#include "core\message\src\TransactiveMessageSubscription.h"
#include "core\message\src\MessagePublicationManager.h"
#include "core/database/src/DbServerLocator.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"

#include "bus/generic_gui/resource.h" // TD14164 azenitha	

//#include "core/threads/src/Thread.h"

#include "ace/ACE.h"

#include <winsock2.h>  // Used by getHostName 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericGUIException;
using TA_Base_Core::ProcessAlreadyRunningException;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::MessageResourceException;
using TA_Base_Core::RightsException;
using TA_Base_Core::RunParams;
using namespace TA_Base_Core;
/////////////////////////////////////////////////////////////////////////////
// CEventsGuiApp

namespace TA_Base_Bus
{
	BEGIN_MESSAGE_MAP(TransActiveWinApp, CWinApp)
		//{{AFX_MSG_MAP(TransActiveWinApp)
		//}}AFX_MSG_MAP
		ON_COMMAND(ID_HELP, CWinApp::OnHelp)
		ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
		END_MESSAGE_MAP()
		
		// Static variable initialisation   
		const std::string TransActiveWinApp::APPLICATION_NAME = "ApplicationName";
    const std::string TransActiveWinApp::ERROR_MESSAGE = "ErrorMessage";
    const std::string TransActiveWinApp::CONSOLE_NAME = "ConsoleName";
	
    /////////////////////////////////////////////////////////////////////////////
    // TransActiveWinApp construction
	
    TransActiveWinApp::TransActiveWinApp(IGUIApplication* gui, const CString& applicationName)
		: m_genericGUI(NULL),
		m_gui(gui),
		m_applicationName(applicationName)
    {
        TA_ASSERT(m_gui != NULL, "IGUIApplication passed is NULL");
        // fix for PW2451
        _set_new_handler(TransActiveWinApp::outOfMemoryHandler);
        
		const int hNameLength = 512;
        char hostName[hNameLength] = {0};
		
        WORD wVersionRequested;
        WSADATA wsaData;
        wVersionRequested = MAKEWORD( 2, 0 );
		
        if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
        {
			
			if( gethostname(hostName, hNameLength) != 0 )
            {
				sprintf(hostName,"%s","console name not available");
            }
        }
        else
        {
            sprintf(hostName,"%s","console name not available");
        }
        m_consoleName = hostName;
        
    }
	
	
    TransActiveWinApp::~TransActiveWinApp()
    {
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // TransActiveWinApp initialization
	
    BOOL TransActiveWinApp::InitInstance()
    {

        ACE::init();
		
        AfxEnableControlContainer();
#if (_MSC_VER <= 1200) // MFC 6
#ifdef _AFXDLL
        // Call this when using MFC in a shared DLL
        Enable3dControls();
#else
        // Call this when linking to MFC statically
        Enable3dControlsStatic();
#endif
#endif
		
        bool shouldStartMessagePump = false;
        try
        {
            m_genericGUI = new GenericGUI(*m_gui,m_lpCmdLine);

			std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);

			shouldStartMessagePump = m_gui->displayApplication();
			
			if (! m_genericGUI->init())
			{
				::MessageBox(AfxGetMainWnd()->m_hWnd, "Something wrong in initializing the component. please contact system administrator.", 
					"Transactive", MB_OK|MB_ICONSTOP);
				::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);
				exit(0);
			}
			// See if the application name is already set in RunParams
			if (appName.empty())
			{
				// not yet set, so just set it as the one that was passed in as the
				// argument
				if (!m_applicationName.IsEmpty())
				{
					RunParams::getInstance().set(RPARAM_APPNAME, m_applicationName);
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
						"Setting the RunParam RPARAM_APPNAME as %s", m_applicationName);
				}
			}
			else
			{
				// use the one that is already in RunParams
				m_applicationName = appName.c_str();
			}
			
			//TD14894 remove the lanch/exit event
			//sendAuditMessage();
			
            m_genericGUI->run();
        }
        catch ( const GenericGUIException& ex)
        {
			//td13694 bgn
			shouldStartMessagePump = handleGenericGUIException(ex);
			if(!shouldStartMessagePump)
			{
				ExitProcess(-1);
			}
			//td13694 end
        }
        catch ( const ProcessAlreadyRunningException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "ProcessAlreadyRunningException", "Shutting down quietly as this application is already running and must only be running once.");
        }
        catch ( const MessageResourceException& ex)
        {
			handleMessageResourceException(ex);
        }
        catch ( const TransactiveException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransActiveException", "Because this has been caught at the top level we must now shutdown.");
            if (m_genericGUI != NULL)
            {
                m_genericGUI->fatalErrorOccurred();
            }
            TA_Base_Bus::TransActiveMessage userMsg;
			std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
			userMsg << appName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010010);
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Because this has been caught at the top level we must now shutdown.");
            if (m_genericGUI != NULL)
            {
                m_genericGUI->fatalErrorOccurred();
            }
            TA_Base_Bus::TransActiveMessage userMsg;
			std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
			userMsg << appName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010010);
        }
        return shouldStartMessagePump;
    }

	bool TransActiveWinApp::handleGenericGUIException(const GenericGUIException& ex)
	{
		LOG(SourceInfo, DebugUtil::ExceptionCatch, "GenericGUIException", "Now calculating which error it was so an appropriate message can be displayed to the user");
		bool result = true; //td 13694
		unsigned long errorId;
		TA_Base_Bus::TransActiveMessage userMsg;
		if (ex.getFailType() != GenericGUIException::ACCESS_DENIED)
		{
			userMsg << m_applicationName;
		}
		
		switch( ex.getFailType() )
		{
		case (GenericGUIException::ENTITY_CONFIGURATION_INVALID ):
			errorId = IDS_UE_010011;
			break;
			
		case ( GenericGUIException::ENTITY_DOES_NOT_EXIST ):
		case ( GenericGUIException::ENTITY_NOT_UNIQUE ):
			errorId = IDS_UE_010013;
			break;
			
		case ( GenericGUIException::COMMAND_LINE_INVALID ):
			errorId = IDS_UE_010008;
			break;
			
		case ( GenericGUIException::NO_DATABASE_CONNECTION ):
			errorId = IDS_UE_010001;
			result = false;
			break;
			
		case ( GenericGUIException::COMMUNICATION_ERROR_WITH_CONTROL_STATION ):
			errorId = IDS_UE_010014;
			break;
			
		case ( GenericGUIException::ACCESS_DENIED ):
			{
				errorId = IDS_UE_010021;
				std::string userName;
				std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
				TA_Base_Bus::AuthenticationLibrary authLibrary;
				TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
				std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
				userName = iOperator->getName();
				std::string actionName = "start this application";
				userMsg << userName;
				userMsg << actionName;		  	
				result = false; //td13694
				break;
			}
		case ( GenericGUIException::UNKNOWN_ACCESS_RIGHTS ):
			{
				errorId = IDS_UE_010019;
				result = false; //td13694
				break;
			}
			
		case ( GenericGUIException::NO_ACTIVE_SESSION ): //TD1141
			errorId = IDS_UE_020050;
			break;
			
		case ( GenericGUIException::INITIALISATION_ERRORS ):
		default:
			errorId = IDS_UE_010012;
			std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
			userMsg << appName;
			break;
		}
		userMsg.showMsgBox(errorId);
		return result;  //td13694
	}

	void TransActiveWinApp::handleMessageResourceException(const MessageResourceException& ex)
	{
		// NickJ: 07/12/04: Added catch for error coming from TransActiveMessage box
		// The problem was a message box that failed to display. Need to notify the user what is happening,
		// but WITHOUT using a TransActiveMessage
		
		// First, log the catch
		LOG_EXCEPTION_CATCH(SourceInfo, "MessageResourceException", "Caught exception at top level, so application will have to shutdown.");
		
		// Then the FATAL log
		CString errorMessage;
		switch (ex.getFailType())
		{
		case (MessageResourceException::DLL_NOT_FOUND):
			{
				errorMessage = "The MessageResource DLL could not be found. Message ID: %d";
				break;
			};
		case (MessageResourceException::FORMATTING_ERROR):
			{
				errorMessage = "The message retrieved for ID %d contained invalid formatting details.";
				break;
			};
		case (MessageResourceException::LANGUAGE_ERROR):
			{
				errorMessage = "The message ID %d could not be found within the MessageResource for this systems language context.";
				break;
			};
		case (MessageResourceException::RESOURCE_NOT_FOUND_ERROR):
			{
				errorMessage = "The message ID %d could not be found within the MessageResource DLL.";
				break;
			};
		case (MessageResourceException::GENERAL_ERROR):
			{
				errorMessage = "An unspecified error occured while attempting to display the message with ID %d.";
				break;
			};
		default:
			{
				TA_ASSERT(false,"An invalid MessageResourceException type was found");
			}
		}            
		errorMessage += " Error description reported from library follows:";
		LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, errorMessage, ex.getMessageID());
		LOGMORE(SourceInfo, ex.what());
		LOGMORE(SourceInfo, "As this exception was not caught within the application, GenericGUI must now exit.");
		
		// Finally, raise a message box informing the user of what is happening.
		
		// TD14164 azenitha++
		CString errMsg;
		errMsg.LoadString(IDS_ERROR_MESSAGE);
		
		/*::MessageBox(NULL,"An error occured while attempting to display an application message box.\nThis is an unrecoverable error. Check log file for further details.\nControl Station will now exit.",
		m_applicationName,MB_OK|MB_ICONSTOP);*/
		::MessageBox(NULL, errMsg, m_applicationName,MB_OK|MB_ICONSTOP);
		// TD14164 ++azenitha
	}

	void TransActiveWinApp::sendAuditMessage()
	{
		// Submit an audit message with the name of the console on which this application is running
		
		unsigned long entityKey = m_genericGUI->getEntityKey();
		
		// Need to get the console name of this session (if we can)
		try
		{
			TA_Base_Core::IConsole* theConsole = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(
				TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );
			
			m_consoleName = theConsole->getName().c_str();
			
			delete theConsole;
			theConsole = NULL;
		}
		catch(...)
		{
			// If there are any problems, then just log that we couldn't get the console name and move on
			LOG_EXCEPTION_CATCH(SourceInfo, "General Exception", 
				"An exception was caught while attempting to retrieve the console details for this session");
		}
		
		// Get the audit message sender with which to send the message
		//remove app start event.
	/*	TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
			getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);
		
		TA_Base_Core::DescriptionParameters params;
		TA_Base_Core::NameValuePair param1(APPLICATION_NAME,std::string(m_applicationName));
		TA_Base_Core::NameValuePair param2(CONSOLE_NAME,std::string(m_consoleName));
		params.push_back(&param1);
		params.push_back(&param2);
		
		sender->sendAuditMessage( TA_Base_Core::ApplicationStatusAudit::ApplicationStart,
			entityKey,
			params,
			"N/A",
			TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
			"",
			"",
			"" );
		
		delete sender;
		sender = NULL;*/
	}

    int TransActiveWinApp::ExitInstance() 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ExitInstance");
        
        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().shutdownInProgress();

        unsigned long entityKey = 0;
		
        if (m_genericGUI != NULL)
        {
            entityKey = m_genericGUI->getEntityKey();
			
            if (m_gui != NULL)
            {
                delete m_gui;
                m_gui = NULL;
            }
			
            m_genericGUI->shutdown();
            m_genericGUI->deactivateAndDeleteServant();
            // Don't need to delete m_genericGUI as the call above does this.
        }
		
		//TD14894 remove the lanch/exit event
		/*
        // Submit an audit message with the name of the console on which this application was running
        // To do that, will need a location key, if that is not set, then just log instead of sending the message,
        // because the messaging libraries will assert and die if there is no location key.
		
        if (RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ))
        {
            // Get the audit message sender with which to send the message
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
                getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);
			
            TA_Base_Core::DescriptionParameters params;
            TA_Base_Core::NameValuePair param1(APPLICATION_NAME,std::string(m_applicationName));
            TA_Base_Core::NameValuePair param2(CONSOLE_NAME,std::string(m_consoleName));
            params.push_back(&param1);
            params.push_back(&param2);
			
            sender->sendAuditMessage( TA_Base_Core::ApplicationStatusAudit::ApplicationExit,
				entityKey,
				params,
				"N/A",
				TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
				"",
				"",
				"" );
			
            // Need to sleep to give the event time to submit itself.
            TA_Base_Core::Thread::sleep(100);
			
            delete sender;
            sender = NULL;
        }
        else
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugFatal,
                "Location key not available on applicaiton exit. No shutdown audit message can be sent.");
        }
		*/
		
        TA_Base_Core::Naming::cleanUp();
        // Clean up the Event Consumers
        TA_Base_Core::gDestroySubscriptionManager();
        TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
		TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();
		
        try
        {
			DbServerLocator::removeInstance();
            TA_Base_Core::CorbaUtil::getInstance().cleanup();
        }
        catch( ... )
        {
        }
		
		ACE::fini();
        
        LOG( SourceInfo, DebugUtil::FunctionExit, "ExitInstance");
        return CWinApp::ExitInstance();
    }
	
	
    void TransActiveWinApp::WinHelp(DWORD dwData, UINT nCmd) 
	{
		if (HELP_CONTEXT == nCmd)
		{
			// display help for the specified topic
			HelpLauncher::getInstance().displayContextSpecificHelp(LOWORD(dwData));
		}
		else
		{
			// display the main help contents
			HelpLauncher::getInstance().displayHelp();
		}
	}

	//WinHelp is replaced by WinHelpInternal under VS 2005 and VS 2008
	void TransActiveWinApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
	{
		if (HELP_CONTEXT == nCmd)
		{
			// display help for the specified topic
			HelpLauncher::getInstance().displayContextSpecificHelp(LOWORD(dwData));
		}
		else
		{
			// display the main help contents
			HelpLauncher::getInstance().displayHelp();
		}
	}
	
	
    int TransActiveWinApp::outOfMemoryHandler( size_t size )
    {
        std::ostringstream oss;
        oss << "No more memory available.  Failed to allocated " << size << " bytes."; 
        TA_THROW(std::bad_alloc(oss.str().c_str()));
        // Shouldn't reach here but if it does, this should tell new to stop allocation attempts
        return 0;  
    }
	
} // End namespace TA_Base_Bus
