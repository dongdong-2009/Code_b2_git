#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/BASManagerGUI.h"
#include "app/bas/BASModeManager/src/BASManagerDlg.h"

Namespace_TA_IRS_APP_Begin
BASManagerGUI::BASManagerGUI(void)
:TA_Base_Bus::AbstractDialogGUI(IDD_BASMANAGER_DIALOG)
,m_basManager(NULL),m_commandLineInvalid(false)
{
	FUNCTION_ENTRY("BASManagerGUI");
	FUNCTION_EXIT;
}

BASManagerGUI::~BASManagerGUI(void)
{
	FUNCTION_ENTRY("~BASManagerGUI");
	if (NULL != m_basManager)
	{
		delete m_basManager;
		m_basManager = NULL;
	}
	FUNCTION_EXIT;
}

void BASManagerGUI::createApplicationObject()
{	
	FUNCTION_ENTRY("createApplicationObject");
    TA_Base_Bus::ScadaProxyFactory::getInstance();
	if (NULL == m_basManager)
	{
		m_basManager = new CBASManagerDlg(*this);
	}
	setApplicationWnd(*m_basManager); 
	FUNCTION_EXIT;
}

void BASManagerGUI::prepareForClose()
{
	FUNCTION_ENTRY("prepareForClose");
	m_basManager->prepareClose();
    TA_Base_Bus::ScadaProxyFactory::removeInstance();
	FUNCTION_EXIT;
}

void BASManagerGUI::onInitGenericGUICompleted()
{
	FUNCTION_ENTRY("onInitGenericGUICompleted");
	FUNCTION_EXIT;
}

unsigned long BASManagerGUI::getApplicationType()
{
	FUNCTION_ENTRY("getApplicationType");	
	
	FUNCTION_EXIT;
	return BAS_MODE_MANAGER_GUI_APPTYPE;
}

void BASManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity) throw(TA_Base_Core::TransactiveException)
{
	FUNCTION_ENTRY("checkEntity");
	if (false == m_commandLineInvalid)
	{
		return;
	}
	FUNCTION_EXIT;
}

void BASManagerGUI::checkCommandLine() throw(TA_Base_Core::TransactiveException)
{
	FUNCTION_ENTRY("checkCommandLine");


	m_commandLineInvalid = true;
	FUNCTION_EXIT;
}

void BASManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
	FUNCTION_ENTRY("entityChanged");
	FUNCTION_EXIT;
}

void BASManagerGUI::serverIsDown()
{
	FUNCTION_ENTRY("serverIsDown");
	FUNCTION_EXIT;
}

void BASManagerGUI::serverIsUp()
{
	FUNCTION_ENTRY("serverIsUp");
	FUNCTION_EXIT;
}

void BASManagerGUI::loadAllUserSettings()
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

void BASManagerGUI::saveAllUserSettings()
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
Namespace_TA_IRS_APP_End