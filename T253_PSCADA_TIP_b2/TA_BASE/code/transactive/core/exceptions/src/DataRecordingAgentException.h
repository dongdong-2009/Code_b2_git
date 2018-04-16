/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/DataRecordingAgentException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if !defined DATA_RECORDING_AGENT_EXCEPTION
#define DATA_RECORDING_AGENT_EXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
	class DataRecordingAgentException : public TransactiveException
	{
	public:	
		DataRecordingAgentException(const std::string& desc) throw()
			: TransactiveException(desc.c_str()) {}

		virtual ~DataRecordingAgentException() throw() {}

	private:
		DataRecordingAgentException() throw();
	};
}

#endif // DATA_RECORDING_AGENT_EXCEPTION

