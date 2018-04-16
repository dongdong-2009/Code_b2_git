/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithm/common/PCAStepParameter.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/25 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Step parameter class derived from based class MLStepParameter for PCA ML algorithm's parameters.
*/


#ifndef PCASTEPPARAMETER_H
#define PCASTEPPARAMETER_H

#include <string>

#include "bus/response_plans/PlanDataAccess/src/Serialization.h"
#include "bus/response_plans/PlanDataAccess/src/DataPointVectorPara.h"

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"
#include "app/response_plans/plan_step_plugin/active_ml/src/MLStepParameter.h"


namespace TA_Base_App
{
	
	class PCAStepParameter : public MLStepParameter
	{
	public:
		PCAStepParameter(const BasicParameter& detail) : MLStepParameter(detail)
		{
		}

		PCAStepParameter(const unsigned long stepId, const int stepType) : MLStepParameter(stepId, stepType)
		{
		}

		~PCAStepParameter() {}

	protected:
		virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
		{
			MLStepParameter::serializeImp(ioSerialization);
		}

	private:
		//PCAStepParameter(const PCAStepParameter&);
		PCAStepParameter& operator=(const PCAStepParameter&);
	};
} // TA_Base_App

#endif // PCASTEPPARAMETER_H