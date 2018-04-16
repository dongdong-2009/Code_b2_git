/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/ModeExecutionException.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
  
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