/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/incident_rights/src/IncidentRightsComponent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// IncidentRightsComponent.cpp: implementation of the IncidentRightsComponent class.
//
//////////////////////////////////////////////////////////////////////
#include <sstream>

#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsComponent.h"
#include "app/configuration/config_plugins/incident_rights/src/resource.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/SingletonHelper.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
	// This is the name of the DLL and is used to load resources correctly.
    const CString IncidentRightsComponent::DLL_NAME = "TA_IncidentRightsConfigPlugin.dll";

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	
	IncidentRightsComponent::IncidentRightsComponent(const std::string& componentName)
		: m_icon(NULL),
		  m_componentName(componentName),
		  m_frameworkView(NULL),
		  m_statusBar(NULL)
	{
		FUNCTION_ENTRY("Constructor");
		m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_APP_INCIDENT) );
		FUNCTION_EXIT;		
	}
	
	IncidentRightsComponent::~IncidentRightsComponent()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;	
	}
	
	HICON IncidentRightsComponent::getIcon()
	{
		FUNCTION_ENTRY("getIcon");
		FUNCTION_EXIT;	
		return m_icon;
	}
	
	CRuntimeClass* IncidentRightsComponent::getComponentView()
	{
		return RUNTIME_CLASS(IncidentRightsView);
	}
	
	
	std::string IncidentRightsComponent::getDLLName() const
	{	
		return std::string(DLL_NAME);
	}
	
	void IncidentRightsComponent::setUpSingletons(const TA_Base_App::IDllHandler &dllHandler)
	{
		FUNCTION_ENTRY("setUpSingletons");

        TA_Base_App::SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, getDLLName() ); 

        FUNCTION_EXIT;		
	}
	
	void IncidentRightsComponent::itemSelected(const CString &menuItem)
	{		
		 FUNCTION_ENTRY("itemSelected");
        // Nothing to be done here as this component has no menu or toolbar items.
        // This should never actually be called.
        FUNCTION_EXIT;
	}
	
	std::vector<TA_Base_App::MenuItem> IncidentRightsComponent::getMenuItems()
	{
		// We do not want any menu items
        std::vector<TA_Base_App::MenuItem> menuItems;
        return menuItems;		
	}
	
	std::vector<TA_Base_App::ToolBarItem> IncidentRightsComponent::getToolBarItems()
	{
		 // We do not want any toolbar items
        std::vector<TA_Base_App::ToolBarItem> toolbarItems;
        return toolbarItems;
		
	}
	
	void IncidentRightsComponent::setCreatedView(CView* ourView)
	{
		FUNCTION_ENTRY("setCreatedView");		
		
    	TA_ASSERT(ourView != NULL, "The view passed to us was NULL");        
		m_frameworkView = dynamic_cast<IncidentRightsView*>(ourView);
        TA_ASSERT(m_frameworkView != NULL, "The wrong type of view was passed to us");
				
        // Before we set the component interface in the view we will set our the component name in the
        // RunParams so the views can retrieve it. We have to do this each time as the component name can
        // change within this DLL.
        TA_Base_Core::RunParams::getInstance().set(RPARAM_COMPONENT.c_str(), m_componentName.c_str());

        m_frameworkView->initialise(this,m_statusBar);
		m_frameworkView->redraw();
		FUNCTION_EXIT;	

	}
	
 	void IncidentRightsComponent::reload()
	{
		  FUNCTION_ENTRY("reload");
		  // don't need to do anything as the matrices will be re-fetched when the view
		  // is redrawn.
		  FUNCTION_EXIT;		
	}
	
	bool IncidentRightsComponent::areCurrentChangesPending(std::vector<std::string>& dataNotApplied)
	{
		FUNCTION_ENTRY("areCurrentChangesPending");		
		bool bChangesPending = false;

		if (m_frameworkView != NULL)
		{
			  bChangesPending = m_frameworkView->areCurrentChangesPending();
		}
		FUNCTION_EXIT;			
		return bChangesPending; 
	}

	void IncidentRightsComponent::setStatusBar(TA_Base_App::IProgressStatusBar* statusBar)
	{
		FUNCTION_ENTRY("setStatusBar");
		
        TA_ASSERT(statusBar != NULL, "The status bar passed to us was NULL");
		
        m_statusBar = statusBar;	
	 
        FUNCTION_EXIT;		
	}

	
}

