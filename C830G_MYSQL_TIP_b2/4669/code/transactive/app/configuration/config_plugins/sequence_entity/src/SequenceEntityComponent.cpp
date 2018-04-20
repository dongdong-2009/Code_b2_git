/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/sequence_entity/src/SequenceEntityComponent.cpp $
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
    const CString SequenceEntityComponent::DLL_NAME = "TA_GenericEntityConfigPlugin.dll";


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
