/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_data_structure/src/DataMatrix.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/03/14 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Generic data matrix template class.
*/

#include "bus/response_plans/plan_data_structure/src/BaseData.h"

#ifndef DATAMATRIX_H
#define DATAMATRIX_H

namespace TA_Base_Bus
{
	namespace Plan
	{
		template <class T>
		class DataMatrix : public BaseData
		{
		private:
			typedef std::vector<T> Row;
			typedef std::vector<Row> Table;

		public:
			DataMatrix() : BaseData() {}
			DataMatrix(size_t colCount, size_t rowCount, std::string typeName)
				: BaseData(typeName)
			{
				for (std::vector<Row>::iterator itRow = m_table.begin(); itRow != m_table.end(); itRow++)
				{
					Row row = *itRow;
					row.resize(rowCount);
				}
			}

			DataMatrix(size_t colCount, size_t rowCount, const T& value, std::string typeName)
				: BaseData(typeName),
				matrix(colCount),
				m_rowCount(rowCount),
				m_colCount(colCount)
			{
				for (std::vector<Row>::iterator it = m_table.begin(); it != m_table.end(); it++)
				{
					Row row = *it;
					row.resize(rowCount, value);
				}
			}

			DataMatrix(const Table& table)
			{
				m_table = table;
			}

			~DataMatrix()
			{
				if (!m_table.empty())
				{
					for (std::vector<Row>::iterator it = m_table.begin(); it != m_table.end(); it++)
					{
						(*it).clear();
					}
					m_table.clear();
				}
			}

			void insertData(size_t column, size_t row, const T& value)
			{
				m_table[column][row] = value;
			}

			std::vector<Row>& getData() { return m_table; }

			size_t& getColumn() { return m_colCount; }
			size_t& getRow() { return m_rowCount; }
			void setColumn(size_t size) { m_colCount = size; }
			void setRow(size_t size) { m_rowCount = size; }

		private:
			std::vector<Row> m_table;
			size_t m_rowCount;
			size_t m_colCount;
		};

		// Predefined Data Matrix tempplate types
		typedef DataMatrix<double>  RealDataMatrixT;
		typedef DataMatrix<size_t>  uDataMatrixT;
		typedef DataMatrix<int>     nDataMatrixT;

	} // Plan

} // TA_Base_Bus

#endif // DATAMATRIX_H