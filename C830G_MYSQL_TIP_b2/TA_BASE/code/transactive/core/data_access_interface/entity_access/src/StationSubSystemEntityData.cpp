/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/StationSubSystemEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * StationSubSystemEntityData provides read-only access to Station Subsystem 
  * entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/StationSubSystemEntityData.h"

namespace TA_Base_Core
{

    const std::string StationSubSystemEntityData::ENTITY_TYPE = "StationSubSystem";

	StationSubSystemEntityData::StationSubSystemEntityData()
    {
    }

	
    StationSubSystemEntityData::StationSubSystemEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    StationSubSystemEntityData::~StationSubSystemEntityData()
    {
    }


	std::string StationSubSystemEntityData::getType()
	{
        return getStaticType();
	}

    
    std::string StationSubSystemEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    IEntityData* StationSubSystemEntityData::clone ( unsigned long key )
    {
        return new StationSubSystemEntityData ( key );
    }


    void StationSubSystemEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
