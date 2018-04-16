#if !defined (TA_3002_App_Bas_BasManager_BasManagerActionThread_H)
#define TA_3002_App_Bas_BasManager_BasManagerActionThread_H
#pragma once

#include "core/threads/src/Thread.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

Namespace_TA_IRS_APP_Begin
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
Namespace_TA_IRS_APP_End
#endif //TA_3002_App_Bas_BasManager_BasManagerActionThread_H