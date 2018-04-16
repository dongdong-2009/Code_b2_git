/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_data_structure/src/DataVector.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/14 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Generic data vector template class.
*/

#include "bus/response_plans/plan_data_structure/src/BaseData.h"

#ifndef DATAVECTOR_H
#define DATAVECTOR_H

#include  <vector>

namespace TA_Base_Bus
{
	namespace Plan
	{
		template <class T>
		class DataVector : public BaseData
		{
		public:
			DataVector() : BaseData() {}
			DataVector(std::string typeName)
				: BaseData(typeName)
			{
			}

			DataVector(const std::vector<T>& list)
				: BaseData(typeName)
			{
				m_datalist = list;
			}

			~DataVector()
			{
				if (!m_datalist.empty())
				{
					m_datalist.clear();
				}
			}

		public:
			std::vector<T>& getData()
			{
				return m_datalist;
			}

			void insertData(const T& item)
			{
				m_datalist.push_back(item);
			}

		private:
			std::vector<T> m_datalist;
		};

		// Predefined Data Vector tempplate types
		typedef DataVector<double>  RealDataVectorT;
		typedef DataVector<size_t>  uDataVectorT;
		typedef DataVector<int>     nDataVectorT;

	} // Plan

} // TA_Base_Bus

#endif // DATAVECTOR_H