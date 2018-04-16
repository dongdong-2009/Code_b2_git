/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_data_structure/src/BaseData.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/16 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Generic data matrix type class.
*/

#include "bus/response_plans/plan_data_structure/src/BaseData.h"

namespace TA_Base_Bus
{
	namespace Plan
	{
		BaseData::BaseData() : m_typeName("") {}
		BaseData::BaseData(std::string typeName) : m_typeName(typeName) {}
		BaseData::~BaseData() {};

	} // Plan

} // TA_Base_Bus