#ifndef __VMS_BLANK_OUT_STEP_COMPONENT_H_
#define __VMS_BLANK_OUT_STEP_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class IActivePlan;

    class VmsBlankoutStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<VmsBlankoutStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl);

        virtual void validateStepParameter( const std::vector<std::string>& parameters );

        /*virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );*/
        /*virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );*/

    private:
        VmsBlankoutStepComponentImpl();
        ~VmsBlankoutStepComponentImpl();
        VmsBlankoutStepComponentImpl( const VmsBlankoutStepComponentImpl& );
        VmsBlankoutStepComponentImpl& operator=( const VmsBlankoutStepComponentImpl& );
    };

    typedef ACE_Singleton<VmsBlankoutStepComponentImpl, ACE_Recursive_Thread_Mutex> VmsBlankoutStepComponent;
}

#endif