
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "SimulatorManager.h"
#include <string>
#include "iostream.h"

BOOL EnablePriv()
{
    HANDLE hToken;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TOKEN_PRIVILEGES tkp;

        LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES) ,NULL,NULL);
		CloseHandle(hToken);
		unsigned long err =GetLastError() ;
        return(( err == ERROR_SUCCESS));
    }
    return TRUE;
}

// processName, titleName, msg
int ACE_TMAIN(int argc, ACE_TCHAR* argv[])  
{
	if(argc<4)
	{

		ACE_DEBUG( (LM_INFO, ACE_TEXT("version 1.027")));
		ACE_DEBUG( (LM_INFO, ACE_TEXT("not entough arg")));
		return 0;
	}
	//EnablePriv();
	SimulatorManager manager("");
	manager.setProcessName(argv[1]);
	manager.setTitleName(argv[2]);
	std::string msg;
	for(int i=3;i<argc;i++)
	{
		msg += argv[i];
		msg +="  "; 
	}
	manager.setMessage(msg);
	manager.sendMessage();
	return 0;
}  
