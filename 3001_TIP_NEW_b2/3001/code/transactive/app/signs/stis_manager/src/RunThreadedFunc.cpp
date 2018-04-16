#include "RunThreadedFunc.h"
#include "core/utilities/src/DebugUtil.h"
#include <process.h>

static unsigned __stdcall ThreadedFuncWrapper(void* param)
{
	ThreadFuncType* func = (ThreadFuncType*)param;
	if(func != NULL)
	{
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start running threaded function");
			(*func)();
		}
		catch (...)
		{
		}
	}
	delete func;
	return 0;
}


void runThreadedFunc(ThreadFuncType func)
{
	ThreadFuncType* param = new ThreadFuncType(func);
	_beginthreadex(NULL, 0, &ThreadedFuncWrapper, (void*)param, 0, NULL);
}