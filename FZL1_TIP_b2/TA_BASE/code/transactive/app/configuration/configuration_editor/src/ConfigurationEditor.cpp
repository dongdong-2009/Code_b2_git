/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the application starting point. This is the first class that is used (as far as
  * we are concerned). This sets up GenericGUI etc. This does not inherit from
  * TransActiveWinApp as it must handle exceptions differently.
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditor.h"
#include "app/configuration/configuration_editor/src/MainFrm.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorDoc.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorView.h"
#include "app/configuration/configuration_editor/src/SessionHandler.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/generic_gui/src/IGUIApplication.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/ApplicationStatusAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include <AFXPRIV.H>
#include "ace/ACE.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::GenericGUIException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::TransActiveWinApp;
using TA_Base_Core::OnlineUpdateListener;

namespace TA_Base_App
{
    //TD18355 marc_bugfix start
    const std::string CConfigurationEditorApp::CONFIGURATIONEDITOR_APPNAME = "ConfigurationEditor" ;
    const std::string CConfigurationEditorApp::CONFIGCONNECTIONFILE_NAME = "ConfigConnectionStrings.csv";
    //TD18355 marc_bugfix end
    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorApp

    BEGIN_MESSAGE_MAP(CConfigurationEditorApp, TransActiveWinApp)
        //{{AFX_MSG_MAP(CConfigurationEditorApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG_MAP
        // Standard file based document commands
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorApp construction

    CConfigurationEditorApp::CConfigurationEditorApp()
        : m_genericGUI(NULL),
          m_sessionHandler(NULL),
          m_applicationName("配置管理器"),
          TransActiveWinApp(&m_gui, "配置管理器"),
          m_isAlreadyRunning(false)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only CConfigurationEditorApp object

    CConfigurationEditorApp theApp;

    /////////////////////////////////////////////////////////////////////////////
    // CConfigurationEditorApp initialization

    BOOL CConfigurationEditorApp::InitInstance()
    {
        FUNCTION_ENTRY("InitInstance");

        ACE::init();

        AfxEnableControlContainer();

        SetRegistryKey(_T("TransActive Configuration Editor"));

        LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

        bool shouldStartMessagePump = false;

        try
        {
            m_genericGUI = new TA_Base_Bus::GenericGUI(m_gui, m_lpCmdLine);

            setDBConnectionString();

            if (TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).empty())
            {
                TA_THROW(GenericGUIException(GenericGUIException::NO_ACTIVE_SESSION));
            }

            if (!RightsChecker::getInstance().canPerformAction(TA_Base_Bus::aca_RUN_APPLICATION))
            {
                showMsgBox(IDS_UE_010021, getOperatorNameFromSession(), "运行配置管理器");
                TA_THROW(GenericGUIException(GenericGUIException::NO_ACTIVE_SESSION));
            }

            shouldStartMessagePump = m_gui.displayApplication();

            if (! m_genericGUI->init())
            {
                ::MessageBox(AfxGetMainWnd()->m_hWnd, "初始化失败，请联系管理员。",
                             "Transactive", MB_OK | MB_ICONSTOP);
                ::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);
                exit(0);
            }

            m_consoleName = retrieveConsoleName();
            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);

            // See if the application name is already set in RunParams
            if (appName.empty())
            {
                // not yet set, so just set it as the one that was passed in as the
                // argument
                RunParams::getInstance().set(RPARAM_APPNAME, m_applicationName);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Setting the RunParam RPARAM_PROGNAME as %s", m_applicationName);
            }
            else
            {
                // use the one that is already in RunParams
                m_applicationName = appName.c_str();
            }

            //submitAppStartedAuditMessage();
            m_genericGUI->run();
        }
        catch (const GenericGUIException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "GenericGUIException", "Now calculating which error it was so an appropriate message can be displayed to the user");

            switch (ex.getFailType())
            {
            case (GenericGUIException::COMMUNICATION_ERROR_WITH_CONTROL_STATION):
            case (GenericGUIException::NO_ACTIVE_SESSION):
                // These errors are ok because we will try other means
                shouldStartMessagePump = runWithoutControlStation();
                FUNCTION_EXIT
                return shouldStartMessagePump;

            case (GenericGUIException::ENTITY_CONFIGURATION_INVALID):
                showMsgBox(IDS_UE_010011);
                break;

            case (GenericGUIException::ENTITY_DOES_NOT_EXIST):
            case (GenericGUIException::ENTITY_NOT_UNIQUE):
                showMsgBox(IDS_UE_010013);
                break;

            case (GenericGUIException::COMMAND_LINE_INVALID):
                showMsgBox(IDS_UE_010008);
                break;

            case (GenericGUIException::NO_DATABASE_CONNECTION):
                showMsgBox(IDS_UE_010001);
                break;

            case (GenericGUIException::ACCESS_DENIED):
                showMsgBox(IDS_UE_010021, getOperatorNameFromSession(), "运行配置管理器");
                break;

            case (GenericGUIException::UNKNOWN_ACCESS_RIGHTS):
                showMsgBox(IDS_UE_010019);
                break;

            case (GenericGUIException::INITIALISATION_ERRORS):
            default:
                std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
                showMsgBox(IDS_UE_010012, appName);
                break;
            }

            //TD18414 marc_bugfix
            //Due to modification for TD18095, behavior has changed
            //If exception occurs, always force this variable to be false
            //to make CE unload completely
            shouldStartMessagePump = false;
        }
        catch (const TA_Base_Core::ProcessAlreadyRunningException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "ProcessAlreadyRunningException", "Shutting down quietly as this application is already running and must only be running once.");
            m_isAlreadyRunning = true;
            //TD18414 marc_bugfix
            //Due to modification for TD18095, behavior has changed
            //If exception occurs, always force this variable to be false
            //to make CE unload completely
            shouldStartMessagePump = false;
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Because this has been caught at the top level we must now shutdown.");

            if (m_genericGUI != NULL)
            {
                m_genericGUI->fatalErrorOccurred();
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
            userMsg << appName;
            userMsg.showMsgBox(IDS_UE_010010);
            //TD18414 marc_bugfix
            //Due to modification for TD18095, behavior has changed
            //If exception occurs, always force this variable to be false
            //to make CE unload completely
            shouldStartMessagePump = false;
        }

        FUNCTION_EXIT;
        return shouldStartMessagePump;
    }

    int CConfigurationEditorApp::ExitInstance()
    {
        FUNCTION_ENTRY("ExitInstance");

        unsigned long entityKey = 0;

        if (m_genericGUI != NULL)
        {
            entityKey = m_genericGUI->getEntityKey();
            m_genericGUI->shutdown();
            m_genericGUI->deactivateAndDeleteServant();
            // Don't need to delete m_genericGUI as the call above does this.
        }
        //TD18060 marc_bugfix-cleanup onlineupdatelistener to avoid run time error
        //If m_genericGUI is NULL, config editor was not run from control station
        else
        {
            OnlineUpdateListener::cleanUp();
        }

        if (m_sessionHandler != NULL)
        {
            entityKey = m_sessionHandler->getEntityKey();
            delete m_sessionHandler;
            m_sessionHandler = NULL;
        }

        /*
        // TD11480: If the location key is not set, we cannot send audit messages. But even if it
        // is set, we still need to make sure it's not set to 0 (which is possible - eg if the db used
        // by the System Controller is missing the MFT's console). NotifyHosts also needs to be set.
        if ( RunParams::getInstance().isSet(RPARAM_LOCATIONKEY)
            && !RunParams::getInstance().get(RPARAM_NOTIFYHOSTS).empty()
            && RunParams::getInstance().get(RPARAM_LOCATIONKEY) != "0"
            && !m_isAlreadyRunning) // TD12059: Don't want an audit message if the app is running elsewhere.
        {
            // Get the audit message sender with which to send the message
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
                getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);

            TA_Base_Core::DescriptionParameters params;
            TA_Base_Core::NameValuePair param1(TransActiveWinApp::APPLICATION_NAME, std::string(m_applicationName));
            TA_Base_Core::NameValuePair param2(TransActiveWinApp::CONSOLE_NAME, m_consoleName);
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

        try
        {
            TA_Base_Core::CorbaUtil::getInstance().cleanup();
        }
        catch (...)
        {
        }

        ACE::fini();

        return CWinApp::ExitInstance();
        FUNCTION_EXIT;
    }

    bool CConfigurationEditorApp::runWithoutControlStation()
    {
        FUNCTION_ENTRY("runWithoutControlStation");

        bool shouldStartMessagePump = false;

        // First we need to set the application name to be the hard-coded one since we can't talk to
        // a Control Station
        RunParams::getInstance().set(RPARAM_APPNAME, m_applicationName);
		RunParams::getInstance().set(TA_Configuration::RPARAM_RUN_WITHOUT_CONTROL_STATION.c_str(), "true");
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Setting the RunParam RPARAM_PROGNAME as %s", m_applicationName);

        try
        {
            m_sessionHandler = new SessionHandler(m_gui);

            try
            {
                shouldStartMessagePump = m_sessionHandler->run();
            }
            catch (...)
            {
                return false;
            }

            submitAppStartedAuditMessage();

            //TD18359 marc_bugfix
            if (shouldStartMessagePump != false)
            {
                //TD18095, jianghp, to fix the performance of showing manager application
                m_gui.onInitGenericGUICompleted();
            }

            m_sessionHandler->endSession();
        }
        catch (const GenericGUIException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "GenericGUIException", "Now calculating which error it was so an appropriate message can be displayed to the user");

            switch (ex.getFailType())
            {
            case (GenericGUIException::ENTITY_CONFIGURATION_INVALID):
                showMsgBox(IDS_UE_010011);
                break;

            case (GenericGUIException::ENTITY_DOES_NOT_EXIST):
            case (GenericGUIException::ENTITY_NOT_UNIQUE):
                showMsgBox(IDS_UE_010013);
                break;

            case (GenericGUIException::COMMAND_LINE_INVALID):
                showMsgBox(IDS_UE_010008);
                break;

            case (GenericGUIException::NO_DATABASE_CONNECTION):
                showMsgBox(IDS_UE_010001);
                break;

            case (GenericGUIException::ACCESS_DENIED):
                showMsgBox(IDS_UE_010021, getOperatorNameFromSession(), "运行配置管理器");
                break;

            case (GenericGUIException::UNKNOWN_ACCESS_RIGHTS):
                showMsgBox(IDS_UE_010019);
                break;

            case (GenericGUIException::INITIALISATION_ERRORS):
            default:
                std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
                showMsgBox(IDS_UE_010012, appName);
                break;
            }
        }
        catch (const TA_Base_Core::ProcessAlreadyRunningException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "ProcessAlreadyRunningException", "Shutting down quietly.");
            m_isAlreadyRunning = true;
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Because this has been caught at the top level we must now shutdown.");
            TA_Base_Bus::TransActiveMessage userMsg;
            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
            userMsg << appName;
            userMsg.showMsgBox(IDS_UE_010010);
        }

        FUNCTION_EXIT;
        return shouldStartMessagePump;
    }

    void CConfigurationEditorApp::WinHelp(DWORD dwData, UINT nCmd)
    {
        if (HELP_CONTEXT == nCmd)
        {
            // retrieve the main frame and get the relevant help file for the current component.
            CMainFrame* frame = dynamic_cast<CMainFrame*>(GetMainWnd());

            if (frame != NULL)
            {
                std::string helpFile = frame->getHelpFile();

                if (!helpFile.empty())
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Using help file %s", helpFile.c_str());
                    TA_Base_Bus::HelpLauncher::getInstance().displayContextSpecificHelp(LOWORD(dwData), helpFile);
                    return;
                }
            }

            // Failed for some reason so just show generic context specific help
            TA_Base_Bus::HelpLauncher::getInstance().displayContextSpecificHelp(LOWORD(dwData));
            return;
        }

        // else display the main help contents
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }

    void CConfigurationEditorApp::submitAppStartedAuditMessage()
    {
        TA_ASSERT((m_genericGUI != NULL) || (m_sessionHandler != NULL),
                  "Must have successfully initialise app before it could send an audit message");

        // Need to get the console name of this session (if we can)
        try
        {
            unsigned long entityKey = (m_sessionHandler == NULL) ? m_genericGUI->getEntityKey() : m_sessionHandler->getEntityKey();

            // Get the audit message sender with which to send the message
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
                    getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);

            TA_Base_Core::DescriptionParameters params;
            TA_Base_Core::NameValuePair param1(TransActiveWinApp::APPLICATION_NAME, std::string(m_applicationName));
            TA_Base_Core::NameValuePair param2(TransActiveWinApp::CONSOLE_NAME, m_consoleName);
            params.push_back(&param1);
            params.push_back(&param2);

            sender->sendAuditMessage(TA_Base_Core::ApplicationStatusAudit::ApplicationStart,
                                     entityKey,
                                     params,
                                     "N/A",
                                     TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                     "",
                                     "",
                                     "");

            delete sender;
            sender = NULL;
        }
        catch (...)
        {
            // If there are any problems, then just log and move on
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",
                                "An exception was caught while attempting to send an audit message");
        }
    }

    std::string CConfigurationEditorApp::retrieveConsoleName()
    {
        // Need to get the console name of this session (if we can)
        try
        {
            TA_Base_Core::IConsole* theConsole = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

            std::string consoleName = theConsole->getName().c_str();

            delete theConsole;
            theConsole = NULL;

            return consoleName;
        }
        catch (...)
        {
            // If there are any problems, then just log that we couldn't get the console name and move on
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",
                                "An exception was caught while attempting to retrieve the console details for this session.  Using the console name instead");
            std::string consoleName = getConsoleName();
            return consoleName;
        }
    }

    //TD18355 marc_bugfix start
    void CConfigurationEditorApp::setDBConnectionString()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "setDBConnectionString");

        //
        // Retrieve the entity name from RunParams to ensure it was on the command line.
        //
        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

        if (entityName.empty())
        {
            TA_THROW(GenericGUIException("No entity specified on command line (ie --entity-name not found)", GenericGUIException::COMMAND_LINE_INVALID));
        }

        // TD15529

        if (entityName == CONFIGURATIONEDITOR_APPNAME)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Try to Set the RunParam RPARAM_DBCONNECTIONFILE");

            if (RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
            {
                std::string localDbConnectionFile = RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current RunParam RPARAM_DBCONNECTIONFILE is %s", localDbConnectionFile.c_str());
                int i = localDbConnectionFile.rfind("\\");

                if (i != std::string::npos)
                {
                    // The run parameter has both the file name and path
                    std::string configDbConnectionFile;
                    configDbConnectionFile.assign(localDbConnectionFile, 0, i + 1);
                    configDbConnectionFile += CONFIGCONNECTIONFILE_NAME;
                    RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, configDbConnectionFile.c_str());
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());
                }
                else
                {
                    // The run parameter has only the file name, no path
                    RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, CONFIGCONNECTIONFILE_NAME.c_str());
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());
                }
            }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "setDBConnectionString");
    }
    //TD18355 marc_bugfix end

    std::string CConfigurationEditorApp::getOperatorNameFromSession()
    {
        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary().getSessionInfo(sessionId, sessionId);
            std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
            return iOperator->getName();
        }
        catch (...)
        {        	
        }

        return "<Unallocated>";
    }
}
