/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/DataPointVectorPara.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/24 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Predefined common ML Algorithm's step parameter class for use with ML steps that includes serializable vector of datapoint type variable.
*/


#ifndef DATAPOINTVECTORPARA_H
#define DATAPOINTVECTORPARA_H

#include <vector>

#include "bus/response_plans/PlanDataAccess/src/MLInOutParameter.h"

namespace TA_Base_Bus
{
	struct DataPoint
	{
		long key;
		long interval;
		DataPoint() : key(0), interval(0) {}
	};
	// Macro define serialization for iterating through the struct DataPoint's member variables
	TA_SERIALIZATION(DataPoint, (key)(interval))		
}

namespace TA_Base_Bus
{
	namespace ML_Plan
	{
		class DataPointVectorPara :
			public MLInOutParameter
		{
		public:
			DataPointVectorPara() : MLInOutParameter() {}
			DataPointVectorPara(std::string name, Inout inOut, size_t size)
				: MLInOutParameter(name, inOut, DATAPOINT_VECTOR, DATAPOINTS, dataTypeStr<long>())
			{
				m_DataPt.resize(size);
				for (typename std::vector<DataPoint>::iterator it = m_DataPt.begin(); it != m_DataPt.end(); it++)
				{
					DataPoint dp = *it;
					dp.key = 0;
					dp.interval = 0;
				}

				std::ostringstream stm;
				stm << size;
				Size = stm.str();
			}

			virtual ~DataPointVectorPara() {};

			void addDataPt(DataPoint& dp)
			{
				m_DataPt.push_back(dp);
			}

		private:
			//DataPointVectorPara(const DataPointVectorPara&);
			DataPointVectorPara & operator=(const DataPointVectorPara&);

		public:
			std::vector<DataPoint> m_DataPt;

		};
	}
	// Macro define serialization for iterating through the DataPointVectorPara common member variables
	TA_SERIALIZATION(ML_Plan::DataPointVectorPara, (Name)(InOut)(ObjectType)(StructType)(DataType)(Size))

} // TA_Base_App

#endif // DATAPOINTVECTORPARA_H