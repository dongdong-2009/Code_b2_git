#include "StdAfx.h"
#include "bus/application_types/src/apptypes.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerGUI.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDlg.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	TunnelModeManagerGUI::TunnelModeManagerGUI(void)
	:TA_Base_Bus::AbstractDialogGUI(IDD_TUNNELMODEMANAGER_MAIN_PANNEL)
	,m_tunnelManager(NULL),m_commandLineInvalid(false)
	{
		FUNCTION_ENTRY("TunnelModeManagerGUI");
		FUNCTION_EXIT;
	}

	TunnelModeManagerGUI::~TunnelModeManagerGUI(void)
	{
		FUNCTION_ENTRY("~TimeTableManagerGUI");
		if (NULL != m_tunnelManager)
		{
			delete m_tunnelManager;
			m_tunnelManager = NULL;
		}
		FUNCTION_EXIT;
	}


	void TunnelModeManagerGUI::createApplicationObject()
	{	
		FUNCTION_ENTRY("createApplicationObject");
		if (NULL == m_tunnelManager)
		{
			m_tunnelManager = new CTunnelModeManagerDlg(*this);
		}
		setApplicationWnd(*m_tunnelManager); 
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::prepareForClose()
	{
		FUNCTION_ENTRY("prepareForClose");
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::onInitGenericGUICompleted()
	{
		FUNCTION_ENTRY("onInitGenericGUICompleted");
		FUNCTION_EXIT;
	}

	unsigned long TunnelModeManagerGUI::getApplicationType()
	{
		FUNCTION_ENTRY("getApplicationType");

		unsigned long applicationType = BAS_TUNNEL_SELECTION_MANAGER_GUI_APPTYPE;
		std::string strTypeValue = TA_Base_Core::RunParams::getInstance().get(RPARAM_TUNNELMODE_TYPE);

		if (0 == strTypeValue.compare(RPARAM_TUNNELMODE_TYPE_VALUE_FIRE))
		{
			applicationType = BAS_TUNNEL_FIRE_MANAGER_GUI_APPTYPE;
		}
		else if (0 == strTypeValue.compare(RPARAM_TUNNELMODE_TYPE_VALUE_CONGESTION))
		{
			applicationType = BAS_TUNNEL_CONGESTION_MANAGER_GUI_APPTYPE;
		}

		FUNCTION_EXIT;
		return applicationType;
	}

	void TunnelModeManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		FUNCTION_ENTRY("checkEntity");
		if (false == m_commandLineInvalid)
		{
			return;
		}
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::checkCommandLine()
	{
		FUNCTION_ENTRY("checkCommandLine");
		using namespace TA_Base_Core;
		m_commandLineInvalid =	RunParams::getInstance().isSet(RPARAM_ENTITYNAME) &&
			RunParams::getInstance().isSet(RPARAM_TUNNELMODE_TYPE);
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::entityChanged(const std::vector<std::string>& changes)
	{
		FUNCTION_ENTRY("entityChanged");
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::serverIsDown()
	{
		FUNCTION_ENTRY("serverIsDown");
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::serverIsUp()
	{
		FUNCTION_ENTRY("serverIsUp");
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::loadAllUserSettings()
	{
		FUNCTION_ENTRY("loadAllUserSettings");
		// First check if we should be loading the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
			setWindowPosition();
			FUNCTION_EXIT;
			return;
		}

		loadPositionSettings();
		FUNCTION_EXIT;
	}

	void TunnelModeManagerGUI::saveAllUserSettings()
	{
		FUNCTION_ENTRY("saveAllUserSettings");
		// First check if we should be saving the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
			return;
		}

		savePositionSettings();
		FUNCTION_EXIT;
	}
}
