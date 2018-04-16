/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/ModeExecutionException.h $
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
#if !defined(APP_BAS_BASAgent_ModeExecutionException_H)
#define APP_BAS_BASAgent_ModeExecutionException_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_App
{
	class ModeExecutionException : public TA_Base_Core::TransactiveException
	{
	public:
		enum ModeFailureType
		{
			MODE_NOT_IN_SINGLE_CONTROL,
			MODE_IN_PROGRESS,
			UNDEFINE_FAILURE_TYPE
		};

		ModeExecutionException(ModeFailureType failType, const std::string& msg) throw();
		virtual ~ModeExecutionException() throw();

		virtual const char* what() const throw();
		const ModeFailureType getFailureType() const;

	private:
		ModeExecutionException() throw();

		const std::string		m_msg;
		ModeFailureType			m_failureType;	
	};
}
#endif //APP_BAS_BASAgent_ModeExecutionException_H