///////////////////////////////////////////////////////////
//  KMeansHelper.h
//  Helper class for implementation of KMeans algorithm
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////
#if !defined(EA_633A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_)
#define EA_633A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_

#include "CommonData.h"
#include "TA_MLDataTypes.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

class KMeansHelper
{
public:
	KMeansHelper(const KMeansParaOpts& paraOpt);
	~KMeansHelper();

public:
	void execute_kmeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_uRow& assignments);
	void execute_kmeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_RealMatrix& rCentroids);

private:
	template<typename MetricType>  
	void FindMetricTypePolicy(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters, 
		TA_uRow& assignments);

	template<typename MetricType>
	void FindMetricTypePolicy(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_RealMatrix& rCentroids);


	template<typename MetricType, 
		     typename InitialPartitionPolicy> 
	void FindEmptyClusterPolicy(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp);

	template<typename MetricType,
		typename InitialPartitionPolicy>
		void FindEmptyClusterPolicy(
			const TA_RealMatrix& rInMatrixData,
			const size_t clusters,
			TA_RealMatrix& rCentroids,
			const InitialPartitionPolicy& ipp);


	template<typename MetricType,
		     typename InitialPartitionPolicy,
	         typename EmptyClusterPolicy> 
	void FindLloydStepType(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp);

	template<typename MetricType,
		typename InitialPartitionPolicy,
		typename EmptyClusterPolicy>
		void FindLloydStepType(
			const TA_RealMatrix& rInMatrixData,
			const size_t clusters,
			TA_RealMatrix& rCentroids,
			const InitialPartitionPolicy& ipp);


	template<typename MetricType,
			 typename InitialPartitionPolicy,
			 typename EmptyClusterPolicy,
			 template<class, class> class LloydStepType>
	void RunKMeans(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp);

	template<typename MetricType,
			 typename InitialPartitionPolicy,
			 typename EmptyClusterPolicy,
			 template<class, class> class LloydStepType>
	void RunKMeans(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_RealMatrix& rCentroids,
		const InitialPartitionPolicy& ipp);

private:
	KMeansParaOpts    m_KMeansParaOpts;
};

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif /// EA_633A5A8C_093D_4efc_AF93_86BE14ACDF31__INCLUDED_