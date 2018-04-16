//////////////////////////////////////////////////////////////////////
///		@file		CommonData.h
///		@author		zhilin,ouyang
///		@date		2017-12-25 10:44:41
///
///		@brief	    to define some common data structures and
///                 variables used in the library.
///                 
//////////////////////////////////////////////////////////////////////
#if !defined(EA_E8814E79_AF7C_4324_9D2C_C4D9C80002F1__INCLUDED_)
#define EA_E8814E79_AF7C_4324_9D2C_C4D9C80002F1__INCLUDED_

#include "CommonDef.h"
#include "TA_MLDataTypes.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

enum PCADecompositionPolicyCode
{
	ExactSVDPolicy_Code = 0,
	RandomizedSVDPolicy_Code,
	QUICSVDPolicy_Code,
};

// the parameter options that pca algrothim may have
typedef struct st_pca_parameter_options_tag
{
	bool         bscaleData;              // parameter for specifying if the data should be scaled in each dimension by standard deviation when PCA is performed.
	int          nDecompositionPolicy;    // parameter for specifying the decomposition policy should be used when PCA is performed. (0: ExactSVDPolicy  1: RandomizedSVDPolicy  2: QUICSVDPolicy)

	st_pca_parameter_options_tag()
	: bscaleData(false)
	, nDecompositionPolicy(ExactSVDPolicy_Code){}

	st_pca_parameter_options_tag(
		bool bNeedScale,
		int  nDecomPolicy)
	: bscaleData(bNeedScale)
	, nDecompositionPolicy(nDecomPolicy){}

}PCA_PARA_OPTS, *PPCA_PARA_OPTS;

enum KMeansMetricTypeCode
{
	ManhattanDistance_Code            = 10,
	SquaredEuclideanDistance_Code,
	EuclideanDistance_Code,
	ChebyshevDistance_Code,
};

enum KMeansInitialPartitionPolicyCode
{
	RefinedStart_Code                = 20,
	SampleInitialization_Code,
};

enum KMeansEmptyClusterPolicyCode
{
	AllowEmptyClusters_Code          = 30,
	KillEmptyClusters_Code,
	MaxVarianceNewCluster,
};

enum KMeansAlgorithmCode
{
	ElkanKMeans_Code                = 40,
	HamerlyKMeans_Code,
	PellegMooreKMeans_Code,
	DefaultDualTreeKMeans_Code,
	CoverTreeDualTreeKMeans_Code,
	NaiveKMeans_Code,
};

// the parameter options that KMeans algorithm may have
typedef struct st_kmeans_parameter_options_tag
{
	bool        bInitialGuess;            // parameter to indicate that an initial cluster assignments or centroids passed in.
	size_t      nMaxIterations;           // parameter to specify the maximum number of iterations allowed before giving up, should be > 0
	int         nMetricType;              // parameter to specify the distance metric to use for this KMeans (ManhattanDistance, SquaredEuclideanDistance, EuclideanDistance, ChebyshevDistance)
	int         nInitialPartitionPolicy;  // parameter to specify the initial partitioning policy (RefinedStart, SampleInitialization)
	int         nEmptyClusterPolicy;      // parameter to specify what to do on an empty cluster (AllowEmptyClusters, KillEmptyClusters, MaxVarianceNewCluster)
	int         nAlgorithmCode;           // parameter to specify which algorithm to use for Lloyd step (ElkanKMeans, HamerlyKMeans, PellegMooreKMeans, DefaultDualTreeKMeans, CoverTreeDualTreeKMeans, NaiveKMeans)

	int         nSamplings;               // parameter that needed when initial Partition Policy is RefinedStart_Code, must greater than 0 if enabled.
	ta_float64  fPercentage;              // parameter that needed when initial Partition Policy is RefinedStart_Code, must greater than 0.0 and <= 1.0 if enabled.

	st_kmeans_parameter_options_tag()
	: bInitialGuess(false)
	, nMaxIterations(1000)
	, nMetricType(EuclideanDistance_Code)
	, nInitialPartitionPolicy(SampleInitialization_Code)
	, nEmptyClusterPolicy(MaxVarianceNewCluster)
	, nAlgorithmCode(NaiveKMeans_Code)
	, nSamplings(0)
	, fPercentage(0.0)
	{}	
}KMeansParaOpts, *PKMeansParaOpts;


enum NCAOptimizerTypeCode
{
	SGD_Code                   = 50,
	LBFGS_Code,
	MINIBATCH_SGD_Code,
};

// parameters for Neighborhood Components Analysis (NCA) algorithm
typedef struct st_nca_parameter_options_tag
{
	ta_float64    stepSize;             // Step size for stochastic gradient descent 0.01
	size_t        maxIterations;        // Maximum number of iterations for SGD or L-BFGS
	ta_float64    tolerance;            // Maximum tolerance for termination of SGD or L-BFGS
	bool          normalize;            // Use a normalized starting point for optimization. This is useful for when points are far apart, or when SGD is returning NaN
	bool          bNeedAdjust;          // a reserved tag for changing or modifying these parameters, default false otherwise set to true
	bool          shuffle;              // specifies whether needs to shuffle the order in which data points are visited for SGD or mini-batch SGD
	int           numBasis;             // Number of memory points to be stored for L-BFGS
	ta_float64    armijoConstant;       // Armijo constant for L-BFGS
	ta_float64    wolfe;                // Wolfe condition parameter for L-BFGS
	int           maxLineSearchTrials;  // Maximum number of line search trials for L-BFGS
	ta_float64    minStep;              // Minimum step of line search for L-BFGS
	ta_float64    maxStep;              // Maximum step of line search for L-BFGS
	size_t        batchSize;            // Batch size for mini-batch SGD
	int           nMetricType;          // parameter to specify the distance metric type (ManhattanDistance, SquaredEuclideanDistance, EuclideanDistance, ChebyshevDistance)
	int           nOptimizerType;       // to indicate the optimizer type (SGD, L_BFGS, MiniBatchSGD)

	st_nca_parameter_options_tag()
		: stepSize(0.01)
		, maxIterations(100000)
		, tolerance(1e-5)
		, normalize(true)
		, bNeedAdjust(false)
		, shuffle(true)
		, numBasis(10)
		, armijoConstant(1e-4)
		, wolfe(0.9)
		, maxLineSearchTrials(50)
		, minStep(1e-20)
		, maxStep(1e20)
		, batchSize(1000)
		, nMetricType(EuclideanDistance_Code)
		, nOptimizerType(SGD_Code)
	{}	
}NCAParaOpts, *PNCAParaOpts;

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif /// EA_E8814E79_AF7C_4324_9D2C_C4D9C80002F1__INCLUDED_