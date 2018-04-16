/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/camera_entity/src/CameraEntityComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/camera_entity/src/CameraEntityComponent.h"
#include "app/configuration/config_plugins/camera_entity/src/resource.h"
#include "app/configuration/config_plugins/camera_entity/src/EntityFrameworkView.h"
#include "app/configuration/config_plugins/camera_entity/src/EntityDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString CameraEntityComponent::DLL_NAME = "TA_CameraEntityConfigPlugin.dll";


    CameraEntityComponent::CameraEntityComponent(const std::string& componentName,
                                                   const std::vector<std::string>& entityTypes)
        : m_icon(NULL),
         AbstractComponent(componentName, new EntityDatabaseAccessor(entityTypes))
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ENTITY) );
    }


    CameraEntityComponent::~CameraEntityComponent()
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


    HICON CameraEntityComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* CameraEntityComponent::getComponentView()
    {
        return RUNTIME_CLASS(EntityFrameworkView);
    }


    std::string CameraEntityComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }

}
