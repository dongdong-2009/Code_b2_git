///////////////////////////////////////////////////////////
//  MLAlgorithmsImpl.h
//  Implementation of the Class MLAlgorithmsImpl
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////

#if !defined(EA_533A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_)
#define EA_533A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_

#include "IMLAlgorithms.h"

/**
 * the implementation class of the interface of IMLAlgorithms which implements all
 * machine learning algorithms available.
 */

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

class MLAlgorithmsImpl : public IMLAlgorithms
{

public:
	virtual void apply_pca(const TA_RealMatrix& rDataSet, TA_RealVec& eigVal, TA_RealMatrix& eigvec, PCA_PARA_OPTS paraOpt = PCA_PARA_OPTS());
	virtual void apply_pca(const TA_RealMatrix& rDataSet, size_t newDimension, TA_RealMatrix& rOutMatrix, PCA_PARA_OPTS paraOpt = PCA_PARA_OPTS());

	virtual void apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_uRow& assignments, KMeansParaOpts paraOpt = KMeansParaOpts());
	virtual void apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_RealMatrix& rCentroids, KMeansParaOpts paraOpt = KMeansParaOpts());

	virtual void apply_nca(const TA_RealMatrix& rInMatrix, const TA_uMatrix& rInLables, TA_RealMatrix& rOutMatrix, NCAParaOpts paraOpt = NCAParaOpts());

private:

};

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif /// !defined(EA_533A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_)
