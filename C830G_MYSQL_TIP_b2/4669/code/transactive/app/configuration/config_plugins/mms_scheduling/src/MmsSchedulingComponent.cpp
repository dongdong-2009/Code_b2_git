/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786 4284)

#include <sstream>

#include "app/configuration/config_plugins/mms_scheduling/src/StdAfx.h"
#include "app/configuration/config_plugins/mms_scheduling/src/CachedConfig.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingComponent.h"
#include "app/configuration/config_plugins/mms_scheduling/src/resource.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingView.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/SingletonHelper.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "ace/OS.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "ace/init_ACE.h"

using TA_Base_Core::DebugUtil;
using  namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString MmsSchedulingComponent::DLL_NAME = "TA_MmsScheduleConfigPlugin.dll";


    MmsSchedulingComponent::MmsSchedulingComponent(const std::string& componentName)
        : m_icon(NULL),
          m_frameworkView(NULL), 
          m_componentName(componentName),
          m_databaseAccessor(new MmsSchedulingDatabaseAccessor()),
          m_statusBar(NULL)
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_MMS) );
    }


    MmsSchedulingComponent::~MmsSchedulingComponent()
    {
        FUNCTION_ENTRY("Destructor");

        // TD4325: ACE initialisation copied from AbstractComponent.
        ACE::fini();

        try
        {
            if (m_databaseAccessor != NULL)
            {
                delete m_databaseAccessor;
                m_databaseAccessor = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void MmsSchedulingComponent::setUpSingletons(const TA_Base_App::IDllHandler& dllHandler)
    {
        FUNCTION_ENTRY("setUpSingletons");

        TA_Base_App::SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, getDLLName() );

        try
        {
            // We must figure out the entity key as we do not yet have access to the ConfigurationEditorGUI
            // object to retrieve the key.
            std::auto_ptr<TA_Base_Core::IEntityData> entity(
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity( TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME) ));

            CachedConfig::getInstance().setEntityKey(entity->getKey());
        }
        catch (...)
        {
            // Error retrieving data
        }

        try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CachedConfig::getInstance().setSessionId(sessionId);
        }
        catch (...)
        {
            // Error retrieving data
        }

        FUNCTION_EXIT;
    }

    
    HICON MmsSchedulingComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* MmsSchedulingComponent::getComponentView()
    {
        return RUNTIME_CLASS(MmsSchedulingView);
    }


    std::string MmsSchedulingComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }


    std::vector<TA_Base_App::MenuItem> MmsSchedulingComponent::getMenuItems()
    {
        // We do not want any menu items
        std::vector<TA_Base_App::MenuItem> menuItems;
        return menuItems;
    }


    std::vector<TA_Base_App::ToolBarItem> MmsSchedulingComponent::getToolBarItems()
    {
        // We do not want any toolbar items
        std::vector<TA_Base_App::ToolBarItem> toolbarItems;
        return toolbarItems;
    }


    void MmsSchedulingComponent::setCreatedView(CView* ourView)
    {
        FUNCTION_ENTRY("setCreatedView");

        TA_ASSERT(ourView != NULL, "The view passed to us was NULL");

        // TD4325: ACE initialisation copied from AbstractComponent.
        ACE::init();

        m_frameworkView = dynamic_cast<MmsSchedulingView*>(ourView);
        
        TA_ASSERT(m_frameworkView != NULL, "The wrong type of view was passed to us");

        // Before we set the component interface in the view we will set our the component name in the
        // RunParams so the views can retrieve it. We have to do this each time as the component name can
        // change within this DLL.
        TA_Base_Core::RunParams::getInstance().set(RPARAM_COMPONENT.c_str(), m_componentName.c_str());

        m_frameworkView->setComponentInterface(this);

        FUNCTION_EXIT;
    }


    void MmsSchedulingComponent::setStatusBar(TA_Base_App::IProgressStatusBar* statusBar)
    {
        FUNCTION_ENTRY("setStatusBar");

        TA_ASSERT(statusBar != NULL, "The status bar passed to us was NULL");

        m_statusBar = statusBar;

        FUNCTION_EXIT;
    }


    void MmsSchedulingComponent::reload()
    {
        FUNCTION_ENTRY("reload");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");

        // This means that next time the user chooses this component the data will be loaded again.
        m_databaseAccessor->invalidateData();

        FUNCTION_EXIT;
    }


    bool MmsSchedulingComponent::areCurrentChangesPending(std::vector<std::string>& dataNotApplied)
    {
        FUNCTION_ENTRY("canLoseFocus");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");
        
        FUNCTION_EXIT;

        return m_databaseAccessor->areCurrentChangesPending(dataNotApplied);
    }


    void MmsSchedulingComponent::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        // Nothing to be done here as this component has no menu or toolbar items.
        // This should never actually be called.

        FUNCTION_EXIT;
    }

}
