#if !defined(GF_3002_app_bas_BASAgent_ModeExecutionException_H)
#define GF_3002_app_bas_BASAgent_ModeExecutionException_H

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
#endif //GF_3002_app_bas_BASAgent_ModeExecutionException_H