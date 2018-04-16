/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsComponent.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

// ITSIGroupsComponent.cpp: implementation of the ITSIGroupsComponent class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/radio_itsi_groups/src/stdafx.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsComponent.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/resource.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsFrameworkView.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString ITSIGroupsComponent::DLL_NAME = "TA_RadioITSIGroupsConfigPlugin.dll";


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	ITSIGroupsComponent::ITSIGroupsComponent(const std::string& componentName)
        : m_icon(NULL),
         TA_Base_App::AbstractComponent(componentName, new ITSIGroupsDatabaseAccessor())
	{
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_PROFILE) );
	}

	ITSIGroupsComponent::~ITSIGroupsComponent()
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

    HICON ITSIGroupsComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* ITSIGroupsComponent::getComponentView()
    {
        return RUNTIME_CLASS(ITSIGroupsFrameworkView);
    }


    std::string ITSIGroupsComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }

} //end of namespace
