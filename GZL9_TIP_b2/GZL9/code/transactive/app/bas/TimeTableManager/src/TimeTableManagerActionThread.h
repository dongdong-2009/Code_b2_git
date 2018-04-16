#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerActionThread)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerActionThread
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/threads/src/Thread.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
namespace TA_IRS_App
{
	class ActionThreadCallbackInterface
	{
	public:
        virtual void ActionThreadFunction(TimeTableManagerAction action) = 0;
        virtual void CancelActionThreadFunction(TimeTableManagerAction action) = 0; 
	};

	class TimeTableManagerActionThread : public TA_Base_Core::Thread
	{
	public:
		TimeTableManagerActionThread(ActionThreadCallbackInterface* callback);
		~TimeTableManagerActionThread(void);
	public:
		void setAction(TimeTableManagerAction action);

		virtual void run();

		virtual void terminate();
	public:
		ActionThreadCallbackInterface* m_callback;
		TimeTableManagerAction m_action;
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerActionThread