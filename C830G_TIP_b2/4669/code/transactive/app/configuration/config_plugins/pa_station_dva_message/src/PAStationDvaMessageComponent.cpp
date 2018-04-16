/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageComponent.cpp $
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

#include "StdAfx.h"
#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString PaStationDvaMessageComponent::DLL_NAME = "TA_PAStationDvaMessageConfigPlugin.dll";
    const DWORD PaStationDvaMessageComponent::RIGHTS_ACTION_ID = TA_Base_Bus::aca_CE_CONFIGURE_PA_STATION_DVA_MESSAGES;
        
    PaStationDvaMessageComponent::PaStationDvaMessageComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new PaStationDvaMessageDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_PA_STATION_DVA_MESSAGES) );
    }


    PaStationDvaMessageComponent::~PaStationDvaMessageComponent()
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


    HICON PaStationDvaMessageComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* PaStationDvaMessageComponent::getComponentView()
    {
        return RUNTIME_CLASS(PaStationDvaMessageFrameworkView);
    }


    std::string PaStationDvaMessageComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }

    std::vector<TA_Base_App::MenuItem> PaStationDvaMessageComponent::getMenuItems()
    {
        // We do not want any menu items
        std::vector<TA_Base_App::MenuItem> menuItems;
        return menuItems;
    }


    std::vector<TA_Base_App::ToolBarItem> PaStationDvaMessageComponent::getToolBarItems()
    {
        // We do not want any toolbar items
        std::vector<TA_Base_App::ToolBarItem> toolbarItems;
        return toolbarItems;
    }

}
