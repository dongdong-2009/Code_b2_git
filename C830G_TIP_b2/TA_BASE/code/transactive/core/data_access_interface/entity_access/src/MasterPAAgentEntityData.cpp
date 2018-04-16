/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/MasterPAAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * MasterPAAgentEntityData implements the interface IMasterPAAgentEntityData to provide read-only access
  * to MasterPAAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/MasterPAAgentEntityData.h"

namespace TA_Base_Core
{

    const std::string MasterPAAgentEntityData::ENTITY_TYPE = "MasterPAAgent";

	MasterPAAgentEntityData::MasterPAAgentEntityData()
    {
    }

	
    MasterPAAgentEntityData::MasterPAAgentEntityData ( unsigned long key )
	: EntityData( key, getType() )
    {
    }

    MasterPAAgentEntityData::~MasterPAAgentEntityData()
    {
    }


    std::string MasterPAAgentEntityData::getType()
    {
        return getStaticType();
    }


    std::string MasterPAAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


} //close namespace TA_Base_Core
