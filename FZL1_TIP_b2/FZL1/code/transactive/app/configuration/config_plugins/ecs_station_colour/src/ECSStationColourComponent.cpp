/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourComponent.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>
	
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourComponent.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/resource.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourFrameworkView.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    // This is the name of the DLL and is used to load resources correctly.
    const CString ECSStationColourComponent::DLL_NAME = "TA_ECSStationColourConfigPlugin.dll";


    ECSStationColourComponent::ECSStationColourComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new ECSStationColourDatabaseAccessor())
    {
		// 7197-Start
		// m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_SEVERITY) );    
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ECS_STATION_COLOR) );
		// 7197-End
    }


    ECSStationColourComponent::~ECSStationColourComponent()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    HICON ECSStationColourComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* ECSStationColourComponent::getComponentView()
    {
        return RUNTIME_CLASS(ECSStationColourFrameworkView);
    }


    std::string ECSStationColourComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }


    std::vector<TA_Base_App::MenuItem> ECSStationColourComponent::getMenuItems()
    {
        return std::vector<TA_Base_App::MenuItem>();
    }

    std::vector<TA_Base_App::ToolBarItem> ECSStationColourComponent::getToolBarItems()
    {
        return std::vector<TA_Base_App::ToolBarItem>();
    }

}
