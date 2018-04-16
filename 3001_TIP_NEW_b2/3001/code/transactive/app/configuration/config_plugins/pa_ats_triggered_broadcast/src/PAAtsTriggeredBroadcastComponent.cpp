/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastComponent.cpp $
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

#pragma warning(disable:4786 4284)

#include <sstream>

#include "StdAfx.h"
#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString PaAtsTriggeredBroadcastComponent::DLL_NAME = "TA_PAAtsTriggeredBroadcastConfigPlugin.dll";
    const DWORD PaAtsTriggeredBroadcastComponent::RIGHTS_ACTION_ID = TA_Base_Bus::aca_CE_CONFIGURE_PA_ATS_TRIGGERED_BROADCASTS;
        
    PaAtsTriggeredBroadcastComponent::PaAtsTriggeredBroadcastComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new PaAtsTriggeredBroadcastDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_PA_ATS_TRIGGERED_BROADCASTS) );
    }


    PaAtsTriggeredBroadcastComponent::~PaAtsTriggeredBroadcastComponent()
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


    HICON PaAtsTriggeredBroadcastComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* PaAtsTriggeredBroadcastComponent::getComponentView()
    {
        return RUNTIME_CLASS(PaAtsTriggeredBroadcastFrameworkView);
    }


    std::string PaAtsTriggeredBroadcastComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
