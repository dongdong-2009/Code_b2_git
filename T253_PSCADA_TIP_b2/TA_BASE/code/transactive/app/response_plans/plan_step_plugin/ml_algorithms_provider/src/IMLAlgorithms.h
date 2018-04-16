
///////////////////////////////////////////////////////////
//  IMLAlgorithms.h
//  Implementation of the Interface IMLAlgorithms
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////

#if !defined(EA_69CB8B31_C84F_46e7_A356_57775E221328__INCLUDED_)
#define EA_69CB8B31_C84F_46e7_A356_57775E221328__INCLUDED_

#include "CommonData.h"
#include <memory>

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

/**
* the interface which provides all the related machine learning algorithms.
*/
class PUBLIC_DLL_EXPORTS IMLAlgorithms
{

public:
	virtual ~IMLAlgorithms() {};

	/**
	* Apply Principal Component Analysis to the provided data set.
	*
	* @param rDataSet Data matrix.	
	* @param eigval Vector to put eigenvalues into.
	* @param eigvec Matrix to put eigenvectors (loadings) into.
	* @param paraOpt set the parameter options when PCA is performed, check the structure for available parameters.
	*/
	 virtual void apply_pca(const TA_RealMatrix& rDataSet, TA_RealVec& eigVal, TA_RealMatrix& eigvec, PCA_PARA_OPTS paraOpt = PCA_PARA_OPTS() ) = 0;


	/**
	* Apply Principal Component Analysis to the provided data set.
	*
	* @param rDataSet Data matrix.
	* @param newDimension the new dimension of the data that intend to perform, dimensions should be less than the pass in data matrix.
	* @param rOutMatrix Matrix to put the output matrix into.
	* @param paraOpt set the parameter options when PCA is performed, check the structure for available parameters.
	*/
	 virtual void apply_pca(const TA_RealMatrix& rDataSet, size_t newDimension, TA_RealMatrix& rOutMatrix, PCA_PARA_OPTS paraOpt = PCA_PARA_OPTS() ) = 0;


	 /**
	 * K-Means clustering, using a variety of possible implementation of Lloyd's algorithm.
	 *
	 * @param rInMatrixData Data matrix provided.
	 * @param clusters Number of clusters to compute.
	 * @param assignments Vector to store cluster assignments in.
	 * @param paraOpt set the parameter options when KMeans is performed, check the structure for available parameters.
	 */
	 virtual void apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_uRow& assignments, KMeansParaOpts paraOpt = KMeansParaOpts()) = 0;


	 /**
	 * K-Means clustering, using a variety of possible implementation of Lloyd's algorithm.
	 *
	 * @param rInMatrixData Data matrix provided.
	 * @param clusters Number of clusters to compute.
	 * @param rCentroids Matrix in which centroids are stored.
	 * @param paraOpt set the parameter options when KMeans is performed, check the structure for available parameters.
	 */
	 virtual void apply_KMeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_RealMatrix& rCentroids, KMeansParaOpts paraOpt = KMeansParaOpts()) = 0;


	 /**
	 * Apply Neighborhood Components Analysis (NCA) on given data set.
	 *
	 * @param rInMatrix Data matrix provided.
	 * @param rInLables Input dataset labels.
	 * @param rOutMatrix output_matrix Covariance matrix of Mahalanobis distance.
	 * @param paraOpt set the parameter options when NCA is performed, check the structure for available parameters.
	 */
	 virtual void apply_nca(const TA_RealMatrix& rInMatrix, const TA_uMatrix& rInLables, TA_RealMatrix& rOutMatrix, NCAParaOpts paraOpt = NCAParaOpts()) = 0;
};

// for C interface
extern "C"
{
	// the caller of this interface is responsible to release the IMLAlgorithms instance.
	PUBLIC_DLL_EXPORTS IMLAlgorithms* getMLAlgorithmsProvider();
}

// for C++ interface
typedef std::unique_ptr<IMLAlgorithms> MLAlgorithmsProviderUPtr;

PUBLIC_DLL_EXPORTS MLAlgorithmsProviderUPtr getMLAlgorithmsProviderUPtr();

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif // !defined(EA_69CB8B31_C84F_46e7_A356_57775E221328__INCLUDED_)
