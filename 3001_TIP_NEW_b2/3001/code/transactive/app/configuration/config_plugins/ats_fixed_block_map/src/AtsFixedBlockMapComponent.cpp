/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapComponent.cpp $
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

#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapComponent.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/resource.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapFrameworkView.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AtsFixedBlockMapComponent::DLL_NAME = "TA_AtsFixedBlockMapConfigPlugin.dll";


    AtsFixedBlockMapComponent::AtsFixedBlockMapComponent(const std::string& componentName)
        : m_icon(NULL),
         TA_Base_App::AbstractComponent(componentName, new AtsFixedBlockMapDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ATS) );
    }


    AtsFixedBlockMapComponent::~AtsFixedBlockMapComponent()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    HICON AtsFixedBlockMapComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AtsFixedBlockMapComponent::getComponentView()
    {
        return RUNTIME_CLASS(AtsFixedBlockMapFrameworkView);
    }


    std::string AtsFixedBlockMapComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
