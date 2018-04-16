/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapComponent.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/resource.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapFrameworkView.h"
#include "app/configuration/config_plugins/ats_power_entity_map/src/AtsPowerEntityMapDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AtsPowerEntityMapComponent::DLL_NAME = "TA_AtsPowerEntityMapConfigPlugin.dll";


    AtsPowerEntityMapComponent::AtsPowerEntityMapComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new AtsPowerEntityMapDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ATS) );
    }


    AtsPowerEntityMapComponent::~AtsPowerEntityMapComponent()
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


    HICON AtsPowerEntityMapComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AtsPowerEntityMapComponent::getComponentView()
    {
        return RUNTIME_CLASS(AtsPowerEntityMapFrameworkView);
    }


    std::string AtsPowerEntityMapComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
