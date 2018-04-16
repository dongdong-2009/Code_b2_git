#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerActionThread)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerActionThread
#if _MSC_VER > 1000
#pragma once
#endif
#include "core/threads/src/Thread.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	class ActionThreadCallbackInterface
	{
	public:
		virtual void ActionThreadFunction(TunnelModeManagerAction action) = 0; 
	};

	class TunnelModeManagerActionThread : public TA_Base_Core::Thread
	{
	public:
		TunnelModeManagerActionThread(ActionThreadCallbackInterface* callback);
		~TunnelModeManagerActionThread(void);
	public:
		void setAction(TunnelModeManagerAction action);

		virtual void run();

		virtual void terminate();
	private:
		TunnelModeManagerAction m_action;
		ActionThreadCallbackInterface* m_callback;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerActionThread