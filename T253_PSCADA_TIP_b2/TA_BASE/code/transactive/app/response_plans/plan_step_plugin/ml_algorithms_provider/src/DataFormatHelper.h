///////////////////////////////////////////////////////////
//  DataFormatHelper.h
//  Implementation of the Class DataFormatHelper
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////

#if !defined(EA_0BB86F2A_84A5_4cce_AF22_C23B934BAB05__INCLUDED_)
#define EA_0BB86F2A_84A5_4cce_AF22_C23B934BAB05__INCLUDED_

#include "CommonDef.h"

#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

using TA_Base_Core::DebugUtil;

/**
 * it is a helper class which provides useful complementary methods for the
 * purpose of organizing or structuring data before or after invoking various 3rd
 * party machine learning algorithms.
 */
class DataFormatHelper
{
public:
	template<typename stdSrcType>
	inline
		size_t getSTDTCols(const stdSrcType& rInSTDT)
	{
		typename stdSrcType::const_iterator cIter = rInSTDT.begin();
		size_t n_f_cols = cIter->size();
		size_t n_f_max = n_f_cols;
		size_t n_line = 1;
		while (cIter != rInSTDT.end())
		{
			if (n_f_cols != cIter->size())
			{
				LOG1(SourceInfo, DebugUtil::DebugWarn, "the number of columns of the input dataset is inconsistent in line: %d! Will add default values for the missing ones.", n_line);
			}
			if (cIter->size() > n_f_max)
			{
				n_f_max = cIter->size();
			}
			cIter++;
			n_line++;
		}

		return n_f_max;
	}

	template<typename stdSrcType, typename DestType>
	inline
		void convertStdT2DestT(const stdSrcType& rInSTDT, DestType& rOutDestType, size_t nMaxCols)
	{
		typename stdSrcType::const_iterator cIterT = rInSTDT.begin();
		typedef  typename stdSrcType::value_type  OuterType;
		typedef  typename OuterType::value_type   InnerType;
		size_t  nCurRow = 0;
		while (cIterT != rInSTDT.end())
		{
			typename OuterType::const_iterator cInnerIter = cIterT->begin();
			for (size_t i = 0; i < nMaxCols; i++)
			{
				if (cInnerIter == cIterT->end())
				{
					rOutDestType.at(nCurRow, i) = InnerType();
				}
				else
				{
					rOutDestType.at(nCurRow, i) = (*cInnerIter);
					cInnerIter++;
				}
			}

			cIterT++;
			nCurRow++;
		}
	}
};

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif // !defined(EA_0BB86F2A_84A5_4cce_AF22_C23B934BAB05__INCLUDED_)
