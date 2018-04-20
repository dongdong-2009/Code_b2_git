/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/plan_tree/src/AgentPlanAccess.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// AgentPlanAccess.h : header file
//

#if !defined(AFX_AGENTPLANACCESS_H__0B586560_53A6_4221_863C_ED8FF12CDA7C__INCLUDED_)
#define AFX_AGENTPLANACCESS_H__0B586560_53A6_4221_863C_ED8FF12CDA7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "bus\response_plans\plan_tree\src\IPlanAccess.h"
#include "bus\response_plans\plan_tree\src\PlanFilter.h"


namespace TA_Base_Bus
{
    // Forwards
    class PlanAgentLibrary;


    /////////////////////////////////////////////////////////////////////////////
    // AgentPlanAccess

    class AgentPlanAccess : public IPlanAccess
    {
    // Operations
    public:
	    AgentPlanAccess(PlanFilter::PlanFilterType planFilterType);
	    AgentPlanAccess(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans);
	    virtual ~AgentPlanAccess();

        // IPlanAccess interface
        virtual IPlanAccess::PlanNodeList getRootChildren();
        virtual IPlanAccess::PlanNodeList getChildren(TA_Base_Core::NodeId categoryId);
        virtual IPlanAccess::PlanIdList getNodeIdsInPlanPath(TA_Base_Core::NodeId planId);
        virtual std::string getPlanPath(TA_Base_Core::NodeId planId);
        virtual TA_Base_Core::NodeId getPlanId(const std::string& planPath);

    private:
        AgentPlanAccess(const AgentPlanAccess& planAccess);
        AgentPlanAccess& operator=(const AgentPlanAccess& planAccess);

        void initialisePlanAgentLib();


    // Attributes
    private:
        PlanAgentLibrary* m_planAgentLib;
        PlanFilter m_planFilter;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_AGENTPLANACCESS_H__0B586560_53A6_4221_863C_ED8FF12CDA7C__INCLUDED_)
