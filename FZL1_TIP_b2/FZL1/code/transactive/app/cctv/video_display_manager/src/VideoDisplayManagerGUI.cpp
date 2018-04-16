/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerGUI.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"
#include "bus\generic_gui/src/AbstractGUIApplication.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "GwxServiceMock.h"
#include "VideoDisplayManagerDAIImpl.h"
#include "ISwitchAgentService.h"
#include "GwxDisplayWrapper.h"
#include "ISequenceEditorView.h"
#include "SequenceEditorDlg.h"
#include "StationVideoDisplayManagerDlg.h"
#include "IPTZControlView.h"
#include "TestDlg.h"

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
        AbstractDialogGUI(IDD_VIDEODISPLAYMANAGER_DIALOG),
		m_presenter(NULL),
		m_dai(NULL)
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
				setApplicationWnd(NULL);
            }
            if(m_presenter != NULL)
			{
                delete m_presenter;
				m_presenter = NULL;
			}
            DisplayItemManager::removeInstance();
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
        if((pWndPrev = CWnd::FindWindow(NULL, OCC_DISPLAY_MANAGER_NAME))
			|| (pWndPrev = CWnd::FindWindow(NULL, STATION_DISPLAY_MANAGER_NAME)))
        {
            CWnd * pWndChild = pWndPrev->GetLastActivePopup();
            pWndPrev->ShowWindow(SW_RESTORE);
            pWndChild->SetForegroundWindow();
            exit(0);
        }
        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
        m_presenter = new VideoDisplayManagerPresenter(m_dai, new GwxDisplayWrapper());
        TA_Base_Core::ILocation::ELocationType locationType = m_presenter->getLocationType();
        application->setLocationType(locationType);
		IGFVideoSwitchDisplayView* pDlg = NULL;
        if(locationType == TA_Base_Core::ILocation::OCC)
        {
            this->setDlgResourceID(IDD_VIDEODISPLAYMANAGER_DIALOG);
            pDlg = new VideoDisplayManagerDlg(*this, *m_presenter);
		}
		else
		{
            this->setDlgResourceID(IDD_STATIONDISPLAY_DIALOG);
            pDlg  = new StationVideoDisplayManagerDlg(*this, *m_presenter);
		}
        m_presenter->SetIView(pDlg);
        setApplicationWnd(*pDlg);
		const char* appName = DISPLAY_MANAGER_NAME;
		TA_Base_Core::RunParams::getInstance().set(RPARAM_APPNAME, appName);
		AfxGetApp()->m_pszAppName = _strdup(appName);
		afxCurrentAppName = _strdup(appName);
        if("true" == TA_Base_Core::RunParams::getInstance().get("LocalTest"))
        {
            CTestDlg *pDlg = new CTestDlg();
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
        try
        {
			// Load the ptzrequestinterval parameter
			std::string ptzRequestInterval = entityData->getHelper()->getParameter(PTZREQUESTINTERVALEPRAMETERNAME);
			RunParams::getInstance().set(PTZREQUESTINTERVALEPRAMETERNAME, ptzRequestInterval.c_str());
	        // Load the default display parameter
            VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
            TA_ASSERT(application != NULL, "Application is NULL");
            application->setDefaultDwellTime(entityData->getDefaultDwellTime());
			// Load the video aspectratio parameter
			std::string videoAspectRatio = entityData->getHelper()->getParameter(VIDEOASPECTRATIOPARAMETERNAME);
			RunParams::getInstance().set(VIDEOASPECTRATIOPARAMETERNAME, videoAspectRatio.c_str());
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
