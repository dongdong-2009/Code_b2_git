/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerGUI.cpp $
  * @author:  Robert Young
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/12/13 17:27:10 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * TODO: <description>
  */

// VideoDisplayManagerGUI.cpp: implementation of the VideoDisplayManagerGUI class.
//
//////////////////////////////////////////////////////////////////////

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "stdafx.h"

#include "app/cctv/video_display_manager/src/VideoDisplayManagerGUI.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlgMODBUS.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/GwxServiceMock.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDAIImpl.h"
#include "app/cctv/video_display_manager/src/ISwitchAgentService.h"
#include "app/cctv/video_display_manager/src/GwxDisplayWrapper.h"
#include "app/cctv/video_display_manager/src/ISequenceEditorView.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/StationVideoDisplayManagerDlgIMOS.h"
#include "app/cctv/video_display_manager/src/StationVideoDisplayManagerDlgMODBUS.h"
#include "app/cctv/video_display_manager/src/IPTZControlView.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenterIMOS.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenterMODBUS.h"
#include "app/cctv/video_display_manager/src/TestDlg.h"
#include "app/cctv/video_display_manager/src/Configuration.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AbstractGUIApplication.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Core::UserSettingsException;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace TA_IRS_App
{
    VideoDisplayManagerGUI::VideoDisplayManagerGUI()
        :
        //   AbstractDialogGUI (IDD_StationVideoDisplay_DIALOG)
        AbstractDialogGUI(IDD_VIDEODISPLAYMANAGER_DIALOG)
    {
        FUNCTION_ENTRY("VideoDisplayManagerGUI");
        m_dai = new VideoDisplayManagerDAIImpl();
        FUNCTION_EXIT;
    }


    VideoDisplayManagerGUI::~VideoDisplayManagerGUI()
    {
        FUNCTION_ENTRY("~VideoDisplayManagerGUI");
        try
        {
            if(getApplicationWnd() != NULL)
            {
                delete getApplicationWnd();
            }
            if(m_presenter != NULL)
			{
                delete m_presenter;
			}
            TA_IRS_App::SwitchAgentCommunicator::getInstance().removeInstance();
        }
        catch(...)
        {
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::createApplicationObject()
    {
        FUNCTION_ENTRY("createApplicationObject");
        //Check whether already one instance is running and bring that window to focus.
        CWnd *pWndPrev;
        if((pWndPrev = CWnd::FindWindow(NULL, _T("闭路电视系统管理器（控制中心）")))
			|| (pWndPrev = CWnd::FindWindow(NULL, _T("闭路电视系统管理器(车站)"))))
        {
            CWnd * pWndChild = pWndPrev->GetLastActivePopup();
            pWndPrev->ShowWindow(SW_RESTORE);
            pWndChild->SetForegroundWindow();
            exit(0);
        }

        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        TA_Base_Core::ILocation* location = m_dai->getLocation(sessionID);
        TA_Base_Core::IConsole* console = m_dai->getConsole(sessionID);
		CCTVConfiguration::getinstance().setLocation(location);
		CCTVConfiguration::getinstance().setConsole(console);
			
		const TA_Base_Core::ILocation::ELocationType locationType = CCTVConfiguration::getinstance().getLocationType();

		if(locationType == TA_Base_Core::ILocation::DPT)
		{
	        m_presenter = new VideoDisplayManagerPresenterIMOS(new GwxDisplayWrapper());
		}
		else
		{
			m_presenter = new VideoDisplayManagerPresenterMODBUS(new GwxDisplayWrapper());
		}

        ISwitchAgentService* agentService = new SwitchAgentCommunicator();
		TA_Base_Core::RunParamUser* runParamUser = NULL;
        if(locationType == TA_Base_Core::ILocation::OCC)
        {
			this->setDlgResourceID(VideoDisplayManagerDlgMODBUS::IDD);
            VideoDisplayManagerDlgMODBUS *pDlg = new VideoDisplayManagerDlgMODBUS(*this, *m_presenter);
            m_presenter->SetIView(pDlg);
            setApplicationWnd(*pDlg);
			runParamUser = pDlg;
        }
		else if(locationType == TA_Base_Core::ILocation::DPT)
        {
			this->setDlgResourceID(StationVideoDisplayManagerDlgIMOS::IDD);
            StationVideoDisplayManagerDlgIMOS *pDlg  = new StationVideoDisplayManagerDlgIMOS(*this, *m_presenter);
            m_presenter->SetIView(pDlg);
            setApplicationWnd(*pDlg);
			runParamUser = pDlg;
        }
		else
		{
			this->setDlgResourceID(StationVideoDisplayManagerDlgMODBUS::IDD);
            StationVideoDisplayManagerDlgMODBUS *pDlg  = new StationVideoDisplayManagerDlgMODBUS(*this, *m_presenter);
            m_presenter->SetIView(pDlg);
            setApplicationWnd(*pDlg);
			runParamUser = pDlg;
		}
		const char* appName = "闭路电视系统管理器";
		TA_Base_Core::RunParams::getInstance().set(RPARAM_APPNAME, appName);
		AfxGetApp()->m_pszAppName = _strdup(appName);
		afxCurrentAppName = _strdup(appName);
        if("true" == TA_Base_Core::RunParams::getInstance().get("LocalTest"))
        {
            CTestDlg *pDlg = new CTestDlg();
			pDlg->setRunParamUser(runParamUser);
			pDlg->Create(CTestDlg::IDD, NULL);
        }
        FUNCTION_EXIT;
    }


    unsigned long VideoDisplayManagerGUI::getApplicationType()
    {
        FUNCTION_ENTRY("getApplicationType");
        FUNCTION_EXIT;
        return VIDIWALL_GUI_APPTYPE;
    }


    void VideoDisplayManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY("checkEntity");
        TA_ASSERT(guiEntity != NULL, "The IEntityData pointer passed in was NULL");
        setGuiEntity(*guiEntity);
        // cast down to the entity type
        TA_Base_Core::DisplayManagerEntityData* entityData = dynamic_cast< TA_Base_Core::DisplayManagerEntityData* >(guiEntity);
        if(NULL == entityData)
        {
            CString message;
            message.FormatMessage(IDS_UE010016_CANNOT_LOAD_DATA, TA_Base_Core::DisplayManagerEntityData::getStaticType().c_str(), "Entity");
            TA_THROW(TA_Base_Core::GenericGUIException(std::string(message), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }
        // load the default display parameter
        try
        {
			CCTVConfiguration::getinstance().setCCTVSubsystemKey(entityData->getSubsystem());
			CCTVConfiguration::getinstance().setDefaultDwellTime(entityData->getDefaultDwellTime());

        }
        catch(TA_Base_Core::TransactiveException&)
        {
            CString message;
            message.FormatMessage(IDS_UE010016_CANNOT_LOAD_DATA, TA_Base_Core::DisplayManagerEntityData::getStaticType().c_str(), "Configuration Data");
            TA_THROW(TA_Base_Core::GenericGUIException(std::string(message), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::checkCommandLine()
    {
        FUNCTION_ENTRY("checkCommandLine");
        // TODO: Check any command line arguments specific to this application
        // are available and correct. Throw the specified exception if they
        // are not.
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY("entityChanged");
        // TODO: Check the changes passed and pass any relevant information on.
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::serverIsDown()
    {
        FUNCTION_ENTRY("serverIsDown");
        // TODO: Get the GUI to update to indicate this.
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::serverIsUp()
    {
        FUNCTION_ENTRY("serverIsUp");
        // TODO: Get the GUI to update to indicate this.
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY("loadAllUserSettings");
        // First check if we should be loading the user settings.
        if(RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            setWindowPosition();
            FUNCTION_EXIT;
            return;
        }
        if(getUserSetting() == NULL)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType())));
        }
        loadPositionSettings();
        // TODO: Restore any other settings here
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY("saveAllUserSettings");
        // First check if we should be saving the user settings.
        if(RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            FUNCTION_EXIT;
            return;
        }
        if(getUserSetting() == NULL)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType())));
        }
        savePositionSettings();
        // TODO: Perform any shutdown tasks here
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::prepareForClose()
    {
        FUNCTION_ENTRY("prepareForClose");
        try
        {
            saveAllUserSettings();
        }
        catch(UserSettingsException&)
        {
            // Handle here with appropriate errror message
        }
        // TODO: Perform any shutdown tasks here
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerGUI::onInitGenericGUICompleted()
    {
        FUNCTION_ENTRY("onInitGenericGUICompleted");
        FUNCTION_EXIT;
    }
} // TA_IRS_App
