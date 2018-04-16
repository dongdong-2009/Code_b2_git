#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerGUI.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerMainDlg.h"

namespace TA_IRS_App
{
	TimeTableManagerGUI::TimeTableManagerGUI(void)
	:TA_Base_Bus::AbstractDialogGUI(IDD_TIMETABLEMANAGER_MAIN_DIALOG)
	,m_timeTableManager(NULL),m_commandLineInvalid(false)
	{
		FUNCTION_ENTRY("TimeTableManagerGUI");
		FUNCTION_EXIT;
	}

	TimeTableManagerGUI::~TimeTableManagerGUI(void)
	{
		FUNCTION_ENTRY("~TimeTableManagerGUI");
		if (NULL != m_timeTableManager)
		{
			delete m_timeTableManager;
			m_timeTableManager = NULL;
		}
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::createApplicationObject()
	{	
		FUNCTION_ENTRY("createApplicationObject");
		TA_Base_Bus::ScadaProxyFactory::getInstance();
		if (NULL == m_timeTableManager)
		{
			m_timeTableManager = new TimeTableManagerMainDlg(*this);
		}
		setApplicationWnd(*m_timeTableManager); 
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::prepareForClose()
	{
		FUNCTION_ENTRY("prepareForClose");
		TimeTableDatapointPairManagerInstance::instance()->uninit();
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::onInitGenericGUICompleted()
	{
		FUNCTION_ENTRY("onInitGenericGUICompleted");
		FUNCTION_EXIT;
	}

	unsigned long TimeTableManagerGUI::getApplicationType()
	{
		FUNCTION_ENTRY("getApplicationType");
		FUNCTION_EXIT;
		return BAS_TIMETABLE_MANAGER_GUI_APPTYPE;
	}

	void TimeTableManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		FUNCTION_ENTRY("checkEntity");
		if (false == m_commandLineInvalid)
		{
			return;
		}
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::checkCommandLine()
	{
		FUNCTION_ENTRY("checkCommandLine");
		using namespace TA_Base_Core;
		m_commandLineInvalid =	RunParams::getInstance().isSet(RPARAM_ENTITYNAME) &&
								RunParams::getInstance().isSet(RPARAM_TIMETABLE_TYPE);
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::entityChanged(const std::vector<std::string>& changes)
	{
		FUNCTION_ENTRY("entityChanged");
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::serverIsDown()
	{
		FUNCTION_ENTRY("serverIsDown");
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::serverIsUp()
	{
		FUNCTION_ENTRY("serverIsUp");
		FUNCTION_EXIT;
	}

	void TimeTableManagerGUI::loadAllUserSettings()
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

	void TimeTableManagerGUI::saveAllUserSettings()
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
