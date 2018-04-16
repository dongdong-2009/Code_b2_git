/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/tis_ratis_tag/src/TisRatisTagComponent.cpp $
  * @author:  James
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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
    // TUpdate: Sets constants as required
    // This is the name of the DLL and is used to load resources correctly.
    const CString TisRatisTagComponent::DLL_NAME = "TA_TisRatisTagConfigPlugin.dll";
    const DWORD TisRatisTagComponent::RIGHTS_ACTION_ID = TA_Base_Bus::aca_CE_CONFIGURE_TIS_RATIS_TAGS;
        
    TisRatisTagComponent::TisRatisTagComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new TisRatisTagDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_TIS_RATIS_TAGS) );
    }


    TisRatisTagComponent::~TisRatisTagComponent()
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


    HICON TisRatisTagComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* TisRatisTagComponent::getComponentView()
    {
        return RUNTIME_CLASS(TisRatisTagFrameworkView);
    }


    std::string TisRatisTagComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
