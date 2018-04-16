/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapComponent.cpp $
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

#pragma warning(disable:4786)

#include <sstream>

#include "AtsDssPlanMapComponent.h"
#include "resource.h"
#include "AtsDssPlanMapFrameworkView.h"
#include "AtsDssPlanMapDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AtsDssPlanMapComponent::DLL_NAME = "TA_AtsDssPlanMapConfigPlugin.dll";


    AtsDssPlanMapComponent::AtsDssPlanMapComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new AtsDssPlanMapDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ATS) );
    }


    AtsDssPlanMapComponent::~AtsDssPlanMapComponent()
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


    HICON AtsDssPlanMapComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AtsDssPlanMapComponent::getComponentView()
    {
        return RUNTIME_CLASS(AtsDssPlanMapFrameworkView);
    }


    std::string AtsDssPlanMapComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
