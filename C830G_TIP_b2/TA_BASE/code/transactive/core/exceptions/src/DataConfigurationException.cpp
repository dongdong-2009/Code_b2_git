/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/DataConfigurationException.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This file outlines a simple exception class for use with
  * database code. It is derived from TransActiveException.
  * This exception class is used when errors are encountered 
  * in the data that is to be written to the database.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "DataConfigurationException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    DataConfigurationException::DataConfigurationException(const char *msg, std::vector<std::string> fieldNames)
		throw(): TransactiveException(msg), m_fields(fieldNames)
	{
		logException();
	}


	DataConfigurationException::~DataConfigurationException() throw()
	{

	}

	//////////////////////////////////////////////////////////////////////
	// Member Functions
	//////////////////////////////////////////////////////////////////////

	void DataConfigurationException::logException()
	{
        for (std::vector<std::string>::iterator iter = m_fields.begin(); iter != m_fields.end(); iter++)
        {
            LOGMORE(SourceInfo, "%s field missing", (*iter).c_str());
        }
	}

} // Closes TA_Base_Core

