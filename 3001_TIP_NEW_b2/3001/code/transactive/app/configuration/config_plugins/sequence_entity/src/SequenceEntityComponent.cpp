/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/sequence_entity/src/SequenceEntityComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/07 19:01:19 $
  * Last modified by:  $Author: hongzhi.zhang $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/sequence_entity/src/SequenceEntityComponent.h"
#include "app/configuration/config_plugins/sequence_entity/src/resource.h"
#include "app/configuration/config_plugins/sequence_entity/src/EntityFrameworkView.h"
#include "app/configuration/config_plugins/sequence_entity/src/EntityDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString SequenceEntityComponent::DLL_NAME = "TA_SequenceEntityConfigPlugin.dll";


    SequenceEntityComponent::SequenceEntityComponent(const std::string& componentName,
                                                   const std::vector<std::string>& entityTypes)
        : m_icon(NULL),
         AbstractComponent(componentName, new EntityDatabaseAccessor(entityTypes))
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ENTITY) );
    }


    SequenceEntityComponent::~SequenceEntityComponent()
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


    HICON SequenceEntityComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* SequenceEntityComponent::getComponentView()
    {
        return RUNTIME_CLASS(EntityFrameworkView);
    }


    std::string SequenceEntityComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }

}
