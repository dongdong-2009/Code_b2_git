/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepMap.h $
  * @author:  Bart Golab
  * @version: $Revision: #14 $
  *
  * Last modification: $DateTime: 2015/08/12 19:22:07 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#if !defined(AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_)
#define AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "SynchronisedMap.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class PlanStep;
    class PlanNode;


    /////////////////////////////////////////////////////////////////////////////
    // PlanStep

    class PlanStepMap
    {
    // Operations
    public:
        PlanStepMap(PlanNode* parent, const TA_Base_Core::StepDetails &stepDetails);
        virtual ~PlanStepMap();

        void getPlanSteps(PlanStepList &planSteps);
        PlanStep* getPlanStepByPos( unsigned long stepPos );
        PlanStep* getPlanStepById( unsigned long stepId );

        void getAdjacentSteps( const unsigned long stepPos, PlanStep*& pInStep, PlanStep*& pOutStep );
        void insertPlanStep( const int stepType, const unsigned long position, const unsigned long id );
        void deletePlanStep( const unsigned long id );
        unsigned long deletePlanSteps( PlanStepList &deleteSteps );
        void copyPlanSteps( const PlanStepList& copySteps, const unsigned long position, std::vector<unsigned int>& newStepIds );
        unsigned long movePlanSteps( const PlanStepList &moveSteps, unsigned long position );
        unsigned long getLastStepPosition();

        unsigned long getNewId();
        unsigned long getNewPostion();

    protected:
        PlanStep* createPlanStep( const TA_Base_Core::StepDetail& refDetail );
        PlanStep* createNewPlanStep( const int stepType, PlanStepId stepId );

    private:
        PlanStepMap(const PlanStepMap &other);
        PlanStepMap &operator=(const PlanStepMap &other);

    // Attributes
    private:
        SynchronisedMap<unsigned long, PlanStep*> m_planStepIdMap;
        SynchronisedMap<unsigned long, PlanStep*> m_planStepPosMap;

        PlanNode* m_parent;
    };
}

#endif // !defined(AFX_PLANSTEPMAP_H__7D11B3AB_F456_49BF_8BD9_807BD007CF24__INCLUDED_)
