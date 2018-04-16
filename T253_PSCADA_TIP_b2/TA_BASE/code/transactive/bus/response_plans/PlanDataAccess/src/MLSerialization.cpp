/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLSerialization.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/21 15:03:08 $
* Last modified by:  $Author: hoa.le $
*
* This is class includes serialization methods for ML Algorithms' step parameters.
*/


#include "bus/response_plans/PlanDataAccess/src/MLSerialization.h"

#include "bus/response_plans/PlanDataAccess/src/MLConstDefn.h"
#include "bus/response_plans/PlanDataAccess/src/DataPointVectorPara.h"
#include "bus/response_plans/PlanDataAccess/src/DataMatrixPara.h"

namespace TA_Base_Bus
{
	MLSerialization::MLSerialization(TA_Base_Bus::Serialization* ioSerial) : m_ioSerial(ioSerial)
	{
	}

	MLSerialization::~MLSerialization() {}

	bool MLSerialization::isSerialize()
	{
		return m_ioSerial->isSerialize();
	}

	std::string MLSerialization::getObjectType()
	{
		std::string str = m_ioSerial->getPtree().get<std::string>(ML_Plan::OBJECT_TYPE);
		return str;
	}

	std::string MLSerialization::getStructType()
	{
		return m_ioSerial->getPtree().get<std::string>(ML_Plan::STRUCT_TYPE);
	}

	std::string MLSerialization::getDataType()
	{
		return m_ioSerial->getPtree().get<std::string>(ML_Plan::DATA_TYPE);
	}

	int MLSerialization::getPosNum()
	{
		return m_ioSerial->stringVector().size();
	}

	void MLSerialization::serialize(MLInOutParameter& param)
	{
		if (NULL != dynamic_cast<ML_Plan::DataPointVectorPara*>(&param))
		{
			ML_Plan::DataPointVectorPara* dpv = dynamic_cast<ML_Plan::DataPointVectorPara*>(&param);
			m_ioSerial->serializeMLPos(*dpv);
			m_ioSerial->serialize(dpv->m_DataPt, getStructType());
		}
		else if (param.ObjectType.compare(ML_Plan::DATA_MATRIX) == 0)
		{
			if (param.DataType.compare(ML_Plan::INT) == 0)
				serializeDataMatrixParaByType<int>(param);
			else if (param.DataType.compare(ML_Plan::DOUBLE) == 0)
				serializeDataMatrixParaByType<double>(param);
			else if (param.DataType.compare(ML_Plan::FLOAT) == 0)
				serializeDataMatrixParaByType<float>(param);
			else if (param.DataType.compare(ML_Plan::STRING) == 0)
				serializeDataMatrixParaByType<std::string>(param);
		}
	}

	TA_Base_Bus::MLInOutParameter*  MLSerialization::deserialize()
	{
		std::string strObj = getObjectType();

		if (strObj.compare(ML_Plan::DATAPOINT_VECTOR) == 0)
		{
			ML_Plan::DataPointVectorPara* param = new ML_Plan::DataPointVectorPara();
			m_ioSerial->serializeMLPos(*param);
			m_ioSerial->serialize(param->m_DataPt, getStructType());
			return param;
		}
		else if (strObj.compare(ML_Plan::DATA_MATRIX) == 0)
		{
			std::string strType = getDataType();
			if (strType.compare(ML_Plan::INT) == 0)
				return deserializeDataMatrixParaByType<int>();
			else if (strType.compare(ML_Plan::DOUBLE) == 0)
				return deserializeDataMatrixParaByType<double>();
			else if (strType.compare(ML_Plan::FLOAT) == 0)
				return deserializeDataMatrixParaByType<float>();
			else if (strType.compare(ML_Plan::STRING) == 0)
				return deserializeDataMatrixParaByType<std::string>();
		}
		return NULL;
	}
} // TA_Base_App