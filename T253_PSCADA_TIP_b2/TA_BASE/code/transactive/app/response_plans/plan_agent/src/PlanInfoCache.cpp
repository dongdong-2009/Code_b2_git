/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanInfoCache.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/02/20 18:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Implementation of PlanInfoCache.
*/


#include "app/response_plans/plan_agent/src/PlanInfoCache.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{

	PlanInfoCacheImpl::PlanInfoCacheImpl()
	{
	}

	boost::shared_ptr<PlanStepInfo>  PlanInfoCacheImpl::findPlanInfo(unsigned long planId)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mapPlanCacheLock);
		
		boost::shared_ptr<PlanStepInfo> retVal(0);
		PlanCacheMapIterator it = m_mapPlanCache.find(planId);
		if (it != m_mapPlanCache.end())
		{
			retVal = it->second;
		}
		return retVal;
	}

	void PlanInfoCacheImpl::addPlanInfo(const unsigned long planId, boost::shared_ptr<PlanStepInfo> info)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mapPlanCacheLock);

		m_mapPlanCache.insert(PlanCacheMap::value_type(planId, info));
	}
}
