/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLSerialization.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/21 15:03:08 $
* Last modified by:  $Author: hoa.le $
*
* This is class includes serialization methods for ML Algorithms' step parameters.
*/


#ifndef MLSERIALIZATION_H
#define MLSERIALIZATION_H

#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

#include "bus/response_plans/PlanDataAccess/src//MLConstDefn.h"
#include "bus/response_plans/PlanDataAccess/src/MLInOutParameter.h"
#include "bus/response_plans/PlanDataAccess/src/DataMatrixPara.h"


namespace TA_Base_Bus
{
	class MLSerialization
	{
	public:
		MLSerialization(Serialization* m_ioSerial);
		virtual ~MLSerialization();

		bool isSerialize();
		std::string getObjectType();
		std::string getStructType();
		std::string getDataType();
		int getPosNum();
		void serialize(MLInOutParameter& param);
		MLInOutParameter* deserialize();

		template<typename T>
		void serializeDataMatrixParaByType(MLInOutParameter& param)
		{
			ML_Plan::DataMatrixPara<T>* dpv = dynamic_cast<ML_Plan::DataMatrixPara<T>*>(&param);
			if (NULL != dpv)
			{
				m_ioSerial->serializeMLPos(*dpv);
				m_ioSerial->serialize(dpv->matrix, getStructType());
			}
		}

		template<typename T>
		MLInOutParameter* deserializeDataMatrixParaByType()
		{
			ML_Plan::DataMatrixPara<T>* dpv = new ML_Plan::DataMatrixPara<T>();
			if (NULL != dpv)
			{
				m_ioSerial->serializeMLPos(*dpv);
				m_ioSerial->serialize(dpv->matrix, getStructType());
			}
			return dpv;
		}

	private:
		Serialization * m_ioSerial;
	};
} // TA_Base_App

#endif // MLSERIALIZATION_H