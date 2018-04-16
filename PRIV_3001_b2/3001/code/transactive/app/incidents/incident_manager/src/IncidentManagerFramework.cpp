/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/incident_manager/src/IncidentManagerFramework.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Provide the interface to access database
  *
  */
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "app/incidents/incident_manager/src/CIncidentManagerApp.h"
#include "app/incidents/incident_manager/src/CtaIncidentSummary.h"
#include "app/incidents/incident_manager/src/IncidentManagerFramework.h"
#include "app/incidents/incident_manager/src/IncidentManagerDoc.h"

#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/GenericGuiException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

using namespace TA_Base_Bus;

namespace TA_IRS_App
{
	const std::string IncidentManagerFramework::RPARAM_REPORTTEMPLATE = "ReportTemplate";
	
	IncidentManagerFramework::IncidentManagerFramework() :
    m_CtaIncidentSummary(NULL),
		m_alarmIdFormCommandLine("")
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}
	
	IncidentManagerFramework::~IncidentManagerFramework()
	{
		FUNCTION_ENTRY( "Destructor" );
		
		return;
		
		if (m_CtaIncidentSummary != NULL) // lizettejl - TD13868 
		{
			delete m_CtaIncidentSummary;
			m_CtaIncidentSummary = NULL;
		}
		
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::loadAllUserSettings()
	{
		FUNCTION_ENTRY( "loadAllUserSettings" );
		// First check if we should be loading the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
		{
			setWindowPosition();
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "loadAllUserSettings" );            
			FUNCTION_EXIT;
			return;
		}
		
		if ( getUserSetting() == NULL )
		{
			setUserSetting( *( new SettingsMgr( getSessionId(), getApplicationType() ) ) );
		}
		
		loadPositionSettings();
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		FUNCTION_ENTRY( "checkEntity" );
		setGuiEntity( *guiEntity );

		TA_Base_Core::IEntityDataPtr guiEntityData(getGUIEntity());
		m_CtaIncidentSummary = new CtaIncidentSummary(guiEntityData);
		
		try
		{
			std::string templatePath = m_CtaIncidentSummary->getConfigParameterValue(RPARAM_REPORTTEMPLATE);
			TA_Base_Core::RunParams::getInstance().set( RPARAM_REPORTTEMPLATE.c_str(),templatePath.c_str() );
		}
		catch (std::exception& e) 
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error(%s) occurs while trying set the template path for the report template.", e.what());
		}
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::saveAllUserSettings()
	{
		FUNCTION_ENTRY( "saveAllUserSettings" );
		// First check if we should be saving the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get( RPARAM_USERPREFERENCES ).empty() )
		{
			FUNCTION_EXIT;
			return;
		}
		
		if ( getUserSetting() == NULL )
		{
			setUserSetting( *( new SettingsMgr( getSessionId(), getApplicationType() ) ) );
		}
		
		savePositionSettings();
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::dutyChanged()
	{
		FUNCTION_ENTRY( "dutyChanged" );
		
		//Mintao ++
		//Bug 642 
		// get document from main frame
		CWnd* app = getApplicationWnd();
		CMainFrame* frame = dynamic_cast<CMainFrame*>(app);
		TA_ASSERT( frame != NULL, "CMainFrame is NULL in duty changed!")
			CView *pView = frame->GetActiveView();
		TA_ASSERT( pView != NULL, "CView is NULL in duty changed!")
			// get document from main frame
			IncidentManagerDoc * pDoc = (IncidentManagerDoc *) pView->GetDocument();
		//Mintao ++
		//Bug 642 
		
		//Bugzilla Bug 179: Mintao
		if (NULL != pDoc)
		{
			pDoc->updateRights();    
		}
		
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::serverIsDown()
	{
		FUNCTION_ENTRY( "serverIsDown" );
		TA_ASSERT(m_CtaIncidentSummary != NULL, "Summary is NULL");
		
		m_CtaIncidentSummary->setServerState(false);
		dutyChanged();
		
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::serverIsUp()
	{
		FUNCTION_ENTRY( "serverIsUp" );
		TA_ASSERT(m_CtaIncidentSummary != NULL, "Summary is NULL");
		
		m_CtaIncidentSummary->setServerState(true);
		dutyChanged();
		
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::createApplicationObject()
	{
		FUNCTION_ENTRY( "createApplicationObject" );
		
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(IncidentManagerDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CIncidentManagerListView));
		
		CIncidentManagerApp *app = dynamic_cast<CIncidentManagerApp*>(AfxGetApp());
		
		//Bugzilla Bug 179: Mintao
		if (NULL != app)
		{
			app->AddDocTemplate(pDocTemplate);
			// Parse command line for standard shell commands, DDE, file open
			CCommandLineInfo cmdInfo;
			app->ParseCommandLine(cmdInfo);
			// Dispatch commands specified on the command line
			app->ProcessShellCommand(cmdInfo);
		}
		
		setApplicationWnd(*AfxGetMainWnd());
		
		FUNCTION_EXIT;
	}
	
	void IncidentManagerFramework::prepareForClose()
	{
		FUNCTION_ENTRY( "prepareForClose" );
		try
		{
			saveAllUserSettings();
		}
		catch ( TA_Base_Core::UserSettingsException& )
		{
			// Handle here with appropriate log message
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Save user setting exceptions." );            
		}
		//Mintao ++
		//Bug 642 
		// get document from main frame
		CWnd* app = getApplicationWnd();
		CMainFrame* frame = dynamic_cast<CMainFrame*>(app);
		TA_ASSERT( frame != NULL, "CMainFrame is NULL in prepareForClose!")
			CView *pView = frame->GetActiveView();
		TA_ASSERT( pView != NULL, "CView is NULL in prepareForClose!")
			// get document from main frame
			IncidentManagerDoc * pDoc = (IncidentManagerDoc *) pView->GetDocument();
		//Mintao ++
		//Bug 642 
		
		//Bugzilla Bug 179: Mintao
		if (NULL != pDoc)
		{
			pDoc->prepareForClose();
			
			
		}
		
		FUNCTION_EXIT;
	}
	
	
	ItaLocationAccessFactory& IncidentManagerFramework::getLocationAccessFactory()
	{
		FUNCTION_ENTRY("getLocationAccessFactory");
		FUNCTION_EXIT;
		//Bugzilla Bug 179: Mintao
		TA_ASSERT(NULL != m_CtaIncidentSummary, "CtaIncidentSummary is not initialized!");
		return *m_CtaIncidentSummary;
	}
	
	ItaIncidentRights& IncidentManagerFramework::getIncidentRightsChecker()
	{
		FUNCTION_ENTRY("getIncidentRightsChecker");
		FUNCTION_EXIT;
		//Bugzilla Bug 179: Mintao
		TA_ASSERT(NULL != m_CtaIncidentSummary, "CtaIncidentSummary is not initialized!");
		return *m_CtaIncidentSummary;
	}
	
	ItaIncidentReportFactory& IncidentManagerFramework::getIncidentReportFactory()
	{
		FUNCTION_ENTRY("getIncidentReportFactory");
		FUNCTION_EXIT;
		//Bugzilla Bug 179: Mintao
		TA_ASSERT(NULL != m_CtaIncidentSummary, "CtaIncidentSummary is not initialized!");
		return *m_CtaIncidentSummary;
	}
	
	TA_Base_Bus::ItaUserMessage& IncidentManagerFramework::getUserMessage()
	{
		FUNCTION_ENTRY("getUserMessage");
		FUNCTION_EXIT;
		//Bugzilla Bug 179: Mintao
		TA_ASSERT(NULL != m_CtaIncidentSummary, "CtaIncidentSummary is not initialized!");
		return *m_CtaIncidentSummary;
	}
	
	void IncidentManagerFramework::checkCommandLine()
	{
		FUNCTION_ENTRY( "checkCommandLine" );
		std::string value = TA_Base_Core::RunParams::getInstance().get("AlarmId");
		if (! value.empty())    
		{
			//when the Incident Manager Launch with alarm Id, the incident dialog 
			//should show.
			m_alarmIdFormCommandLine = value;
		}
		//28 Mar 2007, hongzhi
		CString strValue = TA_Base_Core::RunParams::getInstance().get("AlarmValue").c_str();
		strValue.Replace("{%Blank%}"," ");	// accpt blank as input parameters, 28.Mar.2007, hongzhi
		value = (LPCTSTR)strValue;
		if (! value.empty())    
		{
			//when the Incident Manager Launch with alarm value, the incident dialog 
			//should show.
			m_alarmValueFormCommandLine = value;
		}
		FUNCTION_EXIT;  
	}
	
	std::string IncidentManagerFramework::getAlarmIdFromCommandLine()
	{
		FUNCTION_ENTRY( "getAlarmIdFromCommandLine" );
		FUNCTION_EXIT;  
		return m_alarmIdFormCommandLine;
	}
	
	std::string IncidentManagerFramework::getAlarmValueFromCommandLine()
	{
		FUNCTION_ENTRY( "getAlarmValueFromCommandLine" );
		FUNCTION_EXIT;  
		return m_alarmValueFormCommandLine;
	}
	void IncidentManagerFramework::entityChanged( const std::vector<std::string>& changes )
	{
		FUNCTION_ENTRY( "entityChanged" );
		try
		{
			std::string templatePath = m_CtaIncidentSummary->getConfigParameterValue(RPARAM_REPORTTEMPLATE);
			TA_Base_Core::RunParams::getInstance().set( RPARAM_REPORTTEMPLATE.c_str(),templatePath.c_str() );
		}
		catch (std::exception& e) 
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error(%s) occurs while trying set the template path for the report template.", e.what());
		}
		FUNCTION_EXIT;  
	}
	
	unsigned long IncidentManagerFramework::getApplicationType()
	{
		FUNCTION_ENTRY( "getApplicationType" );
		FUNCTION_EXIT;  
		return INCIDENT_MANAGER_GUI_APPTYPE;
	}

	void IncidentManagerFramework::onInitGenericGUICompleted()
	{
		CMainFrame * pWnd = dynamic_cast<CMainFrame * >(getApplicationWnd());
		TA_ASSERT(pWnd != NULL, "the main frame windows is NULL!");
		pWnd->LockWindowUpdate();
		pWnd->init();
		pWnd->UnlockWindowUpdate();
		pWnd->UpdateWindow();
	}
}// end TA_IRS_App
