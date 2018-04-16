#include "app/bas/BASAgent/src/ModeExecutionException.h"

namespace TA_IRS_App
{
	ModeExecutionException::ModeExecutionException() throw() :
	m_msg("Mode execution error"),
	m_failureType(UNDEFINE_FAILURE_TYPE)
	{

	}

	ModeExecutionException::ModeExecutionException(ModeFailureType failType, const std::string& msg) throw():
	m_msg(msg),
	m_failureType(failType)
	{

	}

	ModeExecutionException::~ModeExecutionException() throw()
	{

	}

	const char* ModeExecutionException::what() const throw()
	{
		return m_msg.c_str();
	}

	const ModeExecutionException::ModeFailureType ModeExecutionException::getFailureType() const
	{
		return m_failureType;
	}
}