/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/DataMatrixPara.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/24 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Predefined common ML Algorithm's step parameter class for use with ML steps that includes serializable vector matrix type variable.
*/


#ifndef DATAMATRIXPARA_H
#define DATAMATRIXPARA_H

#include <vector>
#include <string>

#include "bus/response_plans/PlanDataAccess/src/MLInOutParameter.h"

namespace TA_Base_Bus
{
	namespace ML_Plan
	{
		template <class T>
		class DataMatrixPara :
			public MLInOutParameter
		{
		public:
			DataMatrixPara() : MLInOutParameter() {}
			DataMatrixPara(std::string name, Inout inOut, size_t colCount, size_t rowCount)
				: MLInOutParameter(name, inOut, DATA_MATRIX, MATRIX, dataTypeStr<T>()), matrix(colCount), m_rowCount(rowCount), m_colCount(colCount)
			{
				for (typename std::vector<Row>::iterator it = matrix.begin(); it != matrix.end(); it++)
				{
					std::vector<Row> row = *it;
					row.resize(rowCount);
					for (typename std::vector<T>::iterator it = row.begin(); it != row.end(); it++)
					{
						*it = 0;
					}
				}

				std::ostringstream stm;
				stm << colCount << ',' << rowCount;
				Size = stm.str();
			}

			DataMatrixPara(size_t colCount, size_t rowCount, const T& value) :
				matrix(colCount),
				m_rowCount(rowCount),
				m_colCount(colCount)
			{
				for (typename std::vector<Row>::iterator it = matrix.begin(); it != matrix.end(); it++)
				{
					std::vector<Row> row = *it;
					row.resize(rowCount, value);
				}

				std::ostringstream stm;
				stm << colCount << ',' << rowCount;
				Size = stm.str();
			}

			typedef std::vector<T> Row;
			std::vector<Row> matrix;
			size_t m_rowCount;
			size_t m_colCount;
		};
	}
	// Macro define serialization for iterating through the DataMatrixPara<int> common member variables
	TA_SERIALIZATION(ML_Plan::DataMatrixPara<int>, (Name)(InOut)(ObjectType)(StructType)(DataType)(Size))
	// Macro define serialization for iterating through the DataMatrixPara<double> common member variables
	TA_SERIALIZATION(ML_Plan::DataMatrixPara<double>, (Name)(InOut)(ObjectType)(StructType)(DataType)(Size))
	// Macro define serialization for iterating through the DataMatrixPara<float> common member variables
	TA_SERIALIZATION(ML_Plan::DataMatrixPara<float>, (Name)(InOut)(ObjectType)(StructType)(DataType)(Size))
	// Macro define serialization for iterating through the DataMatrixPara<std::string> common member variables
	TA_SERIALIZATION(ML_Plan::DataMatrixPara<std::string>, (Name)(InOut)(ObjectType)(StructType)(DataType)(Size))
} // TA_Base_App

#endif // DATAMATRIXPARA_H