/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/configuration/src/Component.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * This class implements the IComponent interface and will retrieve the data
  * for a single entry from the CO_COMPONENT table for the Configuration Editor.
  * This is used in the Configuration Editor so does not need to have an invalidate
  * method. This is because the Configuration Editor does not perform online udpates.
  * If it needs to in the future it would need to be added in.
  */

#include "core\data_access_interface\configuration\src\Component.h"

namespace TA_Base_Core
{

    Component::Component(unsigned long key,
                         const std::string& category,
                         const std::string& component,
                         const std::string& library,
                         const std::string& help,
                 const std::string& entityType)
     : m_category(category),
       m_component(component),
       m_library(library),
       m_helpPath(help),
       m_entityType(entityType)
    {

    }


    Component::~Component()
    {

    }


} //end namespace TA_Base_Core
