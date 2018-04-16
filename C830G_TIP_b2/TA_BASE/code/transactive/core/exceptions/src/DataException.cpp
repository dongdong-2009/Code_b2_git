/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/DataException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

