/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanNode.h $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/07/17 15:35:04 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is a representation of a plan node within the plan tree.
  *
  **/

#ifndef I_PLAN_NODE_INCLUDE_H
#define I_PLAN_NODE_INCLUDE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include <set>

namespace TA_Base_App
{
    class PlanStep;
    typedef std::set<unsigned int> UintSet;

    /////////////////////////////////////////////////////////////////////////////
    // PlanNode

    class IPlanNode
    {
    // Operations
    public:
        // Get plan step details
        virtual PlanStep* getPlanStepByStepId( unsigned long stepId ) = 0;
        // Plan editing
        virtual bool isCopiedPlan() = 0;

		virtual std::vector<PlanStep*> getPlanSteps() = 0;
        virtual std::map<unsigned int, UintSet> getSequenceFlows() = 0;
		virtual unsigned long getPlanNodeId() = 0;
    };

} // namespace TA_Base_App

#endif // !defined(I_PLAN_NODE_INCLUDE_H)
