/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithm/active/src/MLAlgorithmStepComponent.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/24 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Implementation to handle multiple ML Steps with a single DLL.
*/


#ifndef ACTIVE_MLALGORITHM_STEP_COMPONENT_H
#define ACTIVE_MLALGORITHM_STEP_COMPONENT_H

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"

namespace TA_Base_App
{
    class MLAlgorithmStepComponentImpl : public StepComponent
    {

    public:
        friend class ACE_Singleton<MLAlgorithmStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, IActivePlan& plan );

        virtual void validateStepParameter( const std::vector<std::string>& parameters );

    private:
        MLAlgorithmStepComponentImpl();
        virtual ~MLAlgorithmStepComponentImpl();
        MLAlgorithmStepComponentImpl( const MLAlgorithmStepComponentImpl& );
        MLAlgorithmStepComponentImpl& operator=( const MLAlgorithmStepComponentImpl& );
    };

    typedef ACE_Singleton<MLAlgorithmStepComponentImpl, ACE_Recursive_Thread_Mutex> MLAlgorithmStepComponent;
} // TA_Base_App

#endif // ACTIVE_MLALGORITHM_STEP_COMPONENT_H