/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/plan_tree/src/PlanFilter.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanFilter.h : header file
//

#if !defined(AFX_PLANFILTER_H__F987C7C3_3A39_42D8_A5C4_9607CDC5D997__INCLUDED_)
#define AFX_PLANFILTER_H__F987C7C3_3A39_42D8_A5C4_9607CDC5D997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanFilter

    class PlanFilter
    {
    // Types
    public:
        typedef enum { PFT_ALL_PLANS, PFT_APPROVED_PLANS } PlanFilterType;


    // Construction/destruction
    public:
        PlanFilter();
	    PlanFilter(PlanFilterType planFilterType);
	    PlanFilter(PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterableNodes);
	    ~PlanFilter();


    // Operations
    public:
        void setPlanFilterType(PlanFilterType planFilterType);
        PlanFilterType getPlanFilterType() const;

        bool isRelevantNode(const TA_Base_Core::NodeSummary& nodeSummary) const;
        bool isFilterableNode(TA_Base_Core::NodeId nodeId) const;

    private:
        PlanFilter(const PlanFilter& planFilter);
        PlanFilter& operator=(const PlanFilter& planFilter);


    // Attributes
    private:
        PlanFilterType m_planFilterType;
        std::vector<TA_Base_Core::NodeId> m_nonFilterableNodes;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANFILTER_H__F987C7C3_3A39_42D8_A5C4_9607CDC5D997__INCLUDED_)
