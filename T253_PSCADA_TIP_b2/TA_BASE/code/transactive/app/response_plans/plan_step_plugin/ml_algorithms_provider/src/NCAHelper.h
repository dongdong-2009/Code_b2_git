///////////////////////////////////////////////////////////
//  NCAHelper.h
//  Helper class for implementation of NCA algorithm
//  Created on:      20-02-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////
#ifndef NCA_HELPER_H__
#define NCA_HELPER_H__

#include "CommonData.h"
#include "TA_MLDataTypes.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

class NCAHelper
{
public:
	NCAHelper(const NCAParaOpts& rParaOpts);
	~NCAHelper(){}

public:
	void execute_nca(const TA_RealMatrix& rInMatrix, TA_uMatrix& rInLables, TA_RealMatrix& rOutMatrix);

private:
	template<typename MetricType>
	void RunNCA(TA_RealMatrix& rInMatrix, const TA_uRow& rInLables,	TA_RealMatrix& rOutMatrix);

private:
	NCAParaOpts      m_ncaParaOpts;
};

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif /// NCA_HELPER_H__