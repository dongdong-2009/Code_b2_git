/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/scada/wf_inhibitor/src/WFInhibitorGUI.cpp  $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides the main dialog used to set WF Global Inhibit information.  
  */
  
  
#include "StdAfx.h"
#include "bus/application_types/src/apptypes.h"
#include "app/scada/WFInhibitor/src/WFInhibitorGUI.h"
#include "app/scada/WFInhibitor/src/WFInhibitorDlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
	WFInhibitorGUI::WFInhibitorGUI(void)
	:
	TA_Base_Bus::AbstractDialogGUI(IDD_WF_INHIBITOR_DIALOG)
	{
		FUNCTION_ENTRY("WFInhibitorGUI");
		FUNCTION_EXIT;
	}

	WFInhibitorGUI::~WFInhibitorGUI(void)
	{
		FUNCTION_ENTRY("~WFInhibitorGUI");
		FUNCTION_EXIT;
	}


	void WFInhibitorGUI::createApplicationObject()
	{	
		FUNCTION_ENTRY("createApplicationObject");

		CWFInhibitorDlg* dlg = new CWFInhibitorDlg(*this);
		setApplicationWnd(*dlg);
		
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::prepareForClose()
	{
		FUNCTION_ENTRY("prepareForClose");
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::onInitGenericGUICompleted()
	{
		FUNCTION_ENTRY("onInitGenericGUICompleted");
		FUNCTION_EXIT;
	}

	unsigned long WFInhibitorGUI::getApplicationType()
	{
		FUNCTION_ENTRY("getApplicationType");
		FUNCTION_EXIT;

		return WF_INHIBITOR_GUI_APPTYPE;
	}

	void WFInhibitorGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		FUNCTION_ENTRY("checkEntity");

		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::checkCommandLine()
	{
		FUNCTION_ENTRY("checkCommandLine");
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::entityChanged(const std::vector<std::string>& changes)
	{
		FUNCTION_ENTRY("entityChanged");
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::serverIsDown()
	{
		FUNCTION_ENTRY("serverIsDown");
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::serverIsUp()
	{
		FUNCTION_ENTRY("serverIsUp");
		FUNCTION_EXIT;
	}

	void WFInhibitorGUI::loadAllUserSettings()
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

	void WFInhibitorGUI::saveAllUserSettings()
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
