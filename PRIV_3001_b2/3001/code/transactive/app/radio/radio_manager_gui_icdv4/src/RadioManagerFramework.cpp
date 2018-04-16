/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioManagerFramework.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/StdAfx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManagerFramework.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/CtaRadioManagerSummary.h"

#include "bus/application_types/src/apptypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/GenericGuiException.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/data_access_interface/entity_access/src/DefaultEntity.h"

#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioManagerFramework::RadioManagerFramework() :
		AbstractDialogGUI(IDD_RADIOMANAGER_DIALOG), //TD18626 AVteam
        m_CRadioManagerDlg(NULL)
{
	
}


RadioManagerFramework::~RadioManagerFramework()
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"RadioManagerFramework::~RadioManagerFramework");

    if ( m_CRadioManagerDlg != NULL )
	{
		delete m_CRadioManagerDlg;
		m_CRadioManagerDlg = NULL;
	}

/*TD18626, jianghp
    	if ( m_CtaRadioManagerSummary != NULL )
    	{
    		delete m_CtaRadioManagerSummary;
    		m_CtaRadioManagerSummary = NULL;
    	}
    
        if (m_MFTFramework) // TD16870
        {
            delete m_MFTFramework;
            m_MFTFramework = NULL;
        }*/
    
}


void RadioManagerFramework::createApplicationObject()
{
    FUNCTION_ENTRY("createApplicationObject");
/*TD18626, jianghp
	if (TA_Base_Core::RunParams::getInstance().isSet("DebugEvent"))
    {
        InitializeDebugWindow(GetModuleHandle(NULL));
    }


	    // this must be constructed here
	    m_MFTFramework = new RadioMFTAgentFramework();
	
		// implicitly create the singleton
		The_RadioOperations::instance();
		The_RadioSystemStatus::instance()->setCommandServant(m_MFTFramework->getCommandServant()); //TD16870
	    
		// create Transactive interfaces
		m_CtaRadioManagerSummary = new CtaRadioManagerSummary(*this->getGUIEntity());
	    
		m_CtaRadioManagerSummary->Subscribe();*/
	

	m_CRadioManagerDlg = new CRadioManagerDlg(*this);


	setApplicationWnd(*m_CRadioManagerDlg);


    FUNCTION_EXIT;
}

void RadioManagerFramework::loadAllUserSettings()
{
    FUNCTION_ENTRY("loadAllUserSettings");

    FUNCTION_EXIT;
}


void RadioManagerFramework::saveAllUserSettings()
{
    FUNCTION_ENTRY("saveAllUserSettings");

    FUNCTION_EXIT;
}



unsigned long RadioManagerFramework::getApplicationType()
{
    FUNCTION_ENTRY("getApplicationType");
    FUNCTION_EXIT;

	return RADIO_CONTROLLER_GUI_APPTYPE;
}


void RadioManagerFramework::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    FUNCTION_ENTRY("checkEntity");

    setGuiEntity( *guiEntity );

    TA_Base_Core::DefaultEntity* app_IEntity = dynamic_cast<TA_Base_Core::DefaultEntity*>(guiEntity);
    if ( NULL == app_IEntity )
    {
        // Throw an exception to indicate entity name passed in waws not of valid type
        TA_THROW( TA_Base_Core::GenericGUIException( "The entity specifed to the application was not of type RadioManagerFramework EntityData",
            TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
    }

    FUNCTION_EXIT;
	
}


void RadioManagerFramework::checkCommandLine()
{
    FUNCTION_ENTRY("checkCommandLine");
    FUNCTION_EXIT;
	
}


void RadioManagerFramework::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY("entityChanged");
    FUNCTION_EXIT;
	
}


void RadioManagerFramework::serverIsDown()
{
    FUNCTION_ENTRY("serverIsDown");
    FUNCTION_EXIT;
	
}


void RadioManagerFramework::serverIsUp()
{
    FUNCTION_ENTRY("serverIsUp");
    FUNCTION_EXIT;
}


void RadioManagerFramework::prepareForClose()
{
    FUNCTION_ENTRY("prepareForClose");

    try
    {
        saveAllUserSettings();
    }
    catch ( TA_Base_Core::UserSettingsException& )
    {
        // Handle here with appropriate log message
       LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Save user setting exceptions." );            
    }
    
	
    FUNCTION_EXIT;
}
//TD18626 AVteam
void RadioManagerFramework::onInitGenericGUICompleted()
	{
		CWaitCursor cur;
		if (TA_Base_Core::RunParams::getInstance().isSet("DebugEvent"))
		{
			InitializeDebugWindow(GetModuleHandle(NULL));
		}

		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();

		CRadioManagerDlg * pDlg = dynamic_cast<CRadioManagerDlg * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");

		pDlg->init();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
		cur.Restore();
	}
//TD18626 AVteam
