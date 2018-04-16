/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithm/active/src/MLAlgorithmStepComponent.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/24 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Implementation to handle multiple ML Steps with a single DLL.
*/


#include "app/response_plans/plan_step_plugin/ml_algorithm/active/src/MLAlgorithmStepComponent.h"

#include "app/response_plans/plan_step_plugin/ml_algorithm/active/src/ActivePCAStep.h"

namespace TA_Base_App
{
	typedef enum
	{
		ML_PCA = 200,
		ML_KMEANS
	} StepTypes;

    ActiveStep* MLAlgorithmStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
		// Create the ML steps based on the step type retrieved from the database.
		switch (detail.type)
		{
		case ML_PCA:
			return new ActivePCAStep(detail, plan);
			break;
		case ML_KMEANS:
			//return new ActiveKMeansStep(detail, plan);
			break;
		}        
		return NULL;
    }

    void MLAlgorithmStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    MLAlgorithmStepComponentImpl::MLAlgorithmStepComponentImpl()
    {

    }

    MLAlgorithmStepComponentImpl::~MLAlgorithmStepComponentImpl()
    {

    }

}  // TA_Base_App