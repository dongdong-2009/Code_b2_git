/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2014/10/16 13:13:15 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#if !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
#define AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/common/StationTisStepParameter.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/StationTisStepEditorParameter.h"


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
        StationTisStep(IPlanNode* parent, BasicParameter& stepDetail);
        StationTisStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~StationTisStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const  { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        StationStepParameter* getStationParameter()       { return m_stationParameter; }
        TextualStepParameter* getStationLevelParameter()  { return m_stationLevelParameter; }
        TisPidListStepParameter* getPidListParameter()    { return m_pidListParameter; }
        TisMessageStepParameter* getMessageParameter()    { return m_messageParameter; }
        TisPriorityStepParameter* getPriorityParameter()  { return m_priorityParameter; }
        TimeSpanStepParameter* getDurationParameter()     { return m_durationParameter; }

		virtual IActivityParameter& getActivityParameter();
    protected:
        StationTisStep(const StationTisStep& other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
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

		StationTisStepParameter m_params;
		StationTisStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_STATIONTISSTEP_H__60E39874_7371_4B6C_9964_266A0BEE2706__INCLUDED_)
