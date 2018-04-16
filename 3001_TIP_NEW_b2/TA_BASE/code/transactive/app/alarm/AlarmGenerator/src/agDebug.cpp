#include <windows.h>
#include <string>

CRITICAL_SECTION cs;

void agDebugInit()
{
	//InitializeCriticalSection(&cs);
}


void agDebug1(LPCSTR strMsg)
{
	std::string str;
	
	str = "AG: ";
	str = str + strMsg;
	OutputDebugStr(str.c_str());
}

void agDebug2(LPCSTR strMsg, LPCSTR str2)
{
	std::string str;
	
	str = "AG: ";
	str = str + strMsg + str2;
	OutputDebugStr(str.c_str());
}

void agDebug2(LPCSTR strMsg, unsigned long int2)
{
	char strInt[20];
	
	ltoa(int2,strInt,10);
	agDebug2(strMsg,strInt);
}

void agDebug(LPCSTR strMsg)
{
//	EnterCriticalSection(&cs);
	agDebug1(strMsg);
//	LeaveCriticalSection(&cs);
}

void agDebug(LPCSTR strMsg, LPCSTR str2)
{
//	EnterCriticalSection(&cs);
	agDebug2(strMsg, str2);
//	LeaveCriticalSection(&cs);
}

void agDebug(LPCSTR strMsg, unsigned long int2)
{
	//EnterCriticalSection(&cs);
	agDebug2(strMsg, int2);
	//LeaveCriticalSection(&cs);
}
