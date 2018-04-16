//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationPAStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_)
#define AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "PAZonesStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/station_pa/common/StationPaStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/StationPaStepEditorParameter.h"

namespace TA_Base_App
{

        class StationStepParameter;
        class PaMessageStepParameter;
        class NumericStepParameter;
        class YesNoStepParameter;
        //class PAZonesStepParameter;
		class TimeSpanStepParameter;
      /////////////////////////////////////////////////////////////////////////////
    // StationPAStep

    class StationPAStep : public PlanStep
    {
    // Operations
    public:
        StationPAStep(IPlanNode* parent, BasicParameter& stepDetail);
        StationPAStep(IPlanNode* parent, const unsigned long stepId, const int stepType);
        virtual ~StationPAStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        StationStepParameter* getStationDestinationParameter() { return m_station; }
        PaZonesStepParameter* getPaZonesParameter(){ return m_zones; }
        PaMessageStepParameter* getPaMessageParameter(){ return m_message; }
        YesNoStepParameter* getChimeParameter(){ return m_chime; }
        YesNoStepParameter* getCyclicParameter(){ return m_cyclic; }
        YesNoStepParameter* getTISSynch(){ return m_TISSynch; }
        TimeSpanStepParameter* getMessageDuration(){ return m_duration;}
        NumericStepParameter* getMessageInterval(){ return m_interval;}

		virtual IActivityParameter& getActivityParameter();
    protected:
        StationPAStep(const StationPAStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        StationPAStep &operator=(const StationPAStep &other);


    // Attributes
    private:
        StationStepParameter*    m_station;
        PaZonesStepParameter*    m_zones;
        PaMessageStepParameter*    m_message;
        YesNoStepParameter*        m_chime;
        YesNoStepParameter*        m_cyclic;
        YesNoStepParameter*        m_TISSynch;
        TimeSpanStepParameter*    m_duration;
        NumericStepParameter*    m_interval;

		StationPaStepParameter m_params;

		StationPaStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_STATIONPASTEP_H__5706B830_68AE_430A_B32A_DAE44C6B600F__INCLUDED_)
