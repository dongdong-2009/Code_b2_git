#include "TA_MLDataTypes.h"
#include "DataFormatHelper.h"

#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

using TA_Base_Core::DebugUtil;

TA_RealMatrix::TA_RealMatrix(const RealTable& rTable)
{
	setData(rTable);
}

TA_RealMatrix::TA_RealMatrix(size_t n_rows, size_t n_cols, const ta_float64 default_value)
	: arma::mat(n_rows, n_cols)
{
	this->fill(default_value);
}

void TA_RealMatrix::setData(const RealTable& rTable)
{
	// get the matrix size (rows and cols)
	DataFormatHelper aDataFormatHelper;
	size_t n_f_rows = rTable.size();
	size_t n_f_cols = aDataFormatHelper.getSTDTCols(rTable);
	this->set_size(n_f_rows, n_f_cols);

	aDataFormatHelper.convertStdT2DestT(rTable, (*this), n_f_cols);
}

TA_uMatrix::TA_uMatrix(size_t n_rows, size_t n_cols, const size_t default_value)
: arma::umat(n_rows, n_cols)
{
	this->fill(default_value);
}

TA_uMatrix::TA_uMatrix(const uTable& rTable)
{
	setData(rTable);
}

void TA_uMatrix::setData(const uTable& rTable)
{
	// get the matrix size (rows and cols)
	DataFormatHelper aDataFormatHelper;
	size_t n_f_rows = rTable.size();
	size_t n_f_cols = aDataFormatHelper.getSTDTCols(rTable);
	this->set_size(n_f_rows, n_f_cols);

	aDataFormatHelper.convertStdT2DestT(rTable, (*this), n_f_cols);
}


TA_uRow::TA_uRow(size_t n_rows, size_t n_cols, size_t default_value)
: arma::Row<size_t>(n_rows, n_cols)
{
	this->fill(default_value);
}

TA_uRow::TA_uRow(const uTable& rTable)
{
	setData(rTable);
}

void TA_uRow::setData(const uTable& rTable)
{
	// get the matrix size (rows and cols)
	DataFormatHelper aDataFormatHelper;
	size_t n_f_rows = rTable.size();
	size_t n_f_cols = aDataFormatHelper.getSTDTCols(rTable);
	this->set_size(n_f_rows, n_f_cols);

	aDataFormatHelper.convertStdT2DestT(rTable, (*this), n_f_cols);
}


void   TA_RealVec::push_back(const ta_float64 value)
{
	(*this) << value;
}


void   TA_uVec::push_back(const size_t value)
{
	(*this) << value;
}

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)
