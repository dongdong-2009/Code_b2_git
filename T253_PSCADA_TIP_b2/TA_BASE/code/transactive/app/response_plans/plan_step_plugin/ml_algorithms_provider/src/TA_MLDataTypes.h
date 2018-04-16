///////////////////////////////////////////////////////////
//  TA_MLDataTypes.h
//  Define common machine learning data types which will
//  be used in this module, such TA_RealMatrix, TA_uRow etc.
//  Created on:      09-1ÔÂ-2018 16:03:34
//  Original author: ST-OYZL
///////////////////////////////////////////////////////////
#if !defined(EA_D7814E79_AF7C_4324_9D2C_C4D9C80002F0__INCLUDED_)
#define EA_D7814E79_AF7C_4324_9D2C_C4D9C80002F0__INCLUDED_

#include "CommonDef.h"
#include <mlpack/core.hpp>

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

/**
* Define the double matrix type for TA, which is just a simple wrapper of the third
* party library  Armadillo.
*/
class PUBLIC_DLL_EXPORTS TA_RealMatrix : public arma::mat
{
private:
	typedef RealVec::iterator                    VecIterT;
	typedef RealVec::const_iterator              cVecIterT;
	typedef RealTable::iterator                  TableITerT;
	typedef RealTable::const_iterator            cTableIterT;

public:
	~TA_RealMatrix(){}
	TA_RealMatrix(){}

	TA_RealMatrix(size_t n_rows, size_t n_cols)
		: arma::mat(n_rows, n_cols){}	

	TA_RealMatrix(const RealVec& rVec)
		: arma::mat(rVec){}

	TA_RealMatrix(const RealTable& rTable);
	TA_RealMatrix(size_t n_rows, size_t n_cols, const ta_float64 default_value);

	void setData(const RealTable& rTable);
};

/**
* Define the unsigned integer (size_t) matrix type for TA, which is just a simple wrapper of the third
* party library  Armadillo.
*/
class PUBLIC_DLL_EXPORTS TA_uMatrix : public arma::umat
{
private:
	typedef uVec::iterator                    VecIterT;
	typedef uVec::const_iterator              cVecIterT;
	typedef uTable::iterator                  TableITerT;
	typedef uTable::const_iterator            cTableIterT;

public:
	~TA_uMatrix(){}
	TA_uMatrix(){}

	TA_uMatrix(size_t n_rows, size_t n_cols)
		: arma::umat(n_rows, n_cols){}

	TA_uMatrix(size_t n_rows, size_t n_cols, const size_t default_value);

	TA_uMatrix(const uVec& rVec)
		: arma::umat(rVec){}

	TA_uMatrix(const uTable& rTable);
	void setData(const uTable& rTable);
};


/**
* Define the unsigned integer (size_t) Row vector type for TA, which is just a simple wrapper of the third
* party library  Armadillo.
*/
class PUBLIC_DLL_EXPORTS TA_uRow : public arma::Row<size_t>
{
private:
	typedef uVec::iterator                    VecIterT;
	typedef uVec::const_iterator              cVecIterT;
	typedef uTable::iterator                  TableITerT;
	typedef uTable::const_iterator            cTableIterT;

public:
	TA_uRow(){}
	~TA_uRow(){}

	TA_uRow(size_t n_rows) : arma::Row<size_t>(n_rows){}
	TA_uRow(size_t n_rows, size_t n_cols) : arma::Row<size_t>(n_rows, n_cols){}
	TA_uRow(const uVec& uVec) : arma::Row<size_t>(uVec){}

	TA_uRow(size_t n_rows, size_t n_cols, size_t default_value);

	TA_uRow(const uTable& rTable);
	void setData(const uTable& rTable);


};

/**
* Define the vector type for TA, which is just a simple wrapper of the third
* party library  Armadillo.
*/
class PUBLIC_DLL_EXPORTS TA_RealVec : public arma::vec
{
public:
	TA_RealVec(){}
	~TA_RealVec(){}

	TA_RealVec(const RealVec& rRelVec)
		: arma::vec(rRelVec){}

public:
	void   push_back(const ta_float64 value);	
};


/**
* Define the vector type for TA, which is just a simple wrapper of the third
* party library  Armadillo.
*/
class PUBLIC_DLL_EXPORTS TA_uVec : public arma::uvec
{
public:
	TA_uVec() {}
	~TA_uVec() {}

	TA_uVec(const uVec& rRelVec)
		: arma::uvec(rRelVec) {}

public:
	void   push_back(const size_t value);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// type conversion functions which to easy the usage between MLL provider library and C++ Standard vector type
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// type StdVecType should be std::vector<T>
template<typename TAMLDataType, typename StdVecType>
inline
void convertTAMLDataType2StdVec(const TAMLDataType& rInTAMLData, StdVecType& rOutStdVec)
{
	typename TAMLDataType::const_iterator cIter = rInTAMLData.begin();
	while (cIter != rInTAMLData.end())
	{
		rOutStdVec.push_back((*cIter));
		cIter++;
	}
}

// type StdMatrixType should be std::vecotr<std::vector<T> >
template<typename TAMLDataType, typename StdMatrixType>
inline
void convertTAMLDataType2StdMatrix(const TAMLDataType& rInTAMLData, StdMatrixType& rOutStdMatrix)
{
	size_t nRows = rInTAMLData.n_rows;
	size_t nCols = rInTAMLData.n_cols;

	rOutStdMatrix.resize(nRows);
	for (size_t i = 0; i < nRows; i++)
	{
		rOutStdMatrix[i].resize(nCols);
		for (size_t j = 0; j < nCols; j++)
		{
			rOutStdMatrix[i][j] = rInTAMLData.at(i, j);
		}
	}
}

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif /// EA_D7814E79_AF7C_4324_9D2C_C4D9C80002F0__INCLUDED_