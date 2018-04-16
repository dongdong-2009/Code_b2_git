/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanAgentAccess.h $
  * @author:  Bart Golab
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/12/03 13:46:05 $
  * Last modified by:  $Author: huangjian $
  *
  * Thread Safe Singleton to wrap access to the Plan Agent's CORBA Interface
  */

#if !defined(AFX_DLLPLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_)
#define AFX_DLLPLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "bus/response_plans/active_plans_display/src/PlanAgentMap.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/exceptions/src/RightsException.h"

namespace TA_Base_Core
{
    struct ActivePlanId;
    struct InteractiveStepData;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanAgentAccess

    class PlanAgentAccess
    {
    public:

        ~PlanAgentAccess();

        static PlanAgentAccess& getInstance();
        static void destroyInstance();

        TA_Base_Core::ActivePlanDetails *getActivePlanDetails(TA_Base_Core::NodeId planNodeId, unsigned long location);

    private:
        PlanAgentAccess();
        PlanAgentAccess(const PlanAgentAccess& planAgentAccess);
        PlanAgentAccess& operator=(const PlanAgentAccess& planAgentAccess);

    private:
        static PlanAgentAccess* m_thePlanAgentAccess;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    private:
        TA_Base_Bus::PlanAgentMap m_planAgentMap;
        std::string m_strSession;
    };


	#define PAA_BEGIN_TRY    try

	#define PAA_END_TRY(paaFn)                                                           \
	catch (const TA_Base_Core::ObjectResolutionException &ex)                                 \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught ObjectResolutionException: %s", ex.what());                          \
		throw;          \
	}                                                                                    \
	catch (const TA_Base_Core::TransactiveException &ex)                                      \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught TransactiveException: %s", ex.what());                               \
		throw;                                                                           \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError &ex)                         \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanAgentError: %s", ex.what.in());              \
		throw;                                                                           \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError &ex)                      \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanNotFound: %s", ex.what.in());                \
		throw;                                                                           \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::PlanStepDetailError &ex)                    \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::PlanStepDetailError: %s", ex.what.in());         \
		TA_THROW(TA_Base_Core::PlanStepDetailException(ex.what.in(), ex.detailType.in(), ex.errorType, ex.stepId)); \
	}                                                                                    \
	catch (const TA_Base_Bus::IPlanAgentCorbaDef::NoPermission &ex)                           \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught IPlanAgentCorbaDef::NoPermission: %s", ex.what.in());                \
		TA_THROW(TA_Base_Core::RightsException(ex.what.in()));                                \
	}                                                                                    \
	catch (const CORBA::Exception &ex)                                                   \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). "                                  \
			"Caught CORBA::Exception: %s",                                               \
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());            \
		TA_THROW(TA_Base_Core::TransactiveException(std::string("CORBA::Exception: ") +       \
				 TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));              \
	}                                                                                    \
	catch (...)                                                                          \
	{                                                                                    \
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,  \
			"Plan Agent access failure in "#paaFn"(). Unknown exception.");              \
		TA_THROW(TA_Base_Core::TransactiveException("Unknown error"));                        \
	}

} // namespace TA_Base_App

#endif // !defined(AFX_DLLPLANAGENTACCESS_H__A514BE67_87EF_46C0_A8BB_869EF214256A__INCLUDED_)
