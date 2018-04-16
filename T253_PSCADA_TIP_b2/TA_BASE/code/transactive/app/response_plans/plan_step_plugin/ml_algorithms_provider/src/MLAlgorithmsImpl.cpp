///////////////////////////////////////////////////////////
//  MLAlgorithmsImpl.cpp
//  Implementation of the Class MLAlgorithmsImpl
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////
#include <mlpack/prereqs.hpp>
#include <mlpack/methods/pca/pca.hpp>
#include <mlpack/methods/pca/decomposition_policies/exact_svd_method.hpp>
#include <mlpack/methods/pca/decomposition_policies/quic_svd_method.hpp>
#include <mlpack/methods/pca/decomposition_policies/randomized_svd_method.hpp>

#include "MLAlgorithmsImpl.h"
#include "TA_MLDataTypes.h"
#include "KMeansHelper.h"
#include "NCAHelper.h"

#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

using namespace mlpack;
using TA_Base_Core::DebugUtil;

void MLAlgorithmsImpl::apply_pca(const TA_RealMatrix& rDataSet, TA_RealVec& eigVal, TA_RealMatrix& eigvec, PCA_PARA_OPTS paraOpt)
{
	if (!rDataSet.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}

	TA_RealMatrix transformDataset(rDataSet);
	switch (paraOpt.nDecompositionPolicy)
	{
	case RandomizedSVDPolicy_Code:
		{
			pca::PCAType<pca::RandomizedSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rDataSet, transformDataset, eigVal, eigvec);
		}		
		break;
	case QUICSVDPolicy_Code:
		{
			pca::PCAType<pca::QUICSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rDataSet, transformDataset, eigVal, eigvec);
		}		
		break;
	case ExactSVDPolicy_Code:
		{
			pca::PCAType<pca::ExactSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rDataSet, transformDataset, eigVal, eigvec);
		}		
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the decomposition policy is not supported! Code: %d", paraOpt.nDecompositionPolicy);
		break;
	}
}


void MLAlgorithmsImpl::apply_pca(const TA_RealMatrix& rDataSet, size_t newDimension, TA_RealMatrix& rOutMatrix, PCA_PARA_OPTS paraOpt)
{
	if (!rDataSet.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}

	rOutMatrix = rDataSet;
	switch (paraOpt.nDecompositionPolicy)
	{
	case RandomizedSVDPolicy_Code:
		{
			pca::PCAType<pca::RandomizedSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rOutMatrix, newDimension);
		}		
		break;
	case QUICSVDPolicy_Code:
		{
			pca::PCAType<pca::QUICSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rOutMatrix, newDimension);
		}		
		break;
	case ExactSVDPolicy_Code:
		{
			pca::PCAType<pca::ExactSVDPolicy> PCAInstance(paraOpt.bscaleData);
			PCAInstance.Apply(rOutMatrix, newDimension);
		}
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the decomposition policy is not supported! Code: %d", paraOpt.nDecompositionPolicy);
		break;
	}
}

void MLAlgorithmsImpl::apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_uRow& assignments, KMeansParaOpts paraOpt)
{
	if (!rInMatrixData.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}

	KMeansHelper aKMeansHelper(paraOpt);
	aKMeansHelper.execute_kmeans(rInMatrixData,  clusters, assignments);
}

void MLAlgorithmsImpl::apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_RealMatrix& rCentroids, KMeansParaOpts paraOpt)
{
	if (!rInMatrixData.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}

	KMeansHelper aKMeansHelper(paraOpt);
	aKMeansHelper.execute_kmeans(rInMatrixData, clusters, rCentroids);
}

void MLAlgorithmsImpl::apply_nca(const TA_RealMatrix& rInMatrix, const TA_uMatrix& rInLables, TA_RealMatrix& rOutMatrix, NCAParaOpts paraOpt)
{
	if (!rInMatrix.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, will not perform nca!");
		return;
	}

	TA_uMatrix uInLables(rInLables);
	arma::umat* pRawLables = dynamic_cast<arma::umat*>(&uInLables);
	if (1 == pRawLables->n_cols)
	{
		(*pRawLables) = arma::trans(*pRawLables);
	}
	if (pRawLables->n_rows > 1)
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "Labels must have only one column or row, will not perform nca!");
		return;
	}

	NCAHelper aNCAHelper(paraOpt);
	aNCAHelper.execute_nca(rInMatrix, uInLables, rOutMatrix);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
	PUBLIC_DLL_EXPORTS IMLAlgorithms* getMLAlgorithmsProvider()
	{
		IMLAlgorithms* pMLAlgorithmProvider =new MLAlgorithmsImpl();
		return pMLAlgorithmProvider;
	}
}

PUBLIC_DLL_EXPORTS MLAlgorithmsProviderUPtr getMLAlgorithmsProviderUPtr()
{
	MLAlgorithmsProviderUPtr pAutoMLAlgorithmProvider(new MLAlgorithmsImpl());
	return pAutoMLAlgorithmProvider;
}


NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)
