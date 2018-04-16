#include <mlpack/prereqs.hpp>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <mlpack/methods/kmeans/allow_empty_clusters.hpp>
#include <mlpack/methods/kmeans/kill_empty_clusters.hpp>
#include <mlpack/methods/kmeans/refined_start.hpp>
#include <mlpack/methods/kmeans/elkan_kmeans.hpp>
#include <mlpack/methods/kmeans/hamerly_kmeans.hpp>
#include <mlpack/methods/kmeans/pelleg_moore_kmeans.hpp>
#include <mlpack/methods/kmeans/dual_tree_kmeans.hpp>

#include "KMeansHelper.h"

#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

using namespace mlpack;
using TA_Base_Core::DebugUtil;

KMeansHelper::KMeansHelper(const KMeansParaOpts& paraOpt)
: m_KMeansParaOpts(paraOpt)
{
}

KMeansHelper::~KMeansHelper()
{
}

void KMeansHelper::execute_kmeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_uRow& assignments)
{
	FUNCTION_ENTRY("execute_kmeans");
	switch (m_KMeansParaOpts.nMetricType)
	{
	case ManhattanDistance_Code:
		FindMetricTypePolicy<metric::ManhattanDistance>(rInMatrixData, clusters, assignments);
		break;
	case SquaredEuclideanDistance_Code:
		FindMetricTypePolicy<metric::SquaredEuclideanDistance>(rInMatrixData, clusters, assignments);
		break;
	case ChebyshevDistance_Code:
		FindMetricTypePolicy<metric::ChebyshevDistance>(rInMatrixData, clusters, assignments);
		break;
	case EuclideanDistance_Code:
		FindMetricTypePolicy<metric::EuclideanDistance>(rInMatrixData, clusters, assignments);
		break;

	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Metric type is not supported! MetricType Code: %d", m_KMeansParaOpts.nMetricType);
		break;
	}
	FUNCTION_EXIT;
}

void KMeansHelper::execute_kmeans(const TA_RealMatrix& rInMatrixData, const size_t clusters, TA_RealMatrix& rCentroids)
{
	FUNCTION_ENTRY("execute_kmeans");
	switch (m_KMeansParaOpts.nMetricType)
	{
	case ManhattanDistance_Code:
		FindMetricTypePolicy<metric::ManhattanDistance>(rInMatrixData, clusters, rCentroids);
		break;
	case SquaredEuclideanDistance_Code:
		FindMetricTypePolicy<metric::SquaredEuclideanDistance>(rInMatrixData, clusters, rCentroids);
		break;
	case ChebyshevDistance_Code:
		FindMetricTypePolicy<metric::ChebyshevDistance>(rInMatrixData, clusters, rCentroids);
		break;
	case EuclideanDistance_Code:
		FindMetricTypePolicy<metric::EuclideanDistance>(rInMatrixData, clusters, rCentroids);
		break;

	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Metric type is not supported! MetricType Code: %d", m_KMeansParaOpts.nMetricType);
		break;
	}
	FUNCTION_EXIT;
}

template<typename MetricType>
void KMeansHelper::FindMetricTypePolicy(
	const TA_RealMatrix& rInMatrixData,
	const size_t clusters,
	TA_uRow& assignments)
{
	if (RefinedStart_Code == m_KMeansParaOpts.nInitialPartitionPolicy)
	{
		if (m_KMeansParaOpts.nSamplings <= 0 ||
			m_KMeansParaOpts.fPercentage <= 0.0 ||
			m_KMeansParaOpts.fPercentage > 1.0)
		{
			LOG2(SourceInfo, DebugUtil::DebugError, "the pass in parameters (nSamplings or fPercentage) were wrong! nSamplings: %d, Percentage: %f", 
				m_KMeansParaOpts.nSamplings, m_KMeansParaOpts.fPercentage);
			return;
		}

		FindEmptyClusterPolicy<MetricType, kmeans::RefinedStart>(rInMatrixData, clusters, assignments, kmeans::RefinedStart(m_KMeansParaOpts.nSamplings, m_KMeansParaOpts.fPercentage));
	}
	else
	{
		FindEmptyClusterPolicy<MetricType, kmeans::SampleInitialization>(rInMatrixData, clusters, assignments, kmeans::SampleInitialization());
	}
}

template<typename MetricType>
void KMeansHelper::FindMetricTypePolicy(
	const TA_RealMatrix& rInMatrixData,
	const size_t clusters,
	TA_RealMatrix& rCentroids)
{
	if (RefinedStart_Code == m_KMeansParaOpts.nInitialPartitionPolicy)
	{
		if (m_KMeansParaOpts.nSamplings <= 0 ||
			m_KMeansParaOpts.fPercentage <= 0.0 ||
			m_KMeansParaOpts.fPercentage > 1.0)
		{
			LOG2(SourceInfo, DebugUtil::DebugError, "the pass in parameters (nSamplings or fPercentage) were wrong! nSamplings: %d, Percentage: %f",
				m_KMeansParaOpts.nSamplings, m_KMeansParaOpts.fPercentage);
			return;
		}

		FindEmptyClusterPolicy<MetricType, kmeans::RefinedStart>(rInMatrixData, clusters, rCentroids, kmeans::RefinedStart(m_KMeansParaOpts.nSamplings, m_KMeansParaOpts.fPercentage));
	}
	else
	{
		FindEmptyClusterPolicy<MetricType, kmeans::SampleInitialization>(rInMatrixData, clusters, rCentroids, kmeans::SampleInitialization());
	}
}

template<typename MetricType,
	     typename InitialPartitionPolicy>
void KMeansHelper::FindEmptyClusterPolicy(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp)
{
	switch (m_KMeansParaOpts.nEmptyClusterPolicy)
	{
	case AllowEmptyClusters_Code:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::AllowEmptyClusters>(rInMatrixData, clusters, assignments, ipp);
		break;
	case KillEmptyClusters_Code:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::KillEmptyClusters>(rInMatrixData, clusters, assignments, ipp);
		break;
	case MaxVarianceNewCluster:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::MaxVarianceNewCluster>(rInMatrixData, clusters, assignments, ipp);
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Empty cluster policy is not supported! nEmptyClusterPolicy Code: %d", m_KMeansParaOpts.nEmptyClusterPolicy);
		break;
	}
}

template<typename MetricType,
		 typename InitialPartitionPolicy>
void KMeansHelper::FindEmptyClusterPolicy(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_RealMatrix& rCentroids,
		const InitialPartitionPolicy& ipp)
{
	switch (m_KMeansParaOpts.nEmptyClusterPolicy)
	{
	case AllowEmptyClusters_Code:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::AllowEmptyClusters>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case KillEmptyClusters_Code:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::KillEmptyClusters>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case MaxVarianceNewCluster:
		FindLloydStepType<MetricType, InitialPartitionPolicy, kmeans::MaxVarianceNewCluster>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Empty cluster policy is not supported! nEmptyClusterPolicy Code: %d", m_KMeansParaOpts.nEmptyClusterPolicy);
		break;
	}
}

template<typename MetricType,
		 typename InitialPartitionPolicy,
		 typename EmptyClusterPolicy>
void KMeansHelper::FindLloydStepType(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp)
{
	switch (m_KMeansParaOpts.nAlgorithmCode)
	{
	case ElkanKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::ElkanKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	case HamerlyKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::HamerlyKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	case PellegMooreKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::PellegMooreKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	case DefaultDualTreeKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::DefaultDualTreeKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	case CoverTreeDualTreeKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::CoverTreeDualTreeKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	case NaiveKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::NaiveKMeans>(rInMatrixData, clusters, assignments, ipp);
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Algorithm Code is not supported! nAlgorithmCode Code: %d", m_KMeansParaOpts.nAlgorithmCode);
		break;
	}
}

template<typename MetricType,
		 typename InitialPartitionPolicy,
		 typename EmptyClusterPolicy>
void KMeansHelper::FindLloydStepType(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_RealMatrix& rCentroids,
		const InitialPartitionPolicy& ipp)
{
	switch (m_KMeansParaOpts.nAlgorithmCode)
	{
	case ElkanKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::ElkanKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case HamerlyKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::HamerlyKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case PellegMooreKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::PellegMooreKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case DefaultDualTreeKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::DefaultDualTreeKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case CoverTreeDualTreeKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::CoverTreeDualTreeKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	case NaiveKMeans_Code:
		RunKMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, kmeans::NaiveKMeans>(rInMatrixData, clusters, rCentroids, ipp);
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Algorithm Code is not supported! nAlgorithmCode Code: %d", m_KMeansParaOpts.nAlgorithmCode);
		break;
	}
}

template<typename MetricType,
		 typename InitialPartitionPolicy,
		 typename EmptyClusterPolicy,
		 template<class, class> class LloydStepType>
void KMeansHelper::RunKMeans(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_uRow& assignments,
		const InitialPartitionPolicy& ipp)
{
	if (!rInMatrixData.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}
	if (m_KMeansParaOpts.nMaxIterations <= 0)
	{
		LOG1(SourceInfo, DebugUtil::DebugError, "the input max iterations is <= 0, no action is needed! MaxIterations: %d", m_KMeansParaOpts.nMaxIterations);
		return;
	}

	LOG0(SourceInfo, DebugUtil::DebugDebug, "start clustering...");

	kmeans::KMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, LloydStepType> kmeans(m_KMeansParaOpts.nMaxIterations, MetricType(), ipp);
	kmeans.Cluster(rInMatrixData, clusters, assignments, m_KMeansParaOpts.bInitialGuess);

	LOG0(SourceInfo, DebugUtil::DebugDebug, "end clustering");

}

template<typename MetricType,
		 typename InitialPartitionPolicy,
		 typename EmptyClusterPolicy,
		 template<class, class> class LloydStepType>
void KMeansHelper::RunKMeans(
		const TA_RealMatrix& rInMatrixData,
		const size_t clusters,
		TA_RealMatrix& rCentroids,
		const InitialPartitionPolicy& ipp)
{
	if (!rInMatrixData.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugError, "the input dataset is empty, no operation is needed!");
		return;
	}
	if (m_KMeansParaOpts.nMaxIterations <= 0)
	{
		LOG1(SourceInfo, DebugUtil::DebugError, "the input max iterations is <= 0, no action is needed! MaxIterations: %d", m_KMeansParaOpts.nMaxIterations);
		return;
	}

	LOG0(SourceInfo, DebugUtil::DebugDebug, "start clustering...");

	kmeans::KMeans<MetricType, InitialPartitionPolicy, EmptyClusterPolicy, LloydStepType> kmeans(m_KMeansParaOpts.nMaxIterations, MetricType(), ipp);
	kmeans.Cluster(rInMatrixData, clusters, rCentroids, m_KMeansParaOpts.bInitialGuess);

	LOG0(SourceInfo, DebugUtil::DebugDebug, "end clustering");
}

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)
