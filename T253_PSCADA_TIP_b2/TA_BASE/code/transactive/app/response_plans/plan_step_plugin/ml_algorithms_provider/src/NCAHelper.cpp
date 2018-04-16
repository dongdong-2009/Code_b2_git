#include <mlpack/prereqs.hpp>
#include <mlpack/core/data/normalize_labels.hpp>
#include <mlpack/core/metrics/lmetric.hpp>
#include <mlpack/methods/nca/nca.hpp>
#include <mlpack/core/optimizers/lbfgs/lbfgs.hpp>
#include <mlpack/core/optimizers/minibatch_sgd/minibatch_sgd.hpp>

#include "NCAHelper.h"

#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

using namespace mlpack;
using TA_Base_Core::DebugUtil;

NCAHelper::NCAHelper(const NCAParaOpts& rParaOpts)
	: m_ncaParaOpts(rParaOpts)
{

}

void NCAHelper::execute_nca(const TA_RealMatrix& rInMatrix, TA_uMatrix& rInLables, TA_RealMatrix& rOutMatrix)
{
	FUNCTION_ENTRY("execute_nca");

	TA_RealMatrix inMatrix(rInMatrix);
	if (!rInLables.size())
	{
		LOG0(SourceInfo, DebugUtil::DebugDebug, "input lables is empty, using last column of input dataset as labels.");

		for (size_t i = 0; i < rInMatrix.n_cols; i++)
			rInLables[i] = (size_t)rInMatrix(rInMatrix.n_rows - 1, i);

		inMatrix.shed_row(rInMatrix.n_rows - 1);
	}
	// Now, normalize the labels.
	TA_uVec vecMappings;
	TA_uRow labels;
	data::NormalizeLabels(rInLables.row(0), labels, vecMappings);

	switch (m_ncaParaOpts.nMetricType)
	{
	case ManhattanDistance_Code:
		RunNCA<metric::ManhattanDistance>(inMatrix, labels, rOutMatrix);
		break;
	case SquaredEuclideanDistance_Code:
		RunNCA<metric::SquaredEuclideanDistance>(inMatrix, labels, rOutMatrix);
		break;
	case ChebyshevDistance_Code:
		RunNCA<metric::ChebyshevDistance>(inMatrix, labels, rOutMatrix);
		break;
	case EuclideanDistance_Code:
		RunNCA<metric::EuclideanDistance>(inMatrix, labels, rOutMatrix);
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the Metric type is not supported! MetricType Code: %d", m_ncaParaOpts.nMetricType);
		break;
	}

	FUNCTION_EXIT;
}


template<typename MetricType>
void NCAHelper::RunNCA(TA_RealMatrix& rInMatrix, const TA_uRow& rInLables, TA_RealMatrix& rOutMatrix)
{
	FUNCTION_ENTRY("RunNCA");

	arma::mat* pOutData = dynamic_cast<arma::mat*>(&rOutMatrix);

	// Normalize the data, if set.
	if (m_ncaParaOpts.normalize)
	{
		// Find the minimum and maximum values for each dimension.
		// arma::vec ranges = arma::max(dynamic_cast<arma::mat>(rInMatrix), 1) - arma::min(dynamic_cast<arma::mat>(rInMatrix), 1);
		arma::mat* pInputData = dynamic_cast<arma::mat*>(&rInMatrix);
		arma::vec ranges = arma::max((*pInputData), 1) - arma::min((*pInputData), 1);
		for (size_t d = 0; d < ranges.n_elem; ++d)
			if (ranges[d] == 0.0)
				ranges[d] = 1; // A range of 0 produces NaN later on.

		(*pOutData) = arma::diagmat(1.0 / ranges);
		LOG0(SourceInfo, DebugUtil::DebugDebug, "Using normalized starting point for optimization.");
	}
	else
	{
		pOutData->eye();
	}

	// now check the optimizer type and run the NCA algorithm
	switch (m_ncaParaOpts.nOptimizerType)
	{
	case SGD_Code:
		{
			nca::NCA<MetricType, optimization::SGD> aNCA(rInMatrix, rInLables);
			if (m_ncaParaOpts.bNeedAdjust)
			{
				// set up necessary parameters, others will be ignored.
				aNCA.Optimizer().StepSize()      = m_ncaParaOpts.stepSize;
				aNCA.Optimizer().MaxIterations() = m_ncaParaOpts.maxIterations;
				aNCA.Optimizer().Tolerance()     = m_ncaParaOpts.tolerance;
				aNCA.Optimizer().Shuffle()       = m_ncaParaOpts.shuffle;
			}

			aNCA.LearnDistance(rOutMatrix);
		}
		
		break;
	case LBFGS_Code:
		{
			nca::NCA<MetricType, optimization::L_BFGS> aNCA(rInMatrix, rInLables);
			if (m_ncaParaOpts.bNeedAdjust)
			{
				// set up necessary parameters, others will be ignored.
				aNCA.Optimizer().NumBasis()            = m_ncaParaOpts.numBasis;
				aNCA.Optimizer().MaxIterations()       = m_ncaParaOpts.maxIterations;
				aNCA.Optimizer().ArmijoConstant()      = m_ncaParaOpts.armijoConstant;
				aNCA.Optimizer().Wolfe()               = m_ncaParaOpts.wolfe;
				aNCA.Optimizer().MinGradientNorm()     = m_ncaParaOpts.tolerance;
				aNCA.Optimizer().MaxLineSearchTrials() = m_ncaParaOpts.maxLineSearchTrials;
				aNCA.Optimizer().MinStep()             = m_ncaParaOpts.minStep;
				aNCA.Optimizer().MaxStep()             = m_ncaParaOpts.maxStep;
			}

			aNCA.LearnDistance(rOutMatrix);
		}
		
		break;
	case MINIBATCH_SGD_Code:
		{
			nca::NCA<MetricType, optimization::MiniBatchSGD> aNCA(rInMatrix, rInLables);
			if (m_ncaParaOpts.bNeedAdjust)
			{
				// set up necessary parameters, others will be ignored.
				aNCA.Optimizer().StepSize()      = m_ncaParaOpts.stepSize;
				aNCA.Optimizer().MaxIterations() = m_ncaParaOpts.maxIterations;
				aNCA.Optimizer().Tolerance()     = m_ncaParaOpts.tolerance;
				aNCA.Optimizer().Shuffle()       = m_ncaParaOpts.shuffle;
				aNCA.Optimizer().BatchSize()     = m_ncaParaOpts.batchSize;
			}

			aNCA.LearnDistance(rOutMatrix);
		}
		
		break;
	default:
		LOG1(SourceInfo, DebugUtil::DebugError, "the optimizer type is not supported! optimizer Code: %d", m_ncaParaOpts.nOptimizerType);
		break;
	}

	FUNCTION_EXIT;
}

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)
