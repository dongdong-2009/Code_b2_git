/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/PlanAgentData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Implementataion of PlanAgentData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/PlanAgentData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string PlanAgentData::ENTITY_TYPE = "PlanAgent";
    const std::string PlanAgentData::ACTIVEPLAN_EXPIRYTIME = "ActivePlanExpiryTime";
    const std::string PlanAgentData::ACTIVEPLAN_THREAD_POOL_SIZE = "ActivePlanThreadPoolSize";
	const std::string PlanAgentData::ACTIVEPLAN_PAUSED_SLEEP_TIME = "ActivePlanPausedSleepTime";
	const std::string PlanAgentData::ASSET_NAME = "AssetName";

    PlanAgentData::PlanAgentData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }


    PlanAgentData::~PlanAgentData()
    { // Nothing to clean up
    }


    IEntityData* PlanAgentData::clone(unsigned long key)
    {
        return new PlanAgentData(key);        
    }


    void PlanAgentData::invalidate()
    {
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		getHelper()->invalidate();
    }


    int PlanAgentData::getActivePlanExpiryTime()
    {
		int activePlanExpiryTime = 0;
		std::string value = getHelper()->getParameter(ACTIVEPLAN_EXPIRYTIME);
		if (!value.empty())
		{
			activePlanExpiryTime = EntityHelper::getIntegerData(value);
		}

        return activePlanExpiryTime;
    }


    int PlanAgentData::getActivePlanThreadPoolSize()
    {
		int activePlanThreadPoolSize = 100;
		std::string value = getHelper()->getParameter(ACTIVEPLAN_THREAD_POOL_SIZE);
		if (!value.empty())
		{
			activePlanThreadPoolSize = EntityHelper::getIntegerData(value);
		}
        return activePlanThreadPoolSize;
    }


	unsigned long PlanAgentData::getActivePlanPausedSleepTime()
    {
		unsigned long activePlanPausedSleepTime = 10000;
		std::string value = getHelper()->getParameter(ACTIVEPLAN_PAUSED_SLEEP_TIME);
		if (!value.empty())
		{
			activePlanPausedSleepTime = EntityHelper::getUnsignedLongData(value);
		}
        return activePlanPausedSleepTime;
    }


	std::string PlanAgentData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }


    std::string PlanAgentData::getType()
    {
        return getStaticType();
    }


    std::string PlanAgentData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
