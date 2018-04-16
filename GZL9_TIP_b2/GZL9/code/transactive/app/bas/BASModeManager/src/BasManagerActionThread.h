#if !defined (APP_BAS_BasManager_BasManagerActionThread_H)
#define APP_BAS_BasManager_BasManagerActionThread_H
#pragma once

#include "core/threads/src/Thread.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

namespace TA_IRS_App
{
	class IActionThreadFunctionInterface
	{
	public:
		virtual void executeThreadFunction(BasManagerAction action) = 0;
	};

	class BasManagerActionThread : public TA_Base_Core::Thread
	{
	public:
		BasManagerActionThread(IActionThreadFunctionInterface* callback);
		~BasManagerActionThread(void);
	public:
		void setAction(BasManagerAction action);
	public:
		virtual void run();
 
		virtual void terminate();
	private:
		BasManagerActionThread(void);
	private:
		BasManagerAction m_action;
		IActionThreadFunctionInterface* m_callBackDlg;
	};
}
#endif //APP_BAS_BasManager_BasManagerActionThread_H

