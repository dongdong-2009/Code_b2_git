#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerGUI.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerDlg.h"

namespace TA_IRS_App
{
	BasParameterManagerGUI::BasParameterManagerGUI(void)
		:m_basManager(NULL),
		TA_Base_Bus::AbstractDialogGUI(IDD_BASPARAMETERMANAGER_DIALOG)
	{
		FUNCTION_ENTRY("BasParameterManagerGUI");
		FUNCTION_EXIT;
	}

	BasParameterManagerGUI::~BasParameterManagerGUI(void)
	{
		FUNCTION_ENTRY("~BasParameterManagerGUI");
		FUNCTION_EXIT;
	}


	void BasParameterManagerGUI::createApplicationObject()
	{	
        FUNCTION_ENTRY( "createApplicationObject" );
        TA_Base_Bus::ScadaProxyFactory::getInstance();
		if (NULL == m_basManager)
		{
			m_basManager = new CBasParameterManagerDlg(*this);
		}
		setApplicationWnd(*m_basManager); 
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::prepareForClose()
	{
		FUNCTION_ENTRY("prepareForClose");
        m_basManager->prepareClose();
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::onInitGenericGUICompleted()
	{
		FUNCTION_ENTRY("onInitGenericGUICompleted");
		FUNCTION_EXIT;
	}

	unsigned long BasParameterManagerGUI::getApplicationType()
	{
		FUNCTION_ENTRY("getApplicationType");	

		FUNCTION_EXIT;
		return BAS_PARAMETER_MANAGER_GUI_APPTYPE;
	}

	void BasParameterManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity) throw(TA_Base_Core::TransactiveException)
	{
		FUNCTION_ENTRY("checkEntity");
		if (false == m_commandLineInvalid)
		{
			return;
		}
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::checkCommandLine() throw(TA_Base_Core::TransactiveException)
	{
		FUNCTION_ENTRY("checkCommandLine");


		m_commandLineInvalid = true;
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::entityChanged(const std::vector<std::string>& changes)
	{
		FUNCTION_ENTRY("entityChanged");
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::serverIsDown()
	{
		FUNCTION_ENTRY("serverIsDown");
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::serverIsUp()
	{
		FUNCTION_ENTRY("serverIsUp");
		FUNCTION_EXIT;
	}

	void BasParameterManagerGUI::loadAllUserSettings()
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

	void BasParameterManagerGUI::saveAllUserSettings()
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