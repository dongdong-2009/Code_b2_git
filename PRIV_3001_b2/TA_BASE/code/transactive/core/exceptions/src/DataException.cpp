/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/DataException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */
// DataException.cpp: implementation of the DataException class.
//
//////////////////////////////////////////////////////////////////////

#include "DataException.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	DataException::DataException(const char *msg,
								 const EDataFail type,
								 const std::string& data)
		throw(): TransactiveException(msg),
          m_type(type),
		  m_data(data)
	{
		logException();
	}


	DataException::~DataException() throw()
	{

	}

	//////////////////////////////////////////////////////////////////////
	// Member Functions
	//////////////////////////////////////////////////////////////////////

	void DataException::logException()
	{
		//TODO: Add DebugUtil logging of the exception here.

	}

} // Closes TA_Base_Core

