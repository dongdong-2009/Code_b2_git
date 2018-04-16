/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_data_structure/src/BaseData.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/16 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Generic data matrix type class.
*/

#include <string>

#ifndef MLDATA_H
#define MLDATA_H

namespace TA_Base_Bus
{
	namespace Plan
	{
		class BaseData
		{
		public:
			BaseData();
			BaseData(std::string typeName);
			~BaseData();

		private:
			//BaseData(const BaseData&);
			BaseData & operator=(const BaseData&);

		private:
			std::string m_typeName;
		};

	} // Plan

} // TA_Base_Bus

#endif // MLDATA_H