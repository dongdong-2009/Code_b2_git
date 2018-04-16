/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanInfoCache.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/02/20 18:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Implementation of PlanInfoCache.
*/


#ifndef PLAN_INFO_CACHE_H
#define PLAN_INFO_CACHE_H

#include <map>
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "boost/shared_ptr.hpp"

#include "ace/Singleton.h"

namespace TA_Base_App
{

	struct PlanStepInfo
	{
		boost::shared_ptr<TA_Base_Core::NodeDetail> nodeDetails;
		boost::shared_ptr<TA_Base_Core::Steps> stepDetails;
		boost::shared_ptr<TA_Base_Core::SequenceFlows> sequenceFlows;
	};
	typedef std::map<unsigned long, boost::shared_ptr<PlanStepInfo>>::iterator PlanCacheMapIterator;
	typedef std::map<unsigned long, boost::shared_ptr<PlanStepInfo>> PlanCacheMap;

	class PlanInfoCacheImpl
	{
	public:
		friend class ACE_Singleton<PlanInfoCacheImpl, ACE_Recursive_Thread_Mutex>;

		boost::shared_ptr<PlanStepInfo> findPlanInfo(unsigned long planId);

		void addPlanInfo(const unsigned long planId, boost::shared_ptr<PlanStepInfo> info);

	private:
		PlanInfoCacheImpl();
		~PlanInfoCacheImpl() {}

		PlanInfoCacheImpl(const PlanInfoCacheImpl&);
		PlanInfoCacheImpl& operator=(const PlanInfoCacheImpl&);

	private:
		PlanCacheMap m_mapPlanCache; // Cache map to store Plan & Step info based on Plan ID.
		mutable ACE_RW_Mutex m_mapPlanCacheLock;

	};

	typedef ACE_Singleton<PlanInfoCacheImpl, ACE_Recursive_Thread_Mutex> PlanInfoCache;
}

#endif // PLAN_INFO_CACHE_H