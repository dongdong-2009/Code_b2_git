/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/OpcServerAgentEntityData.cpp $
  * @author:  Dhanshri Mokashi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: Dhanshri Mokashi $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/OpcServerAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	
    const std::string OpcServerAgentEntityData::ENTITY_TYPE = "OpcServerAgent";
   
    OpcServerAgentEntityData::OpcServerAgentEntityData()
    {}

    OpcServerAgentEntityData::OpcServerAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

    void OpcServerAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    OpcServerAgentEntityData::~OpcServerAgentEntityData()
    {
    }

    
    std::string OpcServerAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string OpcServerAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    TA_Base_Core::IEntityData* OpcServerAgentEntityData::clone(unsigned long key)
    {
        return( new OpcServerAgentEntityData(key) );
    }

}
