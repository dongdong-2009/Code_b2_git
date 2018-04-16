/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/TrainAgentEntityData.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * TrainAgentEntityData provide read-only access to TrainAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"

#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string TrainAgentEntityData::ENTITY_TYPE     = "TrainAgent";
    const std::string TrainAgentEntityData::ASSET_NAME = "AssetName";
    const std::string TrainAgentEntityData::AgentOrigin = "AgentOrigin";


	TrainAgentEntityData::TrainAgentEntityData()
    {
    }


    TrainAgentEntityData::TrainAgentEntityData ( unsigned long key )
	    : EntityData( key, getStaticType() )
    {
    }


    TrainAgentEntityData::~TrainAgentEntityData()
    {
    }


	std::string TrainAgentEntityData::getType()
	{
		return getStaticType();
	}


    std::string TrainAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* TrainAgentEntityData::clone ( unsigned long key )
    {
        return new TrainAgentEntityData ( key );
    }


    void TrainAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


	std::string TrainAgentEntityData::getAssetName()
	{
        // Get the parameters for this entity
		return getHelper()->getParameter(ASSET_NAME);
	}

    unsigned long TrainAgentEntityData::getAgentOrign()
	{
        // Get the parameters for this entity
        std::string value = getHelper()->getParameter(AgentOrigin);
		if (!value.empty())
		{
            return EntityHelper::getUnsignedLongData( value );
		}

        TA_THROW( DataException( "Agent Origin Not defined", DataException::NO_VALUE, "" ) );

		return 0;
	}

} //close namespace TA_Base_Core
