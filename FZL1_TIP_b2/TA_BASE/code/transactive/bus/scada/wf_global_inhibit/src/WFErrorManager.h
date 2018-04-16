#ifndef WF_ERROR_MANAGER_H
#define WF_ERROR_MANAGER_H

#include <string>
#include <map>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class WfErrorManager
	{
	public:
		virtual ~WfErrorManager();

		static WfErrorManager& getInstance();
		static void freeInstance();	

		std::string getErrorMsg(unsigned long errorCode);
	private:
		WfErrorManager();
			
		static WfErrorManager* m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	
		std::map<unsigned long, std::string> databaseErrorMsg;
	};
}

#endif
