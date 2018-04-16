/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/DefaultEntity.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * This is an implemntation of the EntityData abstract class and is the "base" or "default"
 * entity. Any entity in the system can be instantiated as one of these.
 */

#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    std::string DefaultEntity::getType()
    {
        return getHelper()->getEntityType();
    }


    std::string DefaultEntity::getParameterValue(const std::string& name)
    {        
        return getHelper()->getParameter(name);
    }


    void DefaultEntity::invalidate()
    {
        getHelper()->invalidate();
    }


} // TA_Base_Core
