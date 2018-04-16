#ifndef PLANS_STRUCTURES_H_INCLUDED
#define PLANS_STRUCTURES_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanStructures.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  General include for common plans code.
  */

#include <map>
#include <vector>
#include <string>
#include "boost/shared_ptr.hpp"

#include "bus/response_plans/PlanDataAccess/src/DataPointVectorPara.h"

namespace TA_Base_Bus
{

	struct StepTypeInfo
	{
		unsigned long typeId; // type_value
		std::string typeName;
		int typeAccess;
		std::string serverDllName;
		std::string clientDllName;
	};

	struct StepShortcut
	{
		std::string icon;
		std::vector<int> shortcuts;
	};

	enum EMLParameterType
	{
		DATAPOINT_VECTOR = 50,
		DATA_MATRIX
	};

	struct PlanTemplateStepParameterInfo
	{
		unsigned long planId;
		unsigned long stepId;
		unsigned long stepPos;
		unsigned long type;
		unsigned long paramPos;

		boost::shared_ptr<ML_Plan::DataPointVectorPara> dataPointInfo;
	};

	typedef std::vector<boost::shared_ptr<PlanTemplateStepParameterInfo>> PlanTemplateStepParameterList;
	typedef std::vector<boost::shared_ptr<PlanTemplateStepParameterInfo>>::iterator PlanTemplateStepParameterIterator;

}

#endif // PLANS_STRUCTURES_H_INCLUDED
