/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLStepParameter.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/22 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Base class used by ML Algorithms' step parameter classes to serialize and deserialize input and output parameters.
*/


#ifndef MLSTEPPARAMETER_H
#define MLSTEPPARAMETER_H

#include <string>

#include "bus/response_plans/PlanDataAccess/src/MLInOutParameter.h"

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/MLConstDefn.h"
#include "bus/response_plans/PlanDataAccess/src/MLSerialization.h"

namespace TA_Base_App
{
	class MLStepParameter : public BaseStepParameter
	{
	public:
		MLStepParameter(const BasicParameter& detail) : BaseStepParameter(detail),
			m_InParams(0u), m_OutParams(0u)
		{
		}

		MLStepParameter(const unsigned long stepId, const int stepType) : BaseStepParameter(stepId, stepType)
		{
		}

		~MLStepParameter() {}

	protected:

		virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
		{
			TA_Base_Bus::MLSerialization ioSzn(&ioSerialization);

			TA_Base_Bus::MLInOutParameter* param;

			if (ioSzn.isSerialize())
			{
				for (std::vector<TA_Base_Bus::MLInOutParameter*>::iterator it = m_InParams.begin(); it != m_InParams.end(); it++)
				{
					param = *it;
					if (NULL != *it)
					{
						ioSzn.serialize(*param);
					}
				}

				for (std::vector<TA_Base_Bus::MLInOutParameter*>::iterator it = m_OutParams.begin(); it != m_OutParams.end(); it++)
				{
					param = *it;
					if (NULL != *it)
					{
						ioSzn.serialize(*param);
					}
				}
			}
			else
			{
				int vecParamSize = ioSzn.getPosNum();
				for (int i = 0; i < vecParamSize; i++)
				{
					param = ioSzn.deserialize();
					if (param->InOut == TA_Base_Bus::ML_Plan::IN_PARAM)
					{
						m_InParams.push_back(param);
					}
					else if (param->InOut == TA_Base_Bus::ML_Plan::OUT_PARAM)
					{
						m_OutParams.push_back(param);
					}
				}
			}
		}

	private:
		//MLStepParameter(const MLStepParameter&);
		MLStepParameter & operator=(const MLStepParameter&);

	public:
		std::vector<TA_Base_Bus::MLInOutParameter*> m_InParams;
		std::vector<TA_Base_Bus::MLInOutParameter*> m_OutParams;
	};
}

#endif //MLSTEPPARAMETER_H