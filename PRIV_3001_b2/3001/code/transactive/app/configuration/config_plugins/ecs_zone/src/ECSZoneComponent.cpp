/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneComponent.h"
#include "app/configuration/config_plugins/ecs_zone/src/resource.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneFrameworkView.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString ECSZoneComponent::DLL_NAME = "TA_ECSZoneConfigPlugin.dll";


    ECSZoneComponent::ECSZoneComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new ECSZoneDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ECS_ZONES) );
    }


    ECSZoneComponent::~ECSZoneComponent()
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


    HICON ECSZoneComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* ECSZoneComponent::getComponentView()
    {
        return RUNTIME_CLASS(ECSZoneFrameworkView);
    }


    std::string ECSZoneComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
