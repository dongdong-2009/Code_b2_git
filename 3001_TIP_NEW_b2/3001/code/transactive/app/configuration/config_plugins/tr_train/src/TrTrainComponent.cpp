/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainComponent.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786 4284)

#include <sstream>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"

#include "bus/resources/resource.h"


#include "app/configuration/config_plugins/tr_train/src/TrTrainComponent.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainFrameworkView.h"


using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString TrTrainComponent::DLL_NAME = "TA_TRTrainConfigPlugin.dll";
    const DWORD TrTrainComponent::RIGHTS_ACTION_ID = TA_Base_Bus::aca_CE_CONFIGURE_PA_ATS_TRIGGERED_BROADCASTS;
        
    TrTrainComponent::TrTrainComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new TrTrainDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SI_TRAIN) );
    }


    TrTrainComponent::~TrTrainComponent()
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


    HICON TrTrainComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* TrTrainComponent::getComponentView()
    {
        return RUNTIME_CLASS(TrTrainFrameworkView);
    }


    std::string TrTrainComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
