#ifndef ACTIVE_DATA_POINT_CHECK_STEP_INCLUDED_H
#define ACTIVE_DATA_POINT_CHECK_STEP_INCLUDED_H

#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "app/response_plans/plan_step_plugin/data_point_check/common/DataPointCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_App
{
    class ActiveDataPointCheckStep: public ActiveStep
    {
    public:
        ActiveDataPointCheckStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveDataPointCheckStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveDataPointCheckStep(const ActiveDataPointCheckStep&);
        ActiveDataPointCheckStep& operator=(const ActiveDataPointCheckStep&);

		template<class T> bool test(const T& lhs, const T& rhs, const int testType);
        bool isQualityGood( const int quality);
    private:
        DataPointCheckStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;
        TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef,
                     TA_Base_Bus::IDataPointAccessCorbaDef_ptr,
                     TA_Base_Bus::IDataPointAccessCorbaDef_var> m_dataPoint;

        unsigned long m_ulDpKey;
		unsigned long m_nextStepId;
    };
}


#endif //ACTIVE_DATA_POINT_CHECK_STEP_INCLUDED_H
