/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#if !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
#define AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class StationStepParameter;
    class TisPidListStepParameter;
    class TisMessageStepParameter;
    class TisPriorityStepParameter;
    class TimeSpanStepParameter;
    class TextualStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // StationTisStep

    class StationTisStep : public PlanStep
    {
    // Operations
    public:
        StationTisStep(PlanNode* parent, const TA_Base_Core::StepDetail& stepDetail);
        StationTisStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~StationTisStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const  { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        StationStepParameter* getStationParameter()       { return m_stationParameter; }
        TextualStepParameter* getStationLevelParameter()  { return m_stationLevelParameter; }
        TisPidListStepParameter* getPidListParameter()    { return m_pidListParameter; }
        TisMessageStepParameter* getMessageParameter()    { return m_messageParameter; }
        TisPriorityStepParameter* getPriorityParameter()  { return m_priorityParameter; }
        TimeSpanStepParameter* getDurationParameter()     { return m_durationParameter; }

    protected:
        StationTisStep(const StationTisStep& other);

        virtual void createPlanStepParameters(PlanStepParameterList& stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        StationTisStep& operator=(const StationTisStep& other);


    // Attributes
    private:
        StationStepParameter*     m_stationParameter;
        TextualStepParameter*     m_stationLevelParameter;
        TisPidListStepParameter*  m_pidListParameter;
        TisMessageStepParameter*  m_messageParameter;
        TisPriorityStepParameter* m_priorityParameter;
        TimeSpanStepParameter*    m_durationParameter;
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
