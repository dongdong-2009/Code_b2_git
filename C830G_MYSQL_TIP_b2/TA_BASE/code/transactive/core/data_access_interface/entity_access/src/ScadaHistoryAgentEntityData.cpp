/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/ScadaHistoryAgentEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "ScadaHistoryAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"



namespace TA_Base_Core
{

    const std::string ScadaHistoryAgentEntityData::ENTITY_TYPE                  = "DataRecordingAgent";
    const std::string ScadaHistoryAgentEntityData::MAXTRENDDATAPOINTSPERSTATION = "MaxTrendDatapointsPerStation";

    
    std::string ScadaHistoryAgentEntityData::getType()
    {
        return getStaticType();
    }
   
    std::string ScadaHistoryAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    void ScadaHistoryAgentEntityData::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        getHelper()->invalidate();
    }


    int ScadaHistoryAgentEntityData::getMaxTrendDatapointsPerStation()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
		std::string value = getHelper()->getParameter(MAXTRENDDATAPOINTSPERSTATION);
		int maxTrendDatapointsPerStation = 0;
		if (!value.empty())
		{
			maxTrendDatapointsPerStation = EntityHelper::getIntegerData(value);
		}

        return maxTrendDatapointsPerStation;
    }


} // TA_Base_Core
