#ifndef ACTIVE_DATA_POINT_TIME_CHECK_STEP_INCLUDED_H
#define ACTIVE_DATA_POINT_TIME_CHECK_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/data_point_time_check/common/DataPointTimeCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"

namespace TA_Base_Core
{
    struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveDataPointTimeCheckStep: public ActiveStep
    {
    public:
        ActiveDataPointTimeCheckStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveDataPointTimeCheckStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveDataPointTimeCheckStep(const ActiveDataPointTimeCheckStep&);
        ActiveDataPointTimeCheckStep& operator=(const ActiveDataPointTimeCheckStep&);
    private:
        TA_Base_App::DataPointTimeCheckStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

        TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef,
                     TA_Base_Bus::IDataPointAccessCorbaDef_ptr,
                     TA_Base_Bus::IDataPointAccessCorbaDef_var> m_dataPoint;

        unsigned long m_ulDpKey;
        bool m_bLastQualityGood;

		unsigned long m_nextStepId;
    };

}


#endif //ACTIVE_DATA_POINT_TIME_CHECK_STEP_INCLUDED_H
