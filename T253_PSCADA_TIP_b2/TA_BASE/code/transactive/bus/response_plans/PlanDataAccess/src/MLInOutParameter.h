/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLInOutParameter.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/10 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Base class with serializable common member variables used by ML Algorithms' step parameter classes.
*/

#ifndef MLINOUTPARAMETER_H
#define MLINOUTPARAMETER_H

#include <string>

#include "bus/response_plans/PlanDataAccess/src/Serialization.h"
#include "bus/response_plans/PlanDataAccess/src/MLConstDefn.h"


namespace TA_Base_Bus
{
	class MLInOutParameter
	{
	public:

		MLInOutParameter() : Name(""), InOut(ML_Plan::IN_PARAM), ObjectType(""), StructType(""), DataType(""), Size("") {}
		MLInOutParameter(const std::string& name, const ML_Plan::Inout inOut, const std::string& ObType, const std::string& structType, const std::string& dataType) :
			Name(name), InOut(inOut), ObjectType(ObType), StructType(structType), DataType(dataType), Size("")
		{
		}
		virtual ~MLInOutParameter() {};

		template <typename T>
		std::string dataTypeStr()
		{
			std::string dataType = "";

			if (is_same<T, int>::value)
			{
				dataType = "int";
			}
			else if (is_same<T, unsigned int>::value)
			{
				dataType = "uint";
			}
			else if (is_same<T, long>::value)
			{
				dataType = "long";
			}
			else if (is_same<T, unsigned long>::value)
			{
				dataType = "ulong";
			}
			else if (is_same<T, double>::value)
			{
				dataType = "double";
			}
			else if (is_same<T, float>::value)
			{
				dataType = "float";
			}
			else if (is_same<T, std::string>::value)
			{
				dataType = "string";
			}

			return dataType;
		};

	private:
		//MLInOutParameter(const MLInOutParameter&);
		MLInOutParameter & operator=(const MLInOutParameter&);

	public:
		std::string Name;
		int InOut;
		std::string ObjectType;
		std::string StructType;
		std::string DataType;
		std::string Size;
	};		
} // TA_Base_App

#endif // MLINOUTPARAMETER_H

