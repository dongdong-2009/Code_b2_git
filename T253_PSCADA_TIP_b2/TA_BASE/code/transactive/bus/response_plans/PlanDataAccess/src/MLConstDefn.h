/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLConstDefn.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/21 15:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Global constants and enum used by MLA steps.
*/


#ifndef MLCONSTDEFN_H
#define MLCONSTDEFN_H

namespace TA_Base_Bus
{
	namespace ML_Plan
	{
		const char* const NAME = "Name";
		const char* const INOUT = "InOut";
		const char* const OBJECT_TYPE = "ObjectType";
		const char* const STRUCT_TYPE = "StructType";
		const char* const DATA_TYPE = "DataType";
		const char* const DATA_SIZE = "Size";

		const char* const DATAPOINT_VECTOR = "DataPointVectorPara";
		const char* const DATA_MATRIX = "DataMatrixPara";

		const char* const DATAPOINTS = "DataPoints";
		const char* const MATRIX = "Matrix";

		const char* const INT = "int";
		const char* const UINT = "uint";
		const char* const LONG = "long";
		const char* const ULONG = "ulong";
		const char* const DOUBLE = "double";
		const char* const FLOAT = "float";
		const char* const STRING = "string";

		typedef enum
		{
			IN_PARAM,
			OUT_PARAM
		} Inout;
	} // ML_Plan
} // TA_Base_Bus

#endif // MLCONSTDEFN_H