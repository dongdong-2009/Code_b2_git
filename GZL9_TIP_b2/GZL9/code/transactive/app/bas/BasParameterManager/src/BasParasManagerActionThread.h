
#if !defined(TA_GF_3002_app_bas_BasParameterManager_BasParasManagerActionThread)
#define TA_GF_3002_app_bas_BasParameterManager_BasParasManagerActionThread
#pragma once
#include "core/threads/src/Thread.h"
#include "app/bas/BasParameterManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	class CBasParameterManagerDlg;
	class BasParasManagerActionThread : public TA_Base_Core::Thread
	{
	public:
		BasParasManagerActionThread(CBasParameterManagerDlg* callback);
		~BasParasManagerActionThread(void);
	public:
		void setAction(BasParasManagerAction action);
	public:
		virtual void run();

		virtual void terminate();
	private:
		BasParasManagerAction m_action;
		CBasParameterManagerDlg* m_callback;
	};
}

#endif //TA_GF_3002_app_bas_BasParameterManager_BasParasManagerActionThread