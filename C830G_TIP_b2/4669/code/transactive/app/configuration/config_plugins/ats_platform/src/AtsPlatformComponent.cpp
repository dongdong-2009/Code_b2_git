/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ats_platform/src/AtsPlatformComponent.cpp $
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

#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformComponent.h"
#include "app/configuration/config_plugins/ats_platform/src/resource.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformFrameworkView.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AtsPlatformComponent::DLL_NAME = "TA_AtsPlatformConfigPlugin.dll";


    AtsPlatformComponent::AtsPlatformComponent(const std::string& componentName)
        : m_icon(NULL),
        TA_Base_App::AbstractComponent(componentName, new AtsPlatformDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ATS) );
    }


    AtsPlatformComponent::~AtsPlatformComponent()
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


    HICON AtsPlatformComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AtsPlatformComponent::getComponentView()
    {
        return RUNTIME_CLASS(AtsPlatformFrameworkView);
    }


    std::string AtsPlatformComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
